#include "logic/Game.h"
#include "world/TrafficCones.h"

#include <threepp/input/KeyListener.hpp>
#include <iostream>
#include <cmath>

using namespace threepp;

// ---------------- Controls ----------------

struct Game::Controls : KeyListener {
    CarInput in;
    bool reset = false;

    void onKeyPressed(KeyEvent e) override {
        switch (e.key) {
            case Key::W: in.throttle = +1.f; break;
            case Key::S: in.throttle = -1.f; break;
            case Key::A: in.steer    = +1.f; break;
            case Key::D: in.steer    = -1.f; break;
            case Key::SPACE: in.handbrake = true; break;
            case Key::R: reset = true; break;
            default: break;
        }
    }

    void onKeyReleased(KeyEvent e) override {
        switch (e.key) {
            case Key::W:
            case Key::S: in.throttle = 0.f; break;
            case Key::A:
            case Key::D: in.steer    = 0.f; break;
            case Key::SPACE: in.handbrake = false; break;
            case Key::R: reset = false; break;
            default: break;
        }
    }
};

Game::~Game() = default;

// ---------------- Game ctor ----------------

Game::Game(Canvas& canvas, GLRenderer& renderer)
    : canvas_(canvas),
      renderer_(renderer),
      scene_(Scene::create()),
      camera_(PerspectiveCamera::create(70, canvas.aspect(), 0.1f, 1000)),
      camRig_(camera_),
      carMesh_(Mesh::create(BoxGeometry::create(1.f, 0.5f, 2.f),
                            MeshPhongMaterial::create())),
      car_(carMesh_) {

    scene_->background = Color(0x87CEEBu);

    camera_->position.set(0, 6, 18);
    scene_->add(camera_);

    auto light = DirectionalLight::create(0xffffff, 1.1f);
    light->position.set(40, 60, 40);
    scene_->add(light);

    // parkeringsplass
    addParkingLot(*scene_, spots_, lotCenter_, lotW_, lotD_);
    if (spots_.empty()) {
        std::cerr << "No parking spots created!\n";
    }

    // trafikkjegler
    addTrafficCones(*scene_, lotCenter_, lotW_, lotD_, 30, cones_);

    // dør
    doorPos_ = {0.f, 1.0f, -lotD_ * 0.5f - 2.f};
    doorHalfW_ = 3.f;

    auto doorMat = MeshPhongMaterial::create();
    doorMat->color = Color(0x5555ff);
    auto doorGeo = BoxGeometry::create(doorHalfW_ * 2.f, 2.0f, 0.5f);
    doorMesh_ = Mesh::create(doorGeo, doorMat);
    doorMesh_->position.copy(doorPos_);
    scene_->add(doorMesh_);

    // bil
    if (auto carPhong = std::dynamic_pointer_cast<MeshPhongMaterial>(carMesh_->material())) {
        carPhong->color = Color(0xff3b2fu);
    }
    scene_->add(carMesh_);

    startPos_ = {0.f, 0.25f, doorPos_.z - 8.f};
    startYaw_ = 0.f;
    car_.hardReset(startPos_, startYaw_);

    // nøkkel
    auto keyMat = MeshPhongMaterial::create();
    keyMat->color = Color(0xffff00);
    auto keyGeo = SphereGeometry::create(0.6f, 16, 12);
    keyMesh_ = Mesh::create(keyGeo, keyMat);
    keyPos_ = {
        lotCenter_.x + lotW_ * 0.5f - 3.f,
        0.6f,
        lotCenter_.z + lotD_ * 0.5f - 3.f
    };
    keyMesh_->position.copy(keyPos_);
    keyMesh_->visible = false;
    scene_->add(keyMesh_);

    // target marker
    auto targetMat = MeshPhongMaterial::create();
    targetMat->color = Color(0xffff00);
    auto targetGeo = BoxGeometry::create(0.4f, 1.5f, 0.4f);
    targetMarker_ = Mesh::create(targetGeo, targetMat);
    scene_->add(targetMarker_);

    // target-sekvens
    targetSequence_ =
        makeRandomTargetSequence(static_cast<int>(spots_.size()),
                                 requiredTargets_);
    currentTargetIdx_ = 0;
    updateTargetMarkerPosition(targetMarker_,
                               spots_[targetSequence_[currentTargetIdx_]]);

    // input
    controls_ = std::make_unique<Controls>();
    canvas_.addKeyListener(*controls_);

    // resize
    canvas_.onWindowResize([&, this](const WindowSize& size) {
        renderer_.setSize(size);
        camera_->aspect = canvas_.aspect();
        camera_->updateProjectionMatrix();
    });

    std::cout << "PARKING QUEST (Game class):\n";
    std::cout << "- Park in " << requiredTargets_
              << " random spots (yellow pole).\n";
    std::cout << "- Stay inside for " << requiredParkTime_
              << " seconds for it to count.\n";
    std::cout << "- Then collect key and drive through the door.\n";
    std::cout << "Controls: W/S/A/D, SPACE = handbrake, R = reset.\n\n";
}

// ---------------- resetGame ----------------

void Game::resetGame() {
    state_ = GameState::Playing;

    completedTargets_ = 0;
    parkedTimer_ = 0.f;
    keyAvailable_ = false;
    keyCollected_ = false;
    doorOpened_ = false;
    printedWin_ = false;
    hudAccumulator_ = 0.f;

    doorMesh_->position.copy(doorPos_);

    scene_->background = Color(0x87CEEBu);
    if (auto carPhong = std::dynamic_pointer_cast<MeshPhongMaterial>(carMesh_->material())) {
        carPhong->color = Color(0xff3b2fu);
    }

    keyMesh_->position.copy(keyPos_);
    keyMesh_->visible = false;

    for (auto& s : spots_) {
        s.completed = false;
        if (s.completeMarker) {
            scene_->remove(*s.completeMarker);
            s.completeMarker.reset();
        }
    }

    targetSequence_ = makeRandomTargetSequence(static_cast<int>(spots_.size()),
                                               requiredTargets_);
    currentTargetIdx_ = 0;
    targetMarker_->visible = true;
    updateTargetMarkerPosition(targetMarker_,
                               spots_[targetSequence_[currentTargetIdx_]]);

    car_.hardReset(startPos_, startYaw_);
}

// ---------------- update ----------------

void Game::update(float dt) {
    hudAccumulator_ += dt;

    if (controls_->reset) {
        std::cout << "Resetting game (R pressed).\n";
        resetGame();
        controls_->reset = false;
    }

    // bil & kollisjoner
    Vector3 prevPos = car_.node()->position;
    car_.update(dt, controls_->in);
    Vector3 carPos = car_.node()->position;

    {
        const float carRadius  = 0.9f;
        const float coneRadius = 0.35f;

        for (const auto& cone : cones_) {
            Vector3 cp = cone->position;
            float dx = carPos.x - cp.x;
            float dz = carPos.z - cp.z;
            float dist2 = dx * dx + dz * dz;
            float minDist = carRadius + coneRadius;

            if (dist2 < minDist * minDist) {
                car_.node()->position.copy(prevPos);
                car_.stop();
                carPos = prevPos;
                break;
            }
        }
    }

    if (state_ == GameState::Won) {
        camRig_.chase(*car_.node(), dt);

        if (!printedWin_) {
            printedWin_ = true;
            std::cout << "\n************************\n";
            std::cout << "         YOU WIN!       \n";
            std::cout << "************************\n\n";
        }

        if (hudAccumulator_ > 0.5f) {
            hudAccumulator_ = 0.f;
            std::cout << "[HUD] Speed: " << car_.speed()
                      << " m/s | Targets: " << completedTargets_
                      << "/" << requiredTargets_
                      << " | Required park time: " << requiredParkTime_ << " s\n";
        }
        return;
    }

    camRig_.chase(*car_.node(), dt);
    lastInsideTarget_ = false;

    // parkeringslogikk
    if (currentTargetIdx_ < static_cast<int>(targetSequence_.size())) {
        int spotIndex = targetSequence_[currentTargetIdx_];
        auto& spot = spots_[spotIndex];

        bool insideTarget =
            isCarInsideSpot(spot, carPos, carHalfW_, carHalfD_) &&
            std::abs(car_.speed()) < 0.4f;

        lastInsideTarget_ = insideTarget;

        auto carPhong = std::dynamic_pointer_cast<MeshPhongMaterial>(carMesh_->material());

        if (insideTarget) {
            parkedTimer_ += dt;

            float t = std::min(1.f, parkedTimer_ / requiredParkTime_);
            int r = static_cast<int>((1.f - t) * 255.f);
            int g = static_cast<int>(t * 255.f);

            if (carPhong) {
                carPhong->color = Color((r << 16) | (g << 8));
            }

            if (!spot.completed && parkedTimer_ >= requiredParkTime_) {
                spot.completed = true;
                completedTargets_++;

                auto markerMat = MeshPhongMaterial::create();
                markerMat->color = Color(0x00ff00);
                auto markerGeo2 = BoxGeometry::create(0.3f, 1.2f, 0.3f);
                auto marker = Mesh::create(markerGeo2, markerMat);
                marker->position.set(spot.center.x, 0.6f,
                                     spot.center.z - spot.halfD * 0.5f);
                scene_->add(marker);
                spot.completeMarker = marker;

                std::cout << "Target parking #" << completedTargets_
                          << " completed (spot " << spotIndex << ").\n";

                parkedTimer_ = 0.f;
                if (carPhong) {
                    carPhong->color = Color(0xff3b2fu);
                }

                currentTargetIdx_++;
                if (currentTargetIdx_ < static_cast<int>(targetSequence_.size())) {
                    updateTargetMarkerPosition(targetMarker_,
                        spots_[targetSequence_[currentTargetIdx_]]);
                } else {
                    targetMarker_->visible = false;
                }
            }
        } else {
            if (parkedTimer_ > 0.f) {
                parkedTimer_ = 0.f;
                if (carPhong) {
                    carPhong->color = Color(0xff3b2fu);
                }
            }
        }
    }

    // nøkkel
    if (!keyAvailable_ && completedTargets_ >= requiredTargets_) {
        keyAvailable_ = true;
        keyMesh_->visible = true;
        std::cout << "All target spots done! Yellow key spawned.\n";
    }

    if (keyAvailable_ && !keyCollected_) {
        float dx = carPos.x - keyMesh_->position.x;
        float dz = carPos.z - keyMesh_->position.z;
        float dist2 = dx * dx + dz * dz;
        if (dist2 < 2.0f) {
            keyCollected_ = true;
            keyMesh_->visible = false;
            std::cout << "Key collected! Door will open.\n";
        }
    }

    if (keyCollected_ && !doorOpened_) {
        doorMesh_->position.y += 3.f * dt;
        if (doorMesh_->position.y > 4.f) {
            doorOpened_ = true;
            std::cout << "Door is open! Drive through to win.\n";
        }
    }

    if (doorOpened_) {
        if (std::abs(carPos.x - doorPos_.x) <= doorHalfW_ &&
            carPos.z < doorPos_.z + 5.f && carPos.z > doorPos_.z) {

            state_ = GameState::Won;
            scene_->background = Color(0x22aa22);
            if (auto carPhong = std::dynamic_pointer_cast<MeshPhongMaterial>(carMesh_->material())) {
                carPhong->color = Color(0xffff00);
            }

            if (!printedWin_) {
                printedWin_ = true;
                std::cout << "\n************************\n";
                std::cout << "         YOU WIN!       \n";
                std::cout << "************************\n\n";
            }
        }
    }

    if (hudAccumulator_ > 0.5f) {
        hudAccumulator_ = 0.f;
        std::cout << "[HUD] Speed: " << car_.speed()
                  << " m/s | Targets: " << completedTargets_
                  << "/" << requiredTargets_
                  << " | Required park time: " << requiredParkTime_ << " s";
        if (lastInsideTarget_) {
            std::cout << " | Park hold: " << parkedTimer_
                      << " / " << requiredParkTime_ << " s";
        }
        std::cout << "\n";
    }
}

// ---------------- render ----------------

void Game::render() {
    renderer_.render(*scene_, *camera_);
}

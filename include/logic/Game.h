#pragma once

#include <threepp/threepp.hpp>
#include <vector>
#include <memory>

#include "models/Car.h"
#include "models/CameraRig.h"
#include "world/Parking.h"

enum class GameState {
    Playing,
    Won
};

class Game {
public:
    Game(threepp::Canvas& canvas, threepp::GLRenderer& renderer);
    ~Game();

    void update(float dt);
    void render();


private:
    // referanser
    threepp::Canvas& canvas_;
    threepp::GLRenderer& renderer_;

    // threepp scene
    std::shared_ptr<threepp::Scene> scene_;
    std::shared_ptr<threepp::PerspectiveCamera> camera_;
    CameraRig camRig_;

    std::shared_ptr<threepp::Mesh> carMesh_;
    Car car_;
    std::shared_ptr<threepp::Mesh> doorMesh_;
    std::shared_ptr<threepp::Mesh> keyMesh_;
    std::shared_ptr<threepp::Mesh> targetMarker_;

    std::vector<ParkingSpot> spots_;
    threepp::Vector3 lotCenter_;
    float lotW_ = 0.f;
    float lotD_ = 0.f;

    std::vector<std::shared_ptr<threepp::Mesh>> cones_;

    GameState state_ = GameState::Playing;

    const int requiredTargets_ = 3;
    int completedTargets_ = 0;
    float parkedTimer_ = 0.f;
    const float requiredParkTime_ = 1.5f;
    bool lastInsideTarget_ = false;

    std::vector<int> targetSequence_;
    int currentTargetIdx_ = 0;

    threepp::Vector3 doorPos_;
    float doorHalfW_ = 3.f;
    bool doorOpened_ = false;

    bool keyAvailable_ = false;
    bool keyCollected_ = false;
    threepp::Vector3 keyPos_;

    threepp::Vector3 startPos_;
    float startYaw_ = 0.f;

    float hudAccumulator_ = 0.f;
    bool printedWin_ = false;

    const float carHalfW_ = 0.5f;
    const float carHalfD_ = 1.0f;

    // input-håndtering (KeyListener)
    struct Controls;                       // nested type
    std::unique_ptr<Controls> controls_;   // peker til Controls

    void resetGame();
};

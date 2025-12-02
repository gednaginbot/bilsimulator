#include "world/Parking.h"

#include <random>
#include <algorithm>
#include <cmath>

using namespace threepp;

static std::shared_ptr<Group> makeParkingSpotVisual(
        Scene& scene,
        const Vector3& center,
        float width,
        float depth) {

    auto lineMat = MeshBasicMaterial::create();
    lineMat->color = Color(0xffffff);

    const float h = 0.01f;
    const float t = 0.05f;

    auto sideGeo  = BoxGeometry::create(t, h, depth);
    auto frontGeo = BoxGeometry::create(width, h, t);

    auto group = Group::create();

    auto left = Mesh::create(sideGeo, lineMat);
    left->position.set(center.x - width * 0.5f, h * 0.5f, center.z);
    group->add(left);

    auto right = Mesh::create(sideGeo, lineMat);
    right->position.set(center.x + width * 0.5f, h * 0.5f, center.z);
    group->add(right);

    auto front = Mesh::create(frontGeo, lineMat);
    front->position.set(center.x, h * 0.5f, center.z + depth * 0.5f);
    group->add(front);

    scene.add(group);
    return group;
}

void addParkingLot(Scene& scene,
                   std::vector<ParkingSpot>& spots,
                   Vector3& lotCenterOut,
                   float& lotWidthOut,
                   float& lotDepthOut) {

    const int   rows      = 12;
    const int   cols      = 24;
    const float slotW     = 2.6f;
    const float slotD     = 5.2f;
    const float laneWidth = 3.0f;
    const float margin    = 1.0f;

    const float totalW = cols * slotW + 2 * margin;
    const float totalD = rows * slotD + (rows - 1) * laneWidth + 2 * margin;

    Vector3 center{0.f, 0.f, 0.f};
    lotCenterOut = center;
    lotWidthOut  = totalW;
    lotDepthOut  = totalD;

    auto asphaltMat = MeshPhongMaterial::create();
    asphaltMat->color = Color(0x303030);
    auto asphaltGeo = PlaneGeometry::create(totalW, totalD);
    asphaltGeo->rotateX(-math::PI / 2);
    auto asphalt = Mesh::create(asphaltGeo, asphaltMat);
    asphalt->position.set(center.x, 0.0f, center.z);
    scene.add(asphalt);

    float baseX = center.x - totalW * 0.5f + margin + slotW * 0.5f;
    float baseZ = center.z - totalD * 0.5f + margin + slotD * 0.5f;

    spots.clear();
    spots.reserve(rows * cols);

    for (int r = 0; r < rows; ++r) {
        float rowZ = baseZ + r * (slotD + laneWidth);

        for (int c = 0; c < cols; ++c) {
            float x = baseX + c * slotW;
            Vector3 spotCenter{x, 0.f, rowZ};

            auto visual = makeParkingSpotVisual(scene, spotCenter, slotW, slotD);

            ParkingSpot s;
            s.center = spotCenter;
            s.halfW  = slotW * 0.5f;
            s.halfD  = slotD * 0.5f;
            s.visual = visual;
            s.completed = false;
            s.completeMarker.reset();

            spots.push_back(s);
        }
    }
}

bool isCarInsideSpot(const ParkingSpot& s,
                     const Vector3& carPos,
                     float carHalfW,
                     float carHalfD) {

    return std::abs(carPos.x - s.center.x) <= (s.halfW - carHalfW * 0.25f) &&
           std::abs(carPos.z - s.center.z) <= (s.halfD - carHalfD * 0.25f);
}

std::vector<int> makeRandomTargetSequence(int totalSpots, int count) {
    std::vector<int> indices(totalSpots);
    for (int i = 0; i < totalSpots; ++i) indices[i] = i;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(indices.begin(), indices.end(), gen);

    if (count < totalSpots) {
        indices.resize(count);
    }
    return indices;
}

void updateTargetMarkerPosition(const std::shared_ptr<Mesh>& marker,
                                const ParkingSpot& spot) {
    if (!marker) return;
    marker->position.set(
        spot.center.x,
        0.9f,
        spot.center.z - spot.halfD * 0.6f
    );
}

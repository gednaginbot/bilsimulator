// --------------------------------------------------------------------------------------
// Traffic cone placement uses std::mt19937 random distribution based on standard C++
// Examples of random usage were adapted from cppreference.com.
// Object creation and rendering follow standard threepp API usage.
// --------------------------------------------------------------------------------------

#include "world/TrafficCones.h"

#include <random>

using namespace threepp;

void addTrafficCones(Scene& scene,
                     const Vector3& lotCenter,
                     float lotW,
                     float lotD,
                     int count,
                     std::vector<std::shared_ptr<Mesh>>& outCones) {

    auto coneMat = MeshPhongMaterial::create();
    coneMat->color = Color(0xff8800);

    auto coneGeo = ConeGeometry::create(0.4f, 1.0f, 12);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> distX(
        lotCenter.x - lotW * 0.5f + 2.f,
        lotCenter.x + lotW * 0.5f - 2.f
    );
    std::uniform_real_distribution<float> distZ(
        lotCenter.z - lotD * 0.5f + 2.f,
        lotCenter.z + lotD * 0.5f - 2.f
    );

    outCones.clear();
    outCones.reserve(static_cast<std::size_t>(count));

    for (int i = 0; i < count; ++i) {
        auto cone = Mesh::create(coneGeo, coneMat);
        cone->position.set(distX(gen), 0.5f, distZ(gen));
        scene.add(cone);
        outCones.push_back(cone);
    }
}

#pragma once

#include <threepp/threepp.hpp>
#include <memory>

struct CarPhysicsParams {
    float maxSpeed  = 20.f;
    float accel     = 10.f;
    float brake     = 20.f;
    float steerRate = 1.3f;
    float friction  = 3.0f;
};

struct CarInput {
    float throttle = 0.f; // -1..+1
    float steer    = 0.f; // -1..+1
    bool  handbrake = false;
};

class Car {
public:
    explicit Car(std::shared_ptr<threepp::Object3D> node,
                 CarPhysicsParams p = {});

    void update(float dt, const CarInput& in);

    void hardReset(const threepp::Vector3& pos, float yawRad);

    void stop();

    std::shared_ptr<threepp::Object3D> node() const { return node_; }

    float speed()   const { return speed_; }
    float heading() const { return heading_; }

private:
    std::shared_ptr<threepp::Object3D> node_;
    CarPhysicsParams base_;

    float speed_   = 0.f;
    float heading_ = 0.f;
};

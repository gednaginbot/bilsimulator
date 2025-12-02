#include "models/Car.h"

#include <algorithm>
#include <cmath>

using namespace threepp;

Car::Car(std::shared_ptr<Object3D> node, CarPhysicsParams p)
    : node_(std::move(node)), base_(p) {}

void Car::update(float dt, const CarInput& in) {

    float maxSpeed  = base_.maxSpeed;
    float accel     = base_.accel;
    float friction  = base_.friction;

    // steering (mindre styring ved høy fart)
    float steerScale = std::clamp(10.f / (std::abs(speed_) + 5.f), 0.4f, 1.2f);
    heading_ += in.steer * base_.steerRate * steerScale * dt;
    node_->rotation.y = heading_;

    float a = in.throttle * accel;

    // handbrekk: bare bremser, ikke revers
    if (in.handbrake && speed_ > 0.f) {
        speed_ = std::max(0.f, speed_ - base_.brake * dt);
        if (a < 0.f) a = 0.f;
    }

    // friksjon
    if (speed_ > 0) speed_ = std::max(0.f, speed_ - friction * dt);
    if (speed_ < 0) speed_ = std::min(0.f, speed_ + friction * dt);

    // integrer & clamp (litt revers tillatt)
    speed_ = std::clamp(speed_ + a * dt, -maxSpeed * 0.25f, maxSpeed);

    float s = std::sin(heading_), c = std::cos(heading_);
    node_->position.y = 0.25f;
    node_->position.x += speed_ * s * dt;
    node_->position.z += speed_ * c * dt;
}

void Car::hardReset(const Vector3& pos, float yawRad) {
    node_->position.copy(pos);
    heading_ = yawRad;
    node_->rotation.y = yawRad;
    speed_ = 0.f;
}

void Car::stop() {
    speed_ = 0.f;
}

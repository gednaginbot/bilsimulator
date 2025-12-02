// tests/test_car.cpp
#include <catch2/catch_test_macros.hpp>
#include "models/Car.h"

using namespace threepp;

TEST_CASE("Car accelerates forward when throttle is positive") {
    auto node = Object3D::create();
    Car car(node, CarPhysicsParams{
        20.f,  // maxSpeed
        10.f,  // accel
        40.f,  // brake
        1.0f,  // steerRate
        3.0f   // friction
    });

    CarInput in;
    in.throttle = 1.f;
    in.steer = 0.f;
    in.handbrake = false;

    float dt = 0.1f;

    car.update(dt, in);

    REQUIRE(car.speed() > 0.f);
}

TEST_CASE("Car slows down due to friction when no throttle") {
    auto node = Object3D::create();
    Car car(node);

    CarInput in;
    in.throttle = 1.f;
    in.steer = 0.f;
    in.handbrake = false;

    float dt = 0.1f;

    // bygg litt fart
    for (int i = 0; i < 10; ++i) {
        car.update(dt, in);
    }

    float v_before = car.speed();

    // slippe gassen
    in.throttle = 0.f;
    car.update(dt, in);
    float v_after = car.speed();

    REQUIRE(v_after < v_before);
}

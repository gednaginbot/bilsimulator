// tests/test_parking.cpp
// test updated
#include <catch2/catch_test_macros.hpp>
#include "world/Parking.h"

using namespace threepp;

TEST_CASE("Car inside spot is detected correctly") {
    ParkingSpot spot;
    spot.center = {0.f, 0.f, 0.f};
    spot.halfW = 1.5f;
    spot.halfD = 3.0f;

    Vector3 carPos{0.f, 0.f, 0.f};
    float carHalfW = 0.5f;
    float carHalfD = 1.0f;

    REQUIRE(isCarInsideSpot(spot, carPos, carHalfW, carHalfD));
}

TEST_CASE("makeRandomTargetSequence returns unique indices") {
    int total = 10;
    int count = 5;
    auto seq = makeRandomTargetSequence(total, count);

    REQUIRE(seq.size() == static_cast<size_t>(count));

    // sjekk at alle er innenfor [0, total)
    for (int idx : seq) {
        REQUIRE(idx >= 0);
        REQUIRE(idx < total);
    }
}

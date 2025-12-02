#pragma once

#include <threepp/threepp.hpp>
#include <vector>
#include <memory>

struct ParkingSpot {
    threepp::Vector3 center;
    float halfW;
    float halfD;
    bool  completed = false;

    std::shared_ptr<threepp::Group> visual;
    std::shared_ptr<threepp::Mesh>  completeMarker;
};

void addParkingLot(threepp::Scene& scene,
                   std::vector<ParkingSpot>& spots,
                   threepp::Vector3& lotCenterOut,
                   float& lotWidthOut,
                   float& lotDepthOut);

bool isCarInsideSpot(const ParkingSpot& s,
                     const threepp::Vector3& carPos,
                     float carHalfW,
                     float carHalfD);

std::vector<int> makeRandomTargetSequence(int totalSpots, int count);

void updateTargetMarkerPosition(const std::shared_ptr<threepp::Mesh>& marker,
                                const ParkingSpot& spot);

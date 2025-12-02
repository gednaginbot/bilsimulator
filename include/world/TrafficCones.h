#pragma once

#include <threepp/threepp.hpp>
#include <vector>
#include <memory>

void addTrafficCones(threepp::Scene& scene,
                     const threepp::Vector3& lotCenter,
                     float lotW,
                     float lotD,
                     int count,
                     std::vector<std::shared_ptr<threepp::Mesh>>& outCones);

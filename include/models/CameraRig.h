#pragma once

#include <threepp/threepp.hpp>
#include <memory>

class CameraRig {
public:
    explicit CameraRig(std::shared_ptr<threepp::Camera> cam);

    void chase(const threepp::Object3D& target, float dt);

private:
    std::shared_ptr<threepp::Camera> cam_;
};

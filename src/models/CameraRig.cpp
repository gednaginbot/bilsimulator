// --------------------------------------------------------------------------------------
// Camera follow logic loosely inspired by orbit/chase examples from threepp.
// Implementation and smoothing behavior written independently for this project.
// --------------------------------------------------------------------------------------

#include "models/CameraRig.h"

using namespace threepp;

CameraRig::CameraRig(std::shared_ptr<Camera> cam)
    : cam_(std::move(cam)) {}

void CameraRig::chase(const Object3D& target, float dt) {
    const auto& p = target.position;
    float yaw = target.rotation.y;
    const float dist = 10.f, height = 3.0f;

    Vector3 desired(
        p.x - std::sin(yaw)*dist,
        p.y + height,
        p.z - std::cos(yaw)*dist
    );

    float alpha = 1.f - std::exp(-8.f * dt);
    cam_->position.lerp(desired, alpha);
    cam_->lookAt({p.x, p.y + 0.5f, p.z});
}

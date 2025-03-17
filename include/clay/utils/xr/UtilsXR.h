#pragma once
#ifdef CLAY_PLATFORM_XR

// third party
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <openxr/openxr.h>

namespace clay::utils {
    glm::mat4 computeHeadLockViewMatrix(const XrPosef& pose);

    glm::mat4 computeWorldLockViewMatrix(const XrPosef& pose, const glm::vec3& cameraPos, const glm::quat& cameraOrientation, const XrPosef& headPosition);

    glm::mat4 computeProjectionMatrix(const XrFovf& fov, float nearZ, float farZ);

    bool isRayIntersectingSphere(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& sphereCenter, float sphereRadius);

} // namespace clay::utils

#endif
// standard lib
#ifdef CLAY_PLATFORM_XR

#include <string>
#include <vector>
// third party
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// project
#include "clay/utils/xr/UtilsXR.h"

namespace clay::utils {

glm::mat4 computeHeadLockViewMatrix(const XrPosef& pose) {
    return glm::mat4_cast(glm::conjugate(glm::quat(pose.orientation.w, pose.orientation.x, pose.orientation.y, pose.orientation.z)));
}

glm::mat4 computeWorldLockViewMatrix(const XrPosef& eyePose, const glm::vec3& cameraPosition, const glm::quat& cameraOrientation, const XrPosef& headPose) {
    const glm::quat eyeOrientation(eyePose.orientation.w, eyePose.orientation.x, eyePose.orientation.y, eyePose.orientation.z);
    const glm::vec3 eyePosition(eyePose.position.x, eyePose.position.y, eyePose.position.z);
    const glm::vec3 headPosition(headPose.position.x, headPose.position.y, headPose.position.z);

    const glm::vec3 rotatedHeadPose = cameraOrientation * headPosition;
    const glm::vec3 rotatedEyePos = cameraOrientation * (eyePosition - headPosition);

    const glm::vec3 eyePositionFinal = rotatedHeadPose + cameraPosition + rotatedEyePos;
    const glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -(eyePositionFinal));

    // Combine rotation and translation for the final view matrix
    return glm::mat4_cast(glm::conjugate(cameraOrientation * eyeOrientation)) * translationMatrix;
}

glm::mat4 computeProjectionMatrix(const XrFovf& fov, float nearZ, float farZ) {
    // Calculate the frustum bounds at the near plane
    const float left = tan(fov.angleLeft) * nearZ;
    const float right = tan(fov.angleRight) * nearZ;
    const float bottom = tan(fov.angleDown) * nearZ;
    const float top = tan(fov.angleUp) * nearZ;

    // Use glm::frustum to generate the projection matrix
    return glm::frustum(left, right, bottom, top, nearZ, farZ);
}

bool isRayIntersectingSphere(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& sphereCenter, float sphereRadius) {
    const glm::vec3 oc = rayOrigin - sphereCenter;

    // Quadratic coefficients
    const float a = glm::dot(rayDir, rayDir);
    const float b = 2.0f * glm::dot(oc, rayDir);
    const float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;

    // Discriminant to check for intersection
    const float discriminant = b * b - 4.0f * a * c;
    return discriminant >= 0;
}

} // namespace clay::utils

#endif
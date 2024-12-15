// class
#include "clay/graphics/Camera.h"

namespace clay {

Camera::Camera(const glm::vec3& position) {
    mPosition_ = position;
    mForward_ = glm::vec3(0.0f, 0.0f, 1.0f);
    mUp_ = glm::vec3(0.0f, 1.0f, 0.0f);
    mWorldUp_ = glm::vec3(0.0f, 1.0f, 0.0f);
    mRight_ = glm::normalize(glm::cross(mForward_, mWorldUp_));
    updateCameraVectors();
}

Camera::~Camera() {}

void Camera::update(float dt) {}

void Camera::move(const glm::vec3& dir, const float step) {
    mPosition_ += (glm::normalize(dir) * step);
}

void Camera::rotate(const glm::vec3& axis, const float angle) {
    mRotation_ += (axis * angle);
    updateCameraVectors();
}

void Camera::setFOV(const float newFOV) {
    mFOV_ = newFOV;
}

void Camera::setAspectRatio(float newAspect) {
    mAspectRatio_ = newAspect;
}

void Camera::setPosition(const glm::vec3& newPosition) {
    mPosition_ = newPosition;
}

void Camera::setRotation(const glm::vec3& newRotation) {
    mRotation_ = newRotation;
    updateCameraVectors();
}

void Camera::zoom(const float zoomAdjust) {
    mFOV_ += zoomAdjust;
    if (mFOV_ < 0) {
        mFOV_ = 0;
    }
}

void Camera::setMode(const CameraMode mode) {
    mMode_ = mode;
}

glm::mat4 Camera::getProjectionMatrix() const {
    if (mMode_ == CameraMode::PERSPECTIVE) {
        return glm::perspective(glm::radians(mFOV_), mAspectRatio_, 0.1f, 100.0f);
    } else if (mMode_ == CameraMode::ORTHOGONAL) {
        // 4 x 3
        return glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
    }
    return glm::mat4(1);
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(mPosition_, mPosition_ + mForward_, mUp_);
}

glm::vec3 Camera::getRotation() const {
    return mRotation_;
}

glm::vec3 Camera::getPosition() const {
    return mPosition_;
}

glm::vec3 Camera::getForward() const {
    return mForward_;
}

glm::vec3 Camera::getRight() const {
    return mRight_;
}

glm::vec3 Camera::getUp() const {
    return mUp_;
}


float Camera::getFOV() const {
    return mFOV_;
}

float Camera::getAspectRatio() const {
    return mAspectRatio_;
}

Camera::CameraMode Camera::getMode() const {
    return mMode_;
}

void Camera::setMoveSpeed(float newSpeed) {
    mMoveSpeed_ = newSpeed;
}

void Camera::setRotationSpeed(float newSpeed) {
    mRotationSpeed_ = newSpeed;
}

void Camera::setZoomSpeed(float newSpeed) {
    mZoomSpeed_ = newSpeed;
}

float Camera::getMoveSpeed() const {
    return mMoveSpeed_;
}

float Camera::getRotationSpeed() const {
    return mRotationSpeed_;
}

float Camera::getZoomSpeed() const {
    return mZoomSpeed_;
}

void Camera::updateCameraVectors() {
    // Start with a default forward vector (looking down the negative Z-axis)
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);

    // Create the rotation matrix and apply rotations for yaw (Y-axis), pitch (X-axis), and roll (Z-axis)
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(mRotation_.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(mRotation_.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(mRotation_.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Roll

    // Apply the rotation to the forward vector
    forward = glm::vec3(rotationMatrix * glm::vec4(forward, 1.0f));

    // Normalize the forward vector
    mForward_ = glm::normalize(forward);

    // Re-calculate Right and Up vectors based on the new forward vector
    mRight_ = glm::normalize(glm::cross(mForward_, mWorldUp_));
    mUp_ = glm::normalize(glm::cross(mRight_, mForward_));
}

} // namespace clay
// class
#include "clay/entity/render/BaseRenderable.h"

namespace clay {

void BaseRenderable::setEnabled(const bool isEnabled) {
    mEnabled_ = isEnabled;
}

bool BaseRenderable::isEnabled() const {
    return mEnabled_;
}

glm::vec3 BaseRenderable::getPosition() const {
    return mPosition_;
}

glm::quat BaseRenderable::getOrientation() const {
    return mOrientation_;
}

glm::quat &BaseRenderable::getOrientation() {
    return mOrientation_;
}

glm::vec3 BaseRenderable::getScale() const {
    return mScale_;
}

glm::vec4 BaseRenderable::getColor() const {
    return mColor_;
}

void BaseRenderable::setPosition(const glm::vec3& newPosition) {
    mPosition_ = newPosition;
}

void BaseRenderable::setOrientation(const glm::quat& newOrientation) {
    mOrientation_ = newOrientation;
}

void BaseRenderable::setScale(const glm::vec3& newScale) {
    mScale_ = newScale;
}

void BaseRenderable::setColor(const glm::vec4& newColor) {
    mColor_ = newColor;
}

void BaseRenderable::setColor(unsigned int newColor) {
    constexpr float inv255 = 1.0f / 255.0f;

    float red   = static_cast<float>((newColor >> 24) & 0xFF) * inv255;
    float green = static_cast<float>((newColor >> 16) & 0xFF) * inv255;
    float blue  = static_cast<float>((newColor >> 8)  & 0xFF) * inv255;
    float alpha = static_cast<float>(newColor & 0xFF) * inv255;

    mColor_ = glm::vec4(red, green, blue, alpha);
}

} // namespace clay
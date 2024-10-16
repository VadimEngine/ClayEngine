// class
#include "Clay/Entity/Render/BaseRenderable.h"

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

glm::vec3 BaseRenderable::getRotation() const {
    return mRotation_;
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

void BaseRenderable::setRotation(const glm::vec3& newRotation) {
    mRotation_ = newRotation;
}

void BaseRenderable::setScale(const glm::vec3& newScale) {
    mScale_ = newScale;
}

void BaseRenderable::setColor(const glm::vec4& newColor) {
    mColor_ = newColor;
}

void BaseRenderable::setColor(int newColor) {
    float red = ((newColor >> 24) & 0xFF) / 255.0f;
    float green = ((newColor >> 16) & 0xFF) / 255.0f;
    float blue = ((newColor >> 8) & 0xFF) / 255.0f;
    float alpha = (newColor & 0xFF) / 255.0f;

    mColor_ = glm::vec4(red, green, blue, alpha);
}

} // namespace clay
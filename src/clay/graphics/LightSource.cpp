// class
#include "clay/graphics/LightSource.h"

namespace clay {

LightSource::LightSource(LightSource::Type type)
    : type_(type),
     position_(glm::vec3(0.0f)),
     direction_(glm::vec3(0.0f, -1.0f, 0.0f)),
     color_(glm::vec4(1.0f)),
     intensity_(1.0f),
     constantAttenuation_(1.0f),
     linearAttenuation_(0.0f),
     quadraticAttenuation_(0.0f) {}

LightSource::~LightSource() {}

void LightSource::setPosition(const glm::vec3& position) {
    position_ = position;
}

void LightSource::setDirection(const glm::vec3& direction) {
    direction_ = direction;
}

void LightSource::setColor(const glm::vec4& color) {
    color_ = color;
}

void LightSource::setColor(int color) {
    float red = ((color >> 24) & 0xFF) / 255.0f;
    float green = ((color >> 16) & 0xFF) / 255.0f;
    float blue = ((color >> 8) & 0xFF) / 255.0f;
    float alpha = (color & 0xFF) / 255.0f;

    color_ = glm::vec4(red, green, blue, alpha);
}

void LightSource::setIntensity(float intensity){
    intensity_ = intensity;
}

void LightSource::setAttenuation(float constant, float linear, float quadratic) {
    constantAttenuation_ = constant;
    linearAttenuation_ = linear;
    quadraticAttenuation_ = quadratic;
}

LightSource::Type LightSource::getType() const {
    return type_;
}

glm::vec3 LightSource::getPosition() const {
    return position_;
}

glm::vec3 LightSource::getDirection() const {
    return direction_;
}

glm::vec4 LightSource::getColor() const {
    return color_;
}

float LightSource::getIntensity() const {
    return intensity_;
}

} // namespace clay
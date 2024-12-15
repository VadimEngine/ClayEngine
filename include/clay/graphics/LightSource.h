#pragma once
// third party
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace clay {

class LightSource {
public:
    enum class Type { Directional, Point, Spot };

    LightSource(Type type);

    ~LightSource();

    void setPosition(const glm::vec3& position);
    void setDirection(const glm::vec3& direction);
    void setColor(const glm::vec4& color);
    void setColor(int color);
    void setIntensity(float intensity);
    void setAttenuation(float constant, float linear, float quadratic);

    Type getType() const;
    glm::vec3 getPosition() const;
    glm::vec3 getDirection() const;
    glm::vec4 getColor() const;
    float getIntensity() const;

private:
    Type type_;
    glm::vec3 position_;
    glm::vec3 direction_;
    glm::vec4 color_;
    float intensity_;
    float constantAttenuation_;
    float linearAttenuation_;
    float quadraticAttenuation_;
};

} // namespace clay
#pragma once
// third party
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// project
#include "clay/graphics/Renderer.h"

namespace clay {

class BaseRenderable {
public:
    /** Virtual Destructor*/
    virtual ~BaseRenderable() = default;
    /**
     * Rendering this renderable component
     * @param theRenderer Helping Object for rendering
     */
    virtual void render(const Renderer& theRenderer, const glm::mat4& parentModelMat) const = 0;

    /**
     * Set if this Renderable is enabled
     * @param isEnabled New enabled value
     */
    void setEnabled(const bool isEnabled);

    /** Get if this Renderable is enabled */
    bool isEnabled() const;

    /** Get this Renderable's position */
    glm::vec3 getPosition() const;

    /** Get this Renderable's rotation */
    glm::vec3 getRotation() const;

    /** Get this Renderable's scale */
    glm::vec3 getScale() const;

    /** Get the color applied to this Renderable */
    glm::vec4 getColor() const;

    /**
     * Set this Renderable's position
     * @param newPosition New position vector
     */
    void setPosition(const glm::vec3& newPosition);

    /**
     * Set this Renderable's rotation (In degrees)
     * @param newRotation New Rotation vector
     */
    void setRotation(const glm::vec3& newRotation);

    /**
     * Set this Renderable's scale
     * @param newScale New scale vector
     */
    void setScale(const glm::vec3& newScale);

    /**
     * Set the color of this renderable
     * @param newColor Color of this Renderable (rgba)
     */
    void setColor(const glm::vec4& newColor);

    /**
     * Set the color of this renderable
     * @param newColor Color of this Renderable in hex (rgba)
     */
    void setColor(int newColor);

protected:
    /** Renderable Position */
    glm::vec3 mPosition_ = {0.0f, 0.0f, 0.0f};
    /** Renderable Rotation */
    glm::vec3 mRotation_ = { 0.0f, 0.0f, 0.0f };
    /** Renderable Scale */
    glm::vec3 mScale_ = { 1.0f, 1.0f, 1.0f };
    /** Color applied to this renderable*/
    glm::vec4 mColor_ = {1.0f, 1.0f, 1.0f, 1.0f};
    /** If this Renderable is enabled (should be rendered)*/
    bool mEnabled_ = true;
};

} // namespace clay
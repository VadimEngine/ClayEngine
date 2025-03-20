#pragma once
// standard lib
#include <string>
// third party
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
// project
#include "clay/graphics/common/Font.h"
#include "clay/graphics/common/ShaderProgram.h"
#include "clay/graphics/common/SpriteSheet.h"

namespace clay {

class IRenderer {
public:
    virtual ~IRenderer() = default;

    void setCameraUBO(unsigned int cameraUBO);

    unsigned int getCurrentCameraUBO() const;

    unsigned int getCurrentLightUBO() const;


    virtual void renderTextNormalized(
        const std::string& text,
        const glm::mat4& modelMat,
        const Font& font,
        const glm::vec3& scale,
        const glm::vec3& color) = 0;

    /**
     * Render the given Texture with the applied camera and model transforms
     * @param textureId Texture Id to Render
     * @param modelMat Model matrix to transform the sprite by
     */
    virtual void renderSprite(unsigned int textureId, const glm::mat4& modelMat, const glm::vec4& theColor = {1,1,1,1}) const = 0;

    virtual void renderSprite(unsigned int textureId, ShaderProgram& shader, const glm::mat4& modelMat, const glm::vec4& theColor = {1,1,1,1}) const = 0;

    /**
     * Render the given Sprite from a sprite sheet with the applied camera and model transforms.
     * @param theSprite The sprite to render
     * @param modelMat Model matrix to transform the sprite by
     */
    virtual void renderSprite(SpriteSheet::Sprite& theSprite, const glm::mat4& modelMat, const glm::vec4& theColor = {1,1,1,1}) const = 0;

    virtual void renderSprite(SpriteSheet::Sprite& theSprite, ShaderProgram& shader, const glm::mat4& modelMat, const glm::vec4& theColor = {1,1,1,1}) const = 0;


protected:
    unsigned int mCurrentCameraUBO_;
    unsigned int mCurrentLightUBO_;

};

}
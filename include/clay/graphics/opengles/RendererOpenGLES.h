#pragma once

#ifdef CLAY_ENABLE_OPENGL_ES

// project
#include "clay/graphics/common/IRenderer.h"
#include "clay/graphics/common/ShaderProgram.h"
#include "clay/graphics/common/Font.h"
#include "clay/graphics/common/Mesh.h"


namespace clay {

class RendererOpenGLES : public IRenderer {
public:
    RendererOpenGLES(const glm::vec2& screenDim, IGraphicsAPI& graphicsApi, ShaderProgram& textShader, ShaderProgram& spriteShader, Mesh& rectPlane);

    void renderTextNormalized(const std::string& text,
                              const glm::mat4& modelMat,
                              const Font& font,
                              const glm::vec3& scale,
                              const glm::vec3& color) override;

    void renderSprite(unsigned int textureId, const glm::mat4& modelMat, const glm::vec4& theColor = {1,1,1,1}) const override;

    void renderSprite(unsigned int textureId, ShaderProgram& shader, const glm::mat4& modelMat, const glm::vec4& theColor = {1,1,1,1}) const override;

    void renderSprite(SpriteSheet::Sprite& theSprite, const glm::mat4& modelMat, const glm::vec4& theColor = {1,1,1,1}) const override;

    void renderSprite(SpriteSheet::Sprite& theSprite, ShaderProgram& shader, const glm::mat4& modelMat, const glm::vec4& theColor = {1,1,1,1}) const override;


    void updateCameraUBO(const glm::mat4& view, const glm::mat4& projection);

    void updateCameraWorldLockedUBO(const glm::mat4& view, const glm::mat4& projection);

    void updateCameraHeadLockedUBO(const glm::mat4& view, const glm::mat4& projection);

    unsigned int getCameraWorldLockedUBO() const;

    unsigned int getCameraHeadLockedUBO() const;

private:
    IGraphicsAPI& mGraphicsAPI_;
    const ShaderProgram& mTextShader_;
    const ShaderProgram& mSpriteShader_;

    Mesh mRectPlane_;

    unsigned int mCameraUBO_;

    unsigned int mCameraWorldLockedUBO_;
    unsigned int mCameraHeadLockedUBO_;


    /* TODO add HRD
    unsigned int hdrFBO_;
    unsigned int colorBuffers_[2];
    unsigned int pingpongFBO_[2];
    unsigned int pingpongColorbuffers_[2];
    unsigned int mAttachments_[2];
    */
};

} // namespace clay

#endif
#pragma once
// third party
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
// project
#include "clay/graphics/Camera.h"
#include "clay/graphics/Font.h"
#include "clay/graphics/LightSource.h"
#include "clay/graphics/Mesh.h"
#include "clay/graphics/Shader.h"
#include "clay/graphics/SpriteSheet.h"
#include "clay/graphics/Texture.h"

namespace clay {

class Renderer {
public:
    /** Constructor */

    /**
     * @brief Construct a new Renderer object
     *
     * @param screenDim Screen dimensions
     * @param spriteShader Shader for rendering spites
     * @param text2Shader Shader for rendering text
     * @param mvpShader shader for rendering simple shapes
     * @param rectPlane Mesh for a simple rect shape
     */
    Renderer(const glm::vec2& screenDim, Shader& spriteShader,
        Shader& text2Shader, Shader& mvpShader, Mesh& rectPlane, Shader& frameBufferShader, Shader& bloomFinalShader);

    /** Destructor*/
    ~Renderer();

    /**
     * @brief Update the camera used for this render
     *
     * @param camera Camera to render with
     */
    void setCamera(const Camera* camera);

    void setLightSources(const std::vector<std::unique_ptr<LightSource>>& lights) const;

    void setLightSources(const std::vector<LightSource*>& lights) const;

    /**
     * Render the given Texture with the applied camera and model transforms
     * @param textureId Texture Id to Render
     * @param modelMat Model matrix to transform the sprite by
     */
    void renderSprite(unsigned int textureId, const glm::mat4& modelMat, const glm::vec4& theColor = {1,1,1,1}) const;

    /**
     * Render the given Sprite from a sprite sheet with the applied camera and model transforms.
     * @param theSprite The sprite to render
     * @param modelMat Model matrix to transform the sprite by
     */
    void renderSprite(SpriteSheet::Sprite& theSprite, const glm::mat4& modelMat, const glm::vec4& theColor = {1,1,1,1}) const;

    /**
     * Render text at the given location. TODO scale.x scale.y
     * @param text The sprite to render
     * @param position Screen position to render (not normalized)
     * @param scale Scale of the text
     * @param color Color of the rendered text
     */
    void renderText(const std::string& text, const glm::vec2& position, const Font& font, float scale, const glm::vec3& color);

    /**
     * @brief Draw text with a transformation and relative to a camera
     *
     * @param text The sprite to render
     * @param modelMat Transformation to apply to the rendering text
     * @param font font for the drawn text
     * @param scale Text scale
     * @param color Text color
     */
    void renderTextNormalized(const std::string& text, const glm::mat4& modelMat, const Font& font, const glm::vec3& scale, const glm::vec3& color);

    /**
     * Render text centered at the given location.
     * @param text The sprite to render
     * @param position Screen position to render (not normalized)
     * @param scale Scale of the text
     * @param color Color of the rendered text
     */
    void renderTextCentered(const std::string& text, const glm::vec2& position, const Font& font, float scale, const glm::vec4& color);

    // Render line (coords, color, thickness)
    // Render shapes square/circle/triangle (hollow/color/thickness)

    void renderRectangleSimple(const glm::mat4& modelMat, const glm::vec4& theColor) const;

    void renderLineSimple(const glm::vec3& startPoint, const glm::vec3& endPoint, const glm::mat4& modelMat, const glm::vec4& theColor) const;

    /**
     * @brief Get the ID for the Renderer's HDR Frame Buffer Object
     *
     * @return GLuint HDR FBO
     */
    GLuint getHDRFBO() const;

    /**
     * @brief Render the combination of the Scene and Bloom
     *
     */
    void renderHDR();

    /**
     * @brief Enable/disable bloom rendering
     *
     * @param enable if enable
     */
    void setBloom(bool enable);

    /**
     * @brief Set the Exposure applied when rendering with bloom
     *
     * @param newExposure
     */
    void setExposure(float newExposure);

    /**
     * @brief Get the applied Exposure
     */
    float getExposure() const;

    /**
     * @brief Enable/Disable Gamma correction
     *
     * @param enable Gamma correction enable
     */
    void enableGammaCorrect(bool enable);

    /**
     * @brief Clear the scene and bloom buffer
     *
     * @param color0 clear color for scene buffer
     * @param color1 clear color for bloom buffer
     */
    void clearBuffers(const glm::vec4& color0, const glm::vec4 color1);

private:
    /** Max number of light that can be rendered with */
    const static int MAX_LIGHTS;

    /* VAO for a texture quad **/
    unsigned int mTextureQuadVAO_;
    /** Shader for rendering sprites/textures */
    const Shader& mMVPShader_;
    /** Shader used to render sprites */
    const Shader& mSpriteShader_;
    /** Shader for rendering Texts */
    const Shader& mTextShader_;

    Mesh mRectPlane_;

    unsigned int mRectVAO_;

    unsigned int mLineVAO_;
    unsigned int mLineVBO_;

    glm::mat4 mDefaultProjection_;
    /** Uniform buffer object to hold camera uniform variables shared by shaders */
    GLuint mCameraUBO_;
    /** Uniform buffer object to hold light uniform variables shared by shaders */
    GLuint mLightUBO_;


    unsigned int mFrameVAO_;


    GLuint hdrFBO_;
    GLuint colorBuffers_[2];

    GLuint pingpongFBO_[2];
    GLuint pingpongColorbuffers_[2];

    const Shader& mBlurShader_;

    const Shader& mBloomFinalShader_;

    unsigned int mAttachments_[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    float mExposure_ = 1.0f;

    bool mGammaCorrect_ = true;

};

} // namespace clay
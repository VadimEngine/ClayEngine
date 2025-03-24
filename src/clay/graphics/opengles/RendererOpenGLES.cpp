#pragma once
#ifdef CLAY_ENABLE_OPENGL_ES

//project
#include "clay/graphics/opengles/RendererOpenGLES.h"

namespace clay {

RendererOpenGLES::RendererOpenGLES(const glm::vec2& screenDim, IGraphicsAPI& graphicsAPI, ShaderProgram& textShader, ShaderProgram& spriteShader, Mesh& rectPlane)
    : mGraphicsAPI_(graphicsAPI), mTextShader_(textShader), mSpriteShader_(spriteShader), mRectPlane_(rectPlane)  {
    // TODO add HDR support

    mGraphicsAPI_.genBuffer(1, &mCameraHeadLockedUBO_);
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, mCameraHeadLockedUBO_);
    mGraphicsAPI_.bufferData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr, IGraphicsAPI::DataUsage::STATIC_DRAW);
    // define the range of the buffer that links to a uniform binding point
    mGraphicsAPI_.bindBufferRange(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0, mCameraHeadLockedUBO_, 0, 2 * sizeof(glm::mat4));
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0);

    mGraphicsAPI_.genBuffer(1, &mCameraWorldLockedUBO_);
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, mCameraWorldLockedUBO_);
    mGraphicsAPI_.bufferData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr, IGraphicsAPI::DataUsage::STATIC_DRAW);
    // define the range of the buffer that links to a uniform binding point
    mGraphicsAPI_.bindBufferRange(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0, mCameraWorldLockedUBO_, 0, 2 * sizeof(glm::mat4));
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0);

    mCurrentCameraUBO_ = mCameraWorldLockedUBO_;
}

void RendererOpenGLES::renderTextNormalized(const std::string& text,
                                            const glm::mat4& modelMat,
                                            const Font& font,
                                            const glm::vec3& scale,
                                            const glm::vec3& color) {
    // activate corresponding render state
    mTextShader_.bind();
    mTextShader_.bindUniformBuffer(0, mCurrentCameraUBO_);
    mTextShader_.setVec3("textColor", color);
    mGraphicsAPI_.activeTexture(0);
    mGraphicsAPI_.bindVertexArray(font.getVAO());

    // Calculate the total width of the text
    float totalWidth = 0.0f;
    for (const char& c : text) {
        const Font::Character* ch = font.getCharInfo(c);
        if (ch != nullptr) {
            totalWidth += (ch->advance >> 6) * scale.x;
        }
    }

    float startX = -totalWidth / 2.0f; // Center horizontally around the origin

    // iterate through all characters
    for (const char& c : text) {
        const Font::Character* ch = font.getCharInfo(c);

        if (ch != nullptr) {
            float xpos = startX + ch->bearing.x * scale.x;
            float ypos = - (ch->size.y - ch->bearing.y) * scale.y; // Adjust for Y-axis to center vertically around the origin

            float w = ch->size.x * scale.x;
            float h = ch->size.y * scale.y;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            // render glyph texture over quad
            mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, ch->textureId);
            // update content of VBO memory
            mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, font.getVBO());
            mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

            mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, 0);
            // Apply the model matrix to the shader

            mTextShader_.setMat4("uModel", modelMat);

            // render quad
            mGraphicsAPI_.drawArrays(IGraphicsAPI::PrimitiveTopology::TRIANGLE_LIST, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            startX += (ch->advance >> 6) * scale.x; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
    }
    mGraphicsAPI_.bindVertexArray(0);
    mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, 0);
}

void RendererOpenGLES::renderSprite(unsigned int textureId, const glm::mat4 &modelMat, const glm::vec4 &theColor) const {
    mSpriteShader_.bind();
    mGraphicsAPI_.activeTexture(0);
    mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, textureId);

    mSpriteShader_.setInt("uTexture", 0);
    mSpriteShader_.setMat4("uModel", modelMat);

    mRectPlane_.render(mSpriteShader_);
}

void RendererOpenGLES::renderSprite(unsigned int textureId, ShaderProgram& shader, const glm::mat4& modelMat, const glm::vec4& theColor) const {

}

void RendererOpenGLES::renderSprite(SpriteSheet::Sprite& theSprite, const glm::mat4& modelMat, const glm::vec4& theColor) const {

}

void RendererOpenGLES::renderSprite(SpriteSheet::Sprite& theSprite, ShaderProgram& shader, const glm::mat4& modelMat, const glm::vec4& theColor) const {

}

void RendererOpenGLES::updateCameraUBO(const glm::mat4& view, const glm::mat4& projection) {
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, mCameraUBO_);

    mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
    mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0);
}

void RendererOpenGLES::updateCameraWorldLockedUBO(const glm::mat4& view, const glm::mat4& projection) {
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, mCameraWorldLockedUBO_);

    mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
    mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0);
}

void RendererOpenGLES::updateCameraHeadLockedUBO(const glm::mat4& view, const glm::mat4& projection) {
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, mCameraHeadLockedUBO_);

    mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
    mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0);
}

unsigned int RendererOpenGLES::getCameraWorldLockedUBO() const {
    return mCameraWorldLockedUBO_;
}

unsigned int RendererOpenGLES::getCameraHeadLockedUBO() const {
    return mCameraHeadLockedUBO_;
}
} // namespace clay

#endif
// class
#include "Clay/Graphics/Texture.h"

namespace clay {

Texture::Texture(const unsigned char* textureData, int width, int height, int channels, bool gammaCorrect) {
    mWidth_ = width;
    mHeight_ = height;
    mChannels_ = channels;
    mTextureId_ = genGLTexture(textureData, width, height, channels, gammaCorrect);
}

Texture::Texture(const std::filesystem::path& path, bool gammaCorrect) {
    mTextureId_ = loadTexture(path, &mWidth_, &mHeight_, &mChannels_, gammaCorrect);
}

Texture::~Texture() {
    glDeleteTextures(1, &mTextureId_);
}

unsigned int Texture::loadTexture(const std::filesystem::path& texturePath, int* width, int* height, int* channels, bool gammaCorrect) {
    // Load image file
    unsigned char* textureData = SOIL_load_image(texturePath.string().c_str(), width, height, channels, SOIL_LOAD_AUTO);

    if (textureData == nullptr) {
        LOG_E("ERROR LOADING TEXTURE: %s", textureData);
        const char* errorMessage = SOIL_last_result();
        if (errorMessage != nullptr) {
            LOG_E("SOIL error: %s", errorMessage)
        }
        throw std::runtime_error("Texture load failed");
    }

    unsigned textureId = genGLTexture(textureData, *width, *height, *channels, gammaCorrect);

    SOIL_free_image_data(textureData);
    return textureId;
}

unsigned int Texture::getId() const {
    return mTextureId_;
}

unsigned int Texture::getWidth() const {
    return mWidth_;
}

unsigned int Texture::getHeight() const {
    return mHeight_;
}

unsigned int Texture::getChannels() const {
    return mChannels_;
}

glm::ivec2 Texture::getShape() const {
    return {mWidth_, mHeight_};
}

unsigned int Texture::genGLTexture(const unsigned char* textureData, int width, int height, int channels, bool gammaCorrect) {
    unsigned int textureId;

    if (textureData == nullptr) {
        LOG_E("Null texture data cannot be convert to a texture");
        throw std::runtime_error("Texture build failed");
    }

    // create textures
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if (gammaCorrect) {
        // Use GL_SRGB or GL_SRGB_ALPHA for gamma correction
        glTexImage2D(
            GL_TEXTURE_2D, 0,
            (channels == 3) ? GL_SRGB : GL_SRGB_ALPHA,
            width, height, 0,
            (channels == 3) ? GL_RGB : GL_RGBA,
            GL_UNSIGNED_BYTE, textureData
        );
    } else {
        // Use GL_RGB or GL_RGBA for non-gamma-corrected textures
        glTexImage2D(
            GL_TEXTURE_2D, 0,
            (channels == 3) ? GL_RGB : GL_RGBA,
            width, height, 0,
            (channels == 3) ? GL_RGB : GL_RGBA,
            GL_UNSIGNED_BYTE, textureData
        );
    }

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
    GLenum err;
    if ((err = glGetError()) != GL_NO_ERROR) {
        LOG_E("genGLTexture ERROR %d", err);
    }
    return textureId;
}

} // namespace clay
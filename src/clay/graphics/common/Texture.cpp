// standard lib
#include <vector>
// third party
// project
#include "clay/utils/common/Logger.h"
// class
#include "clay/graphics/common/Texture.h"

namespace clay {

Texture::Texture(IGraphicsAPI& graphicsAPI, const unsigned char* textureData, int width, int height, int channels, bool gammaCorrect) 
: mGraphicsAPI_(graphicsAPI) {
    mWidth_ = width;
    mHeight_ = height;
    mChannels_ = channels;
    mTextureId_ = genGLTexture(graphicsAPI, textureData, width, height, channels, gammaCorrect);
}

Texture::Texture(IGraphicsAPI& graphicsAPI, utils::ImageData& imageData, bool gammaCorrect)
: mGraphicsAPI_(graphicsAPI) {
    mWidth_ = imageData.width;
    mHeight_ = imageData.height;
    mChannels_ = imageData.channels;
    mTextureId_ = genGLTexture(graphicsAPI, imageData.pixels, imageData.width, imageData.height, imageData.channels, gammaCorrect);
}

Texture::~Texture() {
    // TODO FIX ERROR WHEN THIS IS CALLED
    mGraphicsAPI_.deleteTexture(1, &mTextureId_);
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

std::vector<unsigned char> Texture::getPixelData() {
    mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D , mTextureId_);
    size_t dataSize = mWidth_ * mHeight_ * mChannels_;
    std::vector<unsigned char> pixels(dataSize);

    mGraphicsAPI_.getTexImage(
        IGraphicsAPI::TextureTarget::TEXTURE_2D,
        0,
        (mChannels_ == 3) ? IGraphicsAPI::TextureFormat::RGB : IGraphicsAPI::TextureFormat::RGBA,
        IGraphicsAPI::DataType::UBYTE,
        pixels.data()
    );
    // unbind
    mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D , 0);
    return pixels;
}

unsigned int Texture::genGLTexture(IGraphicsAPI& graphicsAPI, const unsigned char* textureData, int width, int height, int channels, bool gammaCorrect) {
    unsigned int textureId;

    if (textureData == nullptr) {
        LOG_E("Null texture data cannot be convert to a texture");
        throw std::runtime_error("Texture build failed");
    }

    // create textures
    graphicsAPI.genTextures(1, &textureId);
    graphicsAPI.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, textureId);
    graphicsAPI.texParameter(IGraphicsAPI::TextureTarget::TEXTURE_2D, IGraphicsAPI::TextureParameterType::TEXTURE_WRAP_S, IGraphicsAPI::TextureParameterOption::CLAMP_TO_BORDER);
    graphicsAPI.texParameter(IGraphicsAPI::TextureTarget::TEXTURE_2D, IGraphicsAPI::TextureParameterType::TEXTURE_WRAP_T, IGraphicsAPI::TextureParameterOption::CLAMP_TO_BORDER);
    graphicsAPI.texParameter(IGraphicsAPI::TextureTarget::TEXTURE_2D, IGraphicsAPI::TextureParameterType::TEXTURE_MIN_FILTER, IGraphicsAPI::TextureParameterOption::NEAREST);
    graphicsAPI.texParameter(IGraphicsAPI::TextureTarget::TEXTURE_2D, IGraphicsAPI::TextureParameterType::TEXTURE_MAG_FILTER, IGraphicsAPI::TextureParameterOption::NEAREST);
    if (gammaCorrect) {
        // Use GL_SRGB or GL_SRGB_ALPHA for gamma correction
        // TODO always use RGBA for internal?
        graphicsAPI.texImage2D(
            IGraphicsAPI::TextureTarget::TEXTURE_2D, 0,
            (channels == 3) ? IGraphicsAPI::TextureFormat::SRGB : IGraphicsAPI::TextureFormat::SRGB_ALPHA,
            width, height, 0,
            (channels == 3) ? IGraphicsAPI::TextureFormat::RGB : IGraphicsAPI::TextureFormat::RGBA,
            IGraphicsAPI::DataType::UBYTE,
            textureData
        );
    } else {
        // Use GL_RGB or GL_RGBA for non-gamma-corrected textures
        graphicsAPI.texImage2D(
            IGraphicsAPI::TextureTarget::TEXTURE_2D, 0,
            (channels == 3) ? IGraphicsAPI::TextureFormat::RGB : IGraphicsAPI::TextureFormat::RGBA,
            width, height, 0,
            (channels == 3) ? IGraphicsAPI::TextureFormat::RGB : IGraphicsAPI::TextureFormat::RGBA,
            IGraphicsAPI::DataType::UBYTE,
            textureData
        );
    }

    // Unbind texture
    graphicsAPI.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, 0);
    return textureId;
}

} // namespace clay
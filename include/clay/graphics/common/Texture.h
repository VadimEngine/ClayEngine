#pragma once
// standard lib
#include <filesystem>
#include <stdexcept>
// third party
#include <glm/vec2.hpp>
// project
#include "clay/graphics/common/IGraphicsAPI.h"


namespace clay {

class Texture {
public:
    /**
     * @brief Create a Texture out of given pixel data
     *
     * @param textureData Pixel data
     * @param width width in pixels
     * @param height height in pixels
     * @param channels channels per pixel
     */
    Texture(IGraphicsAPI& graphicsAPI , const unsigned char* textureData, int width, int height, int channels, bool gammaCorrect = false);

    /**
     * @brief Load a texture from a given path
     *
     * @param path
     */
    explicit Texture(IGraphicsAPI& graphicsAPI, const std::filesystem::path& path, bool gammaCorrect = false);

    /**
     * @brief Destructor. Frees the GL texture id
     */
    ~Texture();

    // TODO custom move/copy/= logic?

    /**
     * @brief Load a texture from a file and return the OpenGL Texture Id
     *
     * @param texturePath Texture file path
     * @param width Pointer to store the width in pixels of the loaded texture
     * @param height Pointer to store the height in pixels of the loaded texture
     * @param channels Pointer to store number of channels per pixel
     * @return unsigned int GL Texture Id
     */
    static unsigned int loadTexture(IGraphicsAPI& graphicsAPI, const std::filesystem::path& texturePath, int* width, int* height, int* channels, bool gammaCorrect = false);

    // static void saveImageAsBMP();

    /** Get the GL Texture Id */
    unsigned int getId() const;

    /** Get Texture width in pixels */
    unsigned int getWidth() const;

    /** Get Texture height in pixels */
    unsigned int getHeight() const;

    /** Get number of channels per pixel */
    unsigned int getChannels() const;

    /** Get Width x Height in pixels */
    glm::ivec2 getShape() const;

    std::vector<unsigned char> getPixelData();

private:
    /**
     * @brief Helper method to convert pixel data into a GL texture data
     *
     * @param textureData Pixel data
     * @param width Width in pixels
     * @param height Height in pixels
     * @param channels Channels per pixel
     * @return Gl Texture Id
     */
    static unsigned int genGLTexture(IGraphicsAPI& graphicsAPI, const unsigned char* textureData, int width, int height, int channels, bool gammaCorrect = false);


    IGraphicsAPI& mGraphicsAPI_;
    /** GL Texture Id*/
    unsigned int mTextureId_;
    /** Width in pixels*/
    int mWidth_;
    /** Height in pixels*/
    int mHeight_;
    /** Channels per pixel */
    int mChannels_;
};

} // namespace clay
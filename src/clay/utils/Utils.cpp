// standard lib
#include <vector>
// third party
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <SOIL.h>
// project
#include "clay/application/Logger.h"
// class
#include "clay/utils/Utils.h"

namespace clay::utils {
    size_t Vec2Hash::operator()(const glm::ivec2& v) const {
        return std::hash<int>()(v.x) ^ std::hash<int>()(v.y);
    }

    // void saveTextureAsBMP(unsigned int textureID, const std::filesystem::path& filepath) {
    //     // Bind the texture
    //     glBindTexture(GL_TEXTURE_2D, textureID);

    //     // Get the texture width, height, and internal format
    //     int width, height, internalFormat;
    //     glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    //     glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    //     glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);

    //     // Determine the format and the number of channels
    //     GLenum format;
    //     int channels;
    //     GLenum type = GL_UNSIGNED_BYTE; // Default to unsigned byte

    //     if (internalFormat == GL_RGB || internalFormat == GL_SRGB) {
    //         format = GL_RGB;
    //         channels = 3;
    //     } else if (internalFormat == GL_RGBA || internalFormat == GL_SRGB_ALPHA) {
    //         format = GL_RGBA;
    //         channels = 4;
    //     } else if (internalFormat == GL_RGBA16F) {
    //         format = GL_RGBA;
    //         channels = 4;
    //         type = GL_FLOAT; // For floating-point data
    //     } else {
    //         // Handle other formats if necessary
    //         LOG_E("Unsupported texture format: %d", internalFormat);
    //         glBindTexture(GL_TEXTURE_2D, 0);
    //         return;
    //     }

    //     // Allocate memory to hold the texture data
    //     std::vector<float> textureDataFloat;
    //     std::vector<unsigned char> textureData;

    //     if (type == GL_FLOAT) {
    //         textureDataFloat.resize(width * height * channels);
    //         glGetTexImage(GL_TEXTURE_2D, 0, format, type, textureDataFloat.data());

    //         // Convert float data to unsigned byte for BMP
    //         textureData.resize(width * height * channels);
    //         for (int i = 0; i < width * height * channels; ++i) {
    //             textureData[i] = static_cast<unsigned char>(glm::clamp(textureDataFloat[i] * 255.0f, 0.0f, 255.0f));
    //         }
    //     } else {
    //         textureData.resize(width * height * channels);
    //         glGetTexImage(GL_TEXTURE_2D, 0, format, type, textureData.data());
    //     }

    //     // Unbind the texture
    //     glBindTexture(GL_TEXTURE_2D, 0);

    //     // Save the texture data as a BMP file using SOIL
    //     int saveResult = SOIL_save_image(
    //         filepath.string().c_str(),
    //         SOIL_SAVE_TYPE_BMP,
    //         width, height, channels,
    //         textureData.data()
    //     );

    //     if (!saveResult) {
    //         LOG_E("Failed to save BMP file: %s", filepath.string().c_str());
    //     }
    // }

} // namespace clay::utils

#ifdef CLAY_PLATFORM_DESKTOP

// third party
#include <SOIL.h>
// project
#include "clay/utils/common/Logger.h"
// Header
#include "clay/utils/desktop/UtilsDesktop.h"
#include <glm/common.hpp>

namespace clay::utils {

    utils::FileData loadFileToMemory_desktop(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::binary | std::ios::ate); // 	seek to the end of stream immediately after open to get the file size
        if (!file) {
            throw std::runtime_error("Failed to open file: " + filePath);
        }

        std::streamsize fileSize = file.tellg(); // get file size
        file.seekg(0, std::ios::beg); // move cursor back to begining

        auto buffer = std::make_unique<unsigned char[]>(fileSize);
        if (!file.read(reinterpret_cast<char*>(buffer.get()), fileSize)) {
            throw std::runtime_error("Failed to read file: " + filePath);
        }

        return {std::move(buffer), static_cast<std::size_t>(fileSize)}; 
    }

    utils::ImageData loadImageFileToMemory_desktop(const std::string& filePath) {
        auto fileData = loadFileToMemory_desktop(filePath);
        utils::ImageData imageData;

        unsigned char* rawPixels = SOIL_load_image_from_memory(
            fileData.data.get(),
            fileData.size,
            &imageData.width,
            &imageData.height,
            &imageData.channels,
            SOIL_LOAD_AUTO
        );

        if (rawPixels == nullptr) {
            LOG_E("ERROR LOADING TEXTURE");
            const char* errorMessage = SOIL_last_result();
            if (errorMessage != nullptr) {
                LOG_E("SOIL error: %s", errorMessage);
            }
            throw std::runtime_error("Texture load failed");
        }
        // Wrap the raw pointer in a std::unique_ptr
        imageData.pixels = std::unique_ptr<unsigned char[]>(rawPixels);

        return imageData;
    }

    void saveTextureAsBMP(IGraphicsAPI& gAPI, unsigned int textureID, const std::filesystem::path& filepath) {
        gAPI.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, textureID);

        // Get the texture width, height, and internal format
        int width, height, internalFormat;
        gAPI.getTexLevelParameteriv(IGraphicsAPI::TextureTarget::TEXTURE_2D, 0, IGraphicsAPI::TextureParameterName::WIDTH, &width);
        gAPI.getTexLevelParameteriv(IGraphicsAPI::TextureTarget::TEXTURE_2D, 0, IGraphicsAPI::TextureParameterName::HEIGHT, &height);
        gAPI.getTexLevelParameteriv(IGraphicsAPI::TextureTarget::TEXTURE_2D, 0, IGraphicsAPI::TextureParameterName::INTERNAL_FORMAT, &internalFormat);

        IGraphicsAPI::TextureFormat internalFormatAPI = gAPI.intToTextureFormat(internalFormat);
        IGraphicsAPI::TextureFormat format;
        IGraphicsAPI::DataType dataType = IGraphicsAPI::DataType::UBYTE;
        int channels;

        if (internalFormatAPI == IGraphicsAPI::TextureFormat::RGB || internalFormatAPI == IGraphicsAPI::TextureFormat::SRGB) {
            format = IGraphicsAPI::TextureFormat::RGB;
            channels = 3;
        } else if (internalFormatAPI == IGraphicsAPI::TextureFormat::RGBA || internalFormatAPI == IGraphicsAPI::TextureFormat::SRGB_ALPHA) {
            format = IGraphicsAPI::TextureFormat::RGBA;
            channels = 4;
        } else if (internalFormatAPI == IGraphicsAPI::TextureFormat::RGBA16F) {
            format = IGraphicsAPI::TextureFormat::RGBA;
            channels = 4;
            dataType = IGraphicsAPI::DataType::FLOAT;
        } else {
            LOG_E("Unsupported texture format: %d", internalFormat);
            gAPI.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, 0);
            return;
        }

        // Allocate memory to hold the texture data
        std::vector<float> textureDataFloat;
        std::vector<unsigned char> textureData;

        if (dataType == IGraphicsAPI::DataType::FLOAT) {
            textureDataFloat.resize(width * height * channels);
            gAPI.getTexImage(IGraphicsAPI::TextureTarget::TEXTURE_2D, 0, format, dataType, textureDataFloat.data());
            // Convert float data to unsigned byte for BMP
            textureData.resize(width * height * channels);
            for (int i = 0; i < width * height * channels; ++i) {
                textureData[i] = static_cast<unsigned char>(glm::clamp(textureDataFloat[i] * 255.0f, 0.0f, 255.0f));
            }
        } else {
            // get image directly
            textureData.resize(width * height * channels);
            gAPI.getTexImage(IGraphicsAPI::TextureTarget::TEXTURE_2D, 0, format, dataType, textureData.data());
        }

        // Unbind the texture
        gAPI.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, 0);

        // Save the texture data as a BMP file using SOIL
        int saveResult = SOIL_save_image(
            filepath.string().c_str(),
            SOIL_SAVE_TYPE_BMP,
            width, height, channels,
            textureData.data()
        );

         if (!saveResult) {
             LOG_E("Failed to save BMP file: %s", filepath.string().c_str());
         }
    }


}// namespace clay::utils

#endif
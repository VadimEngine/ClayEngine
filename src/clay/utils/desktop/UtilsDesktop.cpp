#ifdef CLAY_PLATFORM_DESKTOP

// third party
#include <SOIL.h>
// project
#include "clay/utils/common/Logger.h"
// Header
#include "clay/utils/desktop/UtilsDesktop.h"

namespace clay::utils {

    utils::FileData loadFileToMemory_desktop(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (!file) {
            throw std::runtime_error("Failed to open file: " + filePath);
        }

        std::streamsize fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        auto buffer = std::make_unique<unsigned char[]>(fileSize);
        if (!file.read(reinterpret_cast<char*>(buffer.get()), fileSize)) {
            throw std::runtime_error("Failed to read file: " + filePath);
        }

        return {std::move(buffer), static_cast<std::size_t>(fileSize)}; 
    }

     void saveTextureAsBMP(unsigned int textureID, const std::filesystem::path& filepath) {
        // TODO use graphicsAPI class
        //  // Bind the texture
        //  glBindTexture(GL_TEXTURE_2D, textureID);

        //  // Get the texture width, height, and internal format
        //  int width, height, internalFormat;
        //  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        //  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        //  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);

        //  // Determine the format and the number of channels
        //  GLenum format;
        //  int channels;
        //  GLenum type = GL_UNSIGNED_BYTE; // Default to unsigned byte

        //  if (internalFormat == GL_RGB || internalFormat == GL_SRGB) {
        //      format = GL_RGB;
        //      channels = 3;
        //  } else if (internalFormat == GL_RGBA || internalFormat == GL_SRGB_ALPHA) {
        //      format = GL_RGBA;
        //      channels = 4;
        //  } else if (internalFormat == GL_RGBA16F) {
        //      format = GL_RGBA;
        //      channels = 4;
        //      type = GL_FLOAT; // For floating-point data
        //  } else {
        //      // Handle other formats if necessary
        //      LOG_E("Unsupported texture format: %d", internalFormat);
        //      glBindTexture(GL_TEXTURE_2D, 0);
        //      return;
        //  }

        //  // Allocate memory to hold the texture data
        //  std::vector<float> textureDataFloat;
        //  std::vector<unsigned char> textureData;

        //  if (type == GL_FLOAT) {
        //      textureDataFloat.resize(width * height * channels);
        //      glGetTexImage(GL_TEXTURE_2D, 0, format, type, textureDataFloat.data());

        //      // Convert float data to unsigned byte for BMP
        //      textureData.resize(width * height * channels);
        //      for (int i = 0; i < width * height * channels; ++i) {
        //          textureData[i] = static_cast<unsigned char>(glm::clamp(textureDataFloat[i] * 255.0f, 0.0f, 255.0f));
        //      }
        //  } else {
        //      textureData.resize(width * height * channels);
        //      glGetTexImage(GL_TEXTURE_2D, 0, format, type, textureData.data());
        //  }

        //  // Unbind the texture
        //  glBindTexture(GL_TEXTURE_2D, 0);

        //  // Save the texture data as a BMP file using SOIL
        //  int saveResult = SOIL_save_image(
        //      filepath.string().c_str(),
        //      SOIL_SAVE_TYPE_BMP,
        //      width, height, channels,
        //      textureData.data()
        //  );

        //  if (!saveResult) {
        //      LOG_E("Failed to save BMP file: %s", filepath.string().c_str());
        //  }
    }

    clay::utils::ImageData fileDataToImageData(utils::FileData& imageFile) {
        clay::utils::ImageData imageData;
        imageData.pixels = SOIL_load_image_from_memory(
            imageFile.data.get(), 
            imageFile.size,
            &imageData.width, 
            &imageData.height, 
            &imageData.channels, 
            SOIL_LOAD_AUTO
        );
    
        if (imageData.pixels == nullptr) {
            LOG_E("ERROR LOADING TEXTURE");
            const char* errorMessage = SOIL_last_result();
            if (errorMessage != nullptr) {
                LOG_E("SOIL error: %s", errorMessage)
            }
            throw std::runtime_error("Texture load failed");
        }

        return imageData;
    }


}// namespace clay::utils

#endif
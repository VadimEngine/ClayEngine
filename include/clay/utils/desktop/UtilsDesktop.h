#pragma once

#ifdef CLAY_PLATFORM_DESKTOP

// standard lib
#include <fstream>
#include <stdexcept>
// project
#include "clay/utils/common/Utils.h"

namespace clay::utils {
    utils::FileData loadFileToMemory_desktop(const std::string& filePath);

    void saveTextureAsBMP(unsigned int textureID, const std::filesystem::path& filepath);

    clay::utils::ImageData fileDataToImageData(utils::FileData& imageFile);

}// namespace clay::utils

#endif
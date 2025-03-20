#pragma once

#ifdef CLAY_PLATFORM_DESKTOP

// standard lib
#include <fstream>
#include <stdexcept>
#include <filesystem>
// project
#include "clay/utils/common/Utils.h"
#include "clay/graphics/common/IGraphicsAPI.h"

namespace clay::utils {
    utils::FileData loadFileToMemory_desktop(const std::string& filePath);

    utils::ImageData loadImageFileToMemory_desktop(const std::string& filePath);

    void saveTextureAsBMP(IGraphicsAPI& gAPI, unsigned int textureID, const std::filesystem::path& filepath);

}// namespace clay::utils

#endif
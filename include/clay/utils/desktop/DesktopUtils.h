#pragma once
// standard lib
#include <fstream>
#include <stdexcept>
// project
#include "clay/utils/common/Utils.h"

namespace clay::utils {
    utils::FileData loadFileToMemory_desktop(const std::string& filePath);
}// namespace clay::utils
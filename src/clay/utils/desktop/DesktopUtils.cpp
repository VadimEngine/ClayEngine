#include "clay/utils/desktop/DesktopUtils.h"

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

}// namespace clay::utils
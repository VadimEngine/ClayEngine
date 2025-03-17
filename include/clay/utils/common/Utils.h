#pragma once
// standard lib
#include <memory>
// third party
#include <glm/vec2.hpp>

namespace clay::utils {

    struct FileData {
        std::unique_ptr<unsigned char[]> data;
        std::size_t size;
    }; 

    struct ImageData {
        std::unique_ptr<unsigned char[]> pixels;
        int width;
        int height;
        int channels;
    };

    /** Hash for glm::ivec2 */
    struct Vec2Hash {
        size_t operator()(const glm::ivec2& v) const;
    };

} // namespace clay::utils

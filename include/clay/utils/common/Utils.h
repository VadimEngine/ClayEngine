#pragma once
// standard lib
#include <filesystem>
// third party
#include <glm/vec2.hpp>
// project

namespace clay::utils {

    struct FileData {
        std::unique_ptr<unsigned char[]> data;
        std::size_t size;
    }; 



    /** Hash for glm::ivec2 */
    struct Vec2Hash {
        size_t operator()(const glm::ivec2& v) const;
    };

    // /**
    //  * @brief Save the GL Texture as a bitmap
    //  *
    //  * @param textureID GL texture id
    //  * @param filename path to save the GL Texture
    //  */
    // void saveTextureAsBMP(unsigned int textureID, const std::filesystem::path& filepath);

} // namespace clay::utils

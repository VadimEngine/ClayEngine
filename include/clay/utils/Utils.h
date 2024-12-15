#pragma once
// standard lib
#include <vector>
#include <filesystem>
// third party
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/vec2.hpp>
#include <SOIL.h>
// project
#include "clay/application/Logger.h"

namespace clay::utils {
    /** Hash for glm::ivec2 */
    struct Vec2Hash {
        size_t operator()(const glm::ivec2& v) const;
    };

    /**
     * @brief Save the GL Texture as a bitmap
     *
     * @param textureID GL texture id
     * @param filename path to save the GL Texture
     */
    void saveTextureAsBMP(GLuint textureID, const std::filesystem::path& filepath);
} // namespace clay::utils

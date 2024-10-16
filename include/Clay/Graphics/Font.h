#pragma once
// standard lib
#include <filesystem>
#include <string>
#include <unordered_map>
// third party
#include <ft2build.h>
#include FT_FREETYPE_H
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
//project
#include "Clay/Application/Logger.h"

namespace clay {

class Font {
public:
    /**
     * Holds texture information for this font's character
     */
    struct Character {
         // ID handle of the glyph texture
        unsigned int textureId;
        // Size of glyph
        glm::ivec2 size;
        // Offset from baseline to left/top of glyph
        glm::ivec2 bearing;
         // Horizontal offset to advance to next glyph
        unsigned int advance;
    };

    /* VAO for a text rendering **/
    unsigned int mTextVAO_;
    /* VBO for a text rendering **/
    unsigned int mTextVBO_;
    /** Character information from the loaded font */
    std::unordered_map<GLchar, Character> mCharacterFrontInfo_;

    /**
     * Construct a new Font object from a tff file
     *
     * @param fontPath font tff file path
     */
    Font(const std::filesystem::path& fontPath);

    /**
     * Get the Character Info for the given char if it exists
     *
     * @param theChar char to get character information for
     * @return Character font infomation
     */
    const Character* getCharInfo(GLchar theChar) const;

    /**
     * Get the VAO for this font
     */
    unsigned int getVAO() const;

    /**
     * Get VBO for this font
     */
    unsigned int getVBO() const;

};

} // namespace clay
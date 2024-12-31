#pragma once
// standard lib
#include <filesystem>
#include <unordered_map>
// third party
#include <glm/vec2.hpp>
// project
#include <clay/graphics/common/IGraphicsAPI.h>

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

    IGraphicsAPI& mGraphicsAPI_;

    /* VAO for a text rendering **/
    unsigned int mTextVAO_;
    /* VBO for a text rendering **/
    unsigned int mTextVBO_;
    /** Character information from the loaded font */
    std::unordered_map<char, Character> mCharacterFrontInfo_;

    /**
     * Construct a new Font object from a tff file
     *
     * @param fontPath font tff file path
     */
    Font(IGraphicsAPI& graphicsAPI, const std::filesystem::path& fontPath);

    ~Font();

    /**
     * Get the Character Info for the given char if it exists
     *
     * @param theChar char to get character information for
     * @return Character font information
     */
    const Character* getCharInfo(char theChar) const;

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
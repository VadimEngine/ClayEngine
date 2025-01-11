#pragma once
// third party
#include <glm/vec2.hpp>
// project
#include "clay/graphics/common/Texture.h"

namespace clay {

class SpriteSheet {
public:
    struct Sprite {
        /** The sprite sheet this sprite is from*/
        SpriteSheet& parentSpriteSheet;
        /** The grid this sprite is on the Sprite Sheet */
        glm::ivec2 gridIndex;
        /** The size of this sprite. Can be used to draw a sprite consisting of multiple grids */
        glm::ivec2 spriteSize;

        /**
         * Constructor
         * @param theSpriteSheet Parent Sprites Sheet
         * @param theGridIndex Grid index of the Sprite Sheet
         * @param theSpriteSize Size of the sprite
         */
        Sprite(SpriteSheet& theSpriteSheet, const glm::ivec2& theGridIndex, const glm::ivec2& theSpriteSize);

        /**
         * Constructor. Sets spriteSize to be the parent SpriteSheet default sprite size
         * @param theSpriteSheet Parent Sprites Sheet
         * @param theGridIndex Grid index of the Sprite Sheet
         */
        Sprite(SpriteSheet& theSpriteSheet, const glm::ivec2& theGridIndex);
    };

    /**
     * Sprite Sheet is a wrapper around a texture to allow easily accessing a sprite
     * @param textureId Texture id for this sprite sheet
     * @param sheetSize Sprite sheet size in pixels
     * @param defaultSpiteSize Default sprite size
     */
    SpriteSheet(unsigned int textureId, const glm::ivec2& sheetSize, const glm::ivec2& defaultSpiteSize);

    /**
     * @brief Sprite Sheet is a wrapper around a texture to allow easily accessing a sprite
     *
     * @param texture Texture of sprite sheet
     * @param defaultSpiteSize Default sprite size
     */
    SpriteSheet(const Texture& texture, const glm::ivec2& defaultSpiteSize);

    /** Destructor */
    ~SpriteSheet();

    /** Get Texture Id*/
    unsigned int getTextureId() const;

    /** Get Sprite sheet size in pixels */
    glm::ivec2 getSheetSize() const;

    /** Get the default sprite size for this Sprite Sheet */
    glm::ivec2 getDefaultSpriteSize() const;
private:
    /** Texture Id*/
    unsigned int mTextureId_;
    /** Sprite sheet size in pixels*/
    glm::ivec2 mSheetSize_;
    /** Default sprite size for this sheet. (Used to calculates the grid locations)*/
    glm::ivec2 mDefaultSpriteSize_;
};

} // namespace clay
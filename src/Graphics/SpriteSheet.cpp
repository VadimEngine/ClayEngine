// class
#include "Clay/Graphics/SpriteSheet.h"

namespace clay {

SpriteSheet::Sprite::Sprite(SpriteSheet& theSpriteSheet, const glm::ivec2& theGridIndex, const glm::ivec2& theSpriteSize)
    : parentSpriteSheet(theSpriteSheet), gridIndex(theGridIndex), spriteSize(theSpriteSize) {}

SpriteSheet::Sprite::Sprite(SpriteSheet& theSpriteSheet, const glm::ivec2& theGridIndex)
    : parentSpriteSheet(theSpriteSheet), gridIndex(theGridIndex), spriteSize(parentSpriteSheet.getDefaultSpriteSize()) {}

SpriteSheet::SpriteSheet(unsigned int textureId, const glm::ivec2& sheetSize, const glm::ivec2& defaultSpiteSize)
    : mTextureId_(textureId), mSheetSize_(sheetSize), mDefaultSpriteSize_(defaultSpiteSize) {}

SpriteSheet::SpriteSheet(const Texture& texture, const glm::ivec2& defaultSpiteSize)
    : mTextureId_(texture.getId()), mSheetSize_(texture.getShape()), mDefaultSpriteSize_(defaultSpiteSize) {}

SpriteSheet::~SpriteSheet() {}

unsigned int SpriteSheet::getTextureId() const {
    return mTextureId_;
}

glm::ivec2 SpriteSheet::getSheetSize() const {
    return mSheetSize_;
}

glm::ivec2 SpriteSheet::getDefaultSpriteSize() const {
    return mDefaultSpriteSize_;
}

} // namespace clay
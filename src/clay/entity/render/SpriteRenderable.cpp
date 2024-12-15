// class
#include "clay/entity/render/SpriteRenderable.h"

namespace clay {

SpriteRenderable::SpriteRenderable(SpriteSheet::Sprite* pSprite)
    : mpSprite_(pSprite) {}

SpriteRenderable::~SpriteRenderable() {}

void SpriteRenderable::render(const Renderer& theRenderer, const glm::mat4& parentModelMat) const {
    glm::mat4 model = glm::mat4(1.0f);

    // translation matrix for position
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), mPosition_);
    //rotation matrix
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1), glm::radians(mRotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(mRotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(mRotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));
    // scale matrix
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), mScale_);

    glm::mat4 localModelMat = translationMatrix * rotationMatrix * scaleMatrix;
    theRenderer.renderSprite(*mpSprite_, parentModelMat * localModelMat, mColor_);
}

void SpriteRenderable::setSprite(SpriteSheet::Sprite* pSprite) {
    mpSprite_ = pSprite;
}

SpriteSheet::Sprite* SpriteRenderable::getSprite() const {
    return mpSprite_;
}

} // namespace clay
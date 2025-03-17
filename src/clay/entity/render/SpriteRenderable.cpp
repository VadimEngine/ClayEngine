// class
#include "clay/entity/render/SpriteRenderable.h"

namespace clay {

SpriteRenderable::SpriteRenderable(SpriteSheet::Sprite* pSprite)
    : mpSprite_(pSprite) {}

SpriteRenderable::~SpriteRenderable() {}

void SpriteRenderable::render(IGraphicsContext& gContext, const glm::mat4& parentModelMat) const {
    glm::mat4 model = glm::mat4(1.0f);

    // translation matrix for position
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), mPosition_);
    //rotation matrix
    const glm::mat4 rotationMatrix = glm::mat4_cast(mOrientation_);
    // scale matrix
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), mScale_);

    glm::mat4 localModelMat = translationMatrix * rotationMatrix * scaleMatrix;
    gContext.renderer.renderSprite(*mpSprite_, parentModelMat * localModelMat, mColor_);
}

void SpriteRenderable::render(IGraphicsContext& gContext, const glm::mat4& parentModelMat, ShaderProgram& shader) const {
    glm::mat4 model = glm::mat4(1.0f);

    // translation matrix for position
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), mPosition_);
    //rotation matrix
    const glm::mat4 rotationMatrix = glm::mat4_cast(mOrientation_);
    // scale matrix
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), mScale_);

    glm::mat4 localModelMat = translationMatrix * rotationMatrix * scaleMatrix;
    gContext.renderer.renderSprite(*mpSprite_, shader, parentModelMat * localModelMat, mColor_);
}

void SpriteRenderable::setSprite(SpriteSheet::Sprite* pSprite) {
    mpSprite_ = pSprite;
}

SpriteSheet::Sprite* SpriteRenderable::getSprite() const {
    return mpSprite_;
}

} // namespace clay
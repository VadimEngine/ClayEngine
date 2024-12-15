// class
#include "clay/entity/render/TextRenderable.h"

namespace clay {

TextRenderable::TextRenderable(const std::string& text, Font* font)
: mText_(text), mFont_(font) {}

TextRenderable::~TextRenderable() {}

void TextRenderable::render(const Renderer& theRenderer, const glm::mat4& parentModelMat) const {
    // translation matrix for position
    glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), mPosition_);
    //rotation matrix
    glm::mat4 rotationMat = glm::rotate(glm::mat4(1), glm::radians(mRotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMat = glm::rotate(rotationMat, glm::radians(mRotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMat = glm::rotate(rotationMat, glm::radians(mRotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));
    // scale matrix
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), mScale_);

    glm::mat4 localModelMat = translationMat * rotationMat * scaleMat;

    const_cast<Renderer&>(theRenderer).renderTextNormalized(
        mText_,
        parentModelMat * localModelMat,
        *mFont_,
        mScale_,
        mColor_
    );
}

void TextRenderable::setText(const std::string& text) {
    mText_ = text;
}

void TextRenderable::setFont(Font* font) {
    mFont_ = font;
}

} // namespace clay
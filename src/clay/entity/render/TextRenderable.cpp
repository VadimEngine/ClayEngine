// class
#include "clay/entity/render/TextRenderable.h"

namespace clay {

TextRenderable::TextRenderable(const std::string& text, Font* font)
: mText_(text), mFont_(font) {}

TextRenderable::~TextRenderable() {}

void TextRenderable::render(IGraphicsContext& gContext, const glm::mat4& parentModelMat) const {
    // translation matrix for position
    glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), mPosition_);
    //rotation matrix
    const glm::mat4 rotationMatrix = glm::mat4_cast(mOrientation_);
    // scale matrix
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), mScale_);

    glm::mat4 localModelMat = translationMat * rotationMatrix * scaleMat;
    gContext.renderer.renderTextNormalized(
    mText_,
        parentModelMat * localModelMat,
        *mFont_,
        mScale_,
        mColor_
    );
}

void TextRenderable::render(IGraphicsContext& gContext, const glm::mat4& parentModelMat, ShaderProgram& shader) const {
    // translation matrix for position
    glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), mPosition_);
    //rotation matrix
    const glm::mat4 rotationMatrix = glm::mat4_cast(mOrientation_);
    // scale matrix
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), mScale_);

    glm::mat4 localModelMat = translationMat * rotationMatrix * scaleMat;
    // TODO currently stencil highlight is not clean for Text Rendering
    gContext.renderer.renderTextNormalized(
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
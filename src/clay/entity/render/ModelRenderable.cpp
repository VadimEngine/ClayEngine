// class
#include "clay/entity/render/ModelRenderable.h"

namespace clay {

ModelRenderable::ModelRenderable(const Model* pModel, const ShaderProgram* pShader)
    : mpModel_(pModel), mpShader_(pShader) {}

ModelRenderable::~ModelRenderable() {}

void ModelRenderable::render(const Renderer& theRenderer, const glm::mat4& parentModelMat) const {
    // translation matrix for position
    glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), mPosition_);
    //rotation matrix
    glm::mat4 rotationMat = glm::rotate(glm::mat4(1), glm::radians(mRotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMat = glm::rotate(rotationMat, glm::radians(mRotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMat = glm::rotate(rotationMat, glm::radians(mRotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));
    // scale matrix
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), mScale_);

    glm::mat4 localModelMat = translationMat * rotationMat * scaleMat;

    mpShader_->bind();

    // Bind all textures to the Texture Units
    for (const auto& [slot, texInfo] : mTextureByUnit_) {
        const auto& [texId, uniformName] = texInfo; // Unpack the pair
        mpShader_->setTexture(uniformName, texId, slot);
    }

    mpShader_->setMat4("uModel", parentModelMat * localModelMat);
    mpShader_->setVec4("uColor", mColor_);
    // TODO this only applies for some shaders
    mpShader_->setVec2("uSubImageTopLeft", mSubTextureTopLeft);
    mpShader_->setVec2("uSubImageSize", mSubTextureSize);

    if (renderWireframe_) {
        // Enable wire frame
        theRenderer.enableWireFrame(true);
        mpShader_->setBool("uWireframeMode",true);
        // Render wire frame
        mpModel_->render(*mpShader_);

        theRenderer.enableWireFrame(false);
        // revert back to non-wireframe
        mpShader_->setBool("uWireframeMode", false);
    }
    mpModel_->render(*mpShader_);
}

const Model* ModelRenderable::getModel() const {
    return mpModel_;
}

const ShaderProgram* ModelRenderable::getShader() const {
    return mpShader_;
}

void ModelRenderable::setModel(Model* pModel) {
    mpModel_ = pModel;
}

void ModelRenderable::setShader(ShaderProgram* pShader) {
    mpShader_ = pShader;
}

void ModelRenderable::setTexture(unsigned int textureUnit, unsigned int textureId, const std::string& uniformName) {
    mTextureByUnit_[textureUnit] = {textureId, uniformName};
}

void ModelRenderable::setWireframeRendering(const bool enable) {
    renderWireframe_ = enable;
}

void ModelRenderable::setSubTextureSize(const glm::vec2& size) {
    mSubTextureSize = size;
}

void ModelRenderable::setSubTextureTopLeft(const glm::vec2& pos) {
    mSubTextureTopLeft = pos;
}

} // namespace clay
// class
#include "clay/entity/render/ModelRenderable.h"

namespace clay {

ModelRenderable::ModelRenderable(const Model* pModel, const ShaderProgram* pShader)
    : mpModel_(pModel), mpShader_(pShader) {}

ModelRenderable::~ModelRenderable() {}

void ModelRenderable::render(IGraphicsContext& gContext, const glm::mat4& parentModelMat) const {
    // translation matrix for position
    glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), mPosition_);
    //rotation matrix
    const glm::mat4 rotationMatrix = glm::mat4_cast(mOrientation_);
    // scale matrix
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), mScale_);

    glm::mat4 localModelMat = translationMat * rotationMatrix * scaleMat;

    mpShader_->bind();
    mpShader_->bindUniformBuffer(0, gContext.renderer.getCurrentUBO());

    // Bind all textures to the Texture Units
    for (const auto& [slot, texInfo] : mTextureByUnit_) {
        const auto& [texId, uniformName] = texInfo;
        mpShader_->setTexture(uniformName, texId, slot);
    }

    mpShader_->setMat4("uModel", parentModelMat * localModelMat);
    mpShader_->setVec4("uColor", mColor_);
    // TODO this only applies for some shaders
    mpShader_->setVec2("uSubImageTopLeft", mSubTextureTopLeft);
    mpShader_->setVec2("uSubImageSize", mSubTextureSize);

    mpModel_->render(*mpShader_);
}

void ModelRenderable::render(IGraphicsContext& gContext, const glm::mat4& parentModelMat, ShaderProgram& inputShader) const {
    // translation matrix for position
    glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), mPosition_);
    //rotation matrix
    const glm::mat4 rotationMatrix = glm::mat4_cast(mOrientation_);
    // scale matrix
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), mScale_);

    glm::mat4 localModelMat = translationMat * rotationMatrix * scaleMat;

    inputShader.bind();
    inputShader.bindUniformBuffer(0, gContext.renderer.getCurrentUBO());

    // Bind all textures to the Texture Units
    for (const auto& [slot, texInfo] : mTextureByUnit_) {
        const auto& [texId, uniformName] = texInfo; // Unpack the pair
        inputShader.setTexture(uniformName, texId, slot);
    }

    inputShader.setMat4("uModel", parentModelMat * localModelMat);
    inputShader.setVec4("uColor", mColor_);
    // TODO this only applies for some shaders
    inputShader.setVec2("uSubImageTopLeft", mSubTextureTopLeft);
    inputShader.setVec2("uSubImageSize", mSubTextureSize);

    mpModel_->render(inputShader);
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

void ModelRenderable::setSubTextureSize(const glm::vec2& size) {
    mSubTextureSize = size;
}

void ModelRenderable::setSubTextureTopLeft(const glm::vec2& pos) {
    mSubTextureTopLeft = pos;
}

} // namespace clay
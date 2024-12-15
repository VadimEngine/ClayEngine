#include "clay/graphics/common/ShaderProgram.h"

namespace clay {

ShaderProgram::ShaderProgram(IGraphicsAPI& graphicsAPI)
: mGraphicsAPI_(graphicsAPI) {
    mProgramId_ = mGraphicsAPI_.createProgram();
}

ShaderProgram::~ShaderProgram() {
    mGraphicsAPI_.deleteProgram(mProgramId_);
}

void ShaderProgram::addShader(const ShaderCreateInfo& shaderInfo) {
    unsigned int shaderID = mGraphicsAPI_.createShader(shaderInfo.type);
    mGraphicsAPI_.compileShader(shaderID, shaderInfo.sourceData);
    // TODO delete/detach shaders?
}

void ShaderProgram::linkProgram() {
    mGraphicsAPI_.linkProgram(mProgramId_);
}

void ShaderProgram::bind() const {
    mGraphicsAPI_.useProgram(mProgramId_);
}

// utility uniform functions
void ShaderProgram::setBool(const std::string& name, bool value) const {
    mGraphicsAPI_.uniform1i(mGraphicsAPI_.getUniformLocation(mProgramId_, name), value);
}
void ShaderProgram::setInt(const std::string& name, int value) const {
    mGraphicsAPI_.uniform1i(mGraphicsAPI_.getUniformLocation(mProgramId_, name), value);
}
void ShaderProgram::setFloat(const std::string& name, float value) const {
    mGraphicsAPI_.uniform1i(mGraphicsAPI_.getUniformLocation(mProgramId_, name), value);
}
// ------------------------------------------------------------------------
void ShaderProgram::setVec2(const std::string& name, const glm::vec2& value) const {
    mGraphicsAPI_.uniform2fv(mGraphicsAPI_.getUniformLocation(mProgramId_, name), glm::value_ptr(value));
}
void ShaderProgram::setVec2(const std::string& name, float x, float y) const {
    mGraphicsAPI_.uniform2f(mGraphicsAPI_.getUniformLocation(mProgramId_, name), x, y);
}
void ShaderProgram::setVec3(const std::string& name, const glm::vec3& value) const {
    mGraphicsAPI_.uniform3fv(mGraphicsAPI_.getUniformLocation(mProgramId_, name), glm::value_ptr(value));
}
void ShaderProgram::setVec3(const std::string& name, float x, float y, float z) const {
    mGraphicsAPI_.uniform3f(mGraphicsAPI_.getUniformLocation(mProgramId_, name), x, y, z);
}
void ShaderProgram::setVec4(const std::string& name, const glm::vec4& value) const {
    mGraphicsAPI_.uniform4fv(mGraphicsAPI_.getUniformLocation(mProgramId_, name), glm::value_ptr(value));
}
void ShaderProgram::setVec4(const std::string& name, float x, float y, float z, float w) const {
    mGraphicsAPI_.uniform4f(mGraphicsAPI_.getUniformLocation(mProgramId_, name), x, y, z, w);
}
// ------------------------------------------------------------------------
void ShaderProgram::setMat2(const std::string& name, const glm::mat2& mat) const {
    mGraphicsAPI_.uniformMatrix2fv(mGraphicsAPI_.getUniformLocation(mProgramId_, name), glm::value_ptr(mat));
}
// ------------------------------------------------------------------------
void ShaderProgram::setMat3(const std::string& name, const glm::mat3& mat) const {
    mGraphicsAPI_.uniformMatrix2fv(mGraphicsAPI_.getUniformLocation(mProgramId_, name), glm::value_ptr(mat));
}
void ShaderProgram::setMat4(const std::string& name, const glm::mat4& mat) const {
    mGraphicsAPI_.uniformMatrix2fv(mGraphicsAPI_.getUniformLocation(mProgramId_, name), glm::value_ptr(mat));
}

unsigned int ShaderProgram::getProgramId() const {
    return mProgramId_;
}


} // namespace clay
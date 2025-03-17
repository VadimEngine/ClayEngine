// project
#include "clay/utils/common/Logger.h"
// class
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

    if (!checkCompileErrors(shaderID, shaderInfo.type)) {
        throw std::runtime_error("Shader compilation failed.");
    }

    mGraphicsAPI_.attachShader(mProgramId_, shaderID);
    mShaderIds_.push_back(shaderID);  // Store shader ID for later deletion
}

void ShaderProgram::linkProgram() {
    mGraphicsAPI_.linkProgram(mProgramId_);

    // delete shaders after linking
    for (unsigned int shaderID : mShaderIds_) {
        mGraphicsAPI_.deleteShader(shaderID);
    }
    mShaderIds_.clear();
}

void ShaderProgram::bind() const {
    mGraphicsAPI_.useProgram(mProgramId_);
}

bool ShaderProgram::checkCompileErrors(unsigned int shaderID, ShaderCreateInfo::Type type) {
    std::string log = mGraphicsAPI_.getShaderLog(shaderID);
    if (!log.empty()) {
        LOG_E("ERROR::SHADER_COMPILATION_ERROR of type: %d %s", (int)type, log.c_str());
        return false;
    }
    return true;
}

// utility uniform functions
void ShaderProgram::setBool(const std::string& name, bool value) const {
    mGraphicsAPI_.uniform1i(mGraphicsAPI_.getUniformLocation(mProgramId_, name), (int)value);
}
void ShaderProgram::setInt(const std::string& name, int value) const {
    mGraphicsAPI_.uniform1i(mGraphicsAPI_.getUniformLocation(mProgramId_, name), value);
}
void ShaderProgram::setFloat(const std::string& name, float value) const {
    mGraphicsAPI_.uniform1f(mGraphicsAPI_.getUniformLocation(mProgramId_, name), value);
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
    mGraphicsAPI_.uniformMatrix3fv(mGraphicsAPI_.getUniformLocation(mProgramId_, name), glm::value_ptr(mat));
}
void ShaderProgram::setMat4(const std::string& name, const glm::mat4& mat) const {
    mGraphicsAPI_.uniformMatrix4fv(mGraphicsAPI_.getUniformLocation(mProgramId_, name), glm::value_ptr(mat));
}

void ShaderProgram::setTexture(const std::string& uniformName, unsigned int textureId, unsigned int textureUnit) const {
    mGraphicsAPI_.activeTexture(textureUnit);
    mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, textureId);
    setInt(uniformName, textureUnit);
}

void ShaderProgram::bindUniformBuffer(unsigned int index, unsigned int buffer) const {
    mGraphicsAPI_.bindBufferBase(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, index, buffer);
}

void ShaderProgram::bindUniformBuffer(const std::string& uniformBlockName, unsigned int index, unsigned int buffer) const {
    unsigned int blockIndex = mGraphicsAPI_.getUniformBlockIndex(mProgramId_, uniformBlockName.c_str());

    mGraphicsAPI_.uniformBlockBinding(mProgramId_, blockIndex, index);
    mGraphicsAPI_.bindBufferBase(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, index, buffer);
}

unsigned int ShaderProgram::getProgramId() const {
    return mProgramId_;
}

} // namespace clay
#pragma once
// standard lib
#include <cstdint>
#include <string>
#include <stdexcept>
// third party
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
// project
#include "clay/graphics/common/IGraphicsAPI.h"

namespace clay {

class ShaderProgram {
public:
    ShaderProgram(IGraphicsAPI& graphicsAPI);

    ~ShaderProgram();

    void addShader(const ShaderCreateInfo& shaderInfo);

    void linkProgram();

    void bind() const;

    bool checkCompileErrors(unsigned int shaderID, ShaderCreateInfo::Type type);

    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

    void setTexture(const std::string& uniformName, unsigned int textureId, unsigned int textureUnit) const;

    /** Get the shader program Id*/
    unsigned int getProgramId() const;

private:
    /** Program Id for this Shader*/
    unsigned int mProgramId_;
    IGraphicsAPI& mGraphicsAPI_;
};

} // namespace clay
#pragma once
// standard lib
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
// third party
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
// project
#include "Clay/Application/Logger.h"

namespace clay {

class Shader {
public:
    /**
     * Constructor
     * @param vertexSourcePath Path the vertex Shader path
     * @param fragmentSourcePath Path to fragment Shader path
     */
    Shader(const std::filesystem::path& vertexSourcePath, const std::filesystem::path& fragmentSourcePath);

    /** Destructor */
    ~Shader();

    /** Bind the shader*/
    void bind() const;

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

    /** Get the shader program Id*/
    GLuint getProgramId() const;

private:
    /** Program Id for this Shader*/
    GLuint mProgramId_;
};

} // namespace clay
// class
#include "Clay/Graphics/Shader.h"

namespace clay {

Shader::Shader(const std::filesystem::path& vertexSourcePath, const std::filesystem::path& fragmentSourcePath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::badbit);

    try {
        vShaderFile.open(vertexSourcePath);
        fShaderFile.open(fragmentSourcePath);

        std::stringstream vShaderSteam, fShaderSteam;

        vShaderSteam << vShaderFile.rdbuf();
        fShaderSteam << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderSteam.str();
        fragmentCode = fShaderSteam.str();

    } catch (std::ifstream::failure e) {
        LOG_E("Error::SHADER::FILE_NOT_SUCCESFULLY_READ: %s ", e.what());
        LOG_E("Erro::SHADER : %s %s", vertexSourcePath.string().c_str(), fragmentSourcePath.string().c_str());
    }

    const GLchar* vShaderCode = vertexCode.c_str();
    const GLchar* fShaderCode = fragmentCode.c_str();

    GLuint vertex, fragment;
    GLint success;
    GLchar infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        LOG_E("SHADER::VERTEX::COMPILATION_FAILED\n %s", infoLog);
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        LOG_E("SHADER::FRAGMENT::COMPILATION_FAILED\n %s", infoLog);
    }

    mProgramId_ = glCreateProgram();
    glAttachShader(mProgramId_, vertex);
    glAttachShader(mProgramId_, fragment);
    glLinkProgram(mProgramId_);

    glGetProgramiv(mProgramId_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(mProgramId_, 512, NULL, infoLog);
        LOG_E("SHADER::PROGRAM::LINKING_FAILED\n %s", infoLog);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // Bind the Camera UBO
    const unsigned int uniformBlockIndex = glGetUniformBlockIndex(mProgramId_, "Camera");
    if (uniformBlockIndex != GL_INVALID_INDEX) {
        glUniformBlockBinding(mProgramId_, uniformBlockIndex, 0);
    }

    // Bind the LightBuffer UBO
    const unsigned int lightBlockIndex = glGetUniformBlockIndex(mProgramId_, "LightBuffer");
    if (lightBlockIndex != GL_INVALID_INDEX) {
        glUniformBlockBinding(mProgramId_, lightBlockIndex, 1);
    }
}

Shader::~Shader() {
    glDeleteProgram(mProgramId_);
}

void Shader::bind() const{
    glUseProgram(mProgramId_);
}

// utility uniform functions
void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(mProgramId_, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(mProgramId_, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(mProgramId_, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(mProgramId_, name.c_str()), 1, glm::value_ptr(value));
}
void Shader::setVec2(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(mProgramId_, name.c_str()), x, y);
}
void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(mProgramId_, name.c_str()), 1, glm::value_ptr(value));
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(mProgramId_, name.c_str()), x, y, z);
}
void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(mProgramId_, name.c_str()), 1, glm::value_ptr(value));
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(mProgramId_, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const {
    glUniformMatrix2fv(glGetUniformLocation(mProgramId_, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(glGetUniformLocation(mProgramId_, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(mProgramId_, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

GLuint Shader::getProgramId() const {
    return mProgramId_;
}

} // namespace clay
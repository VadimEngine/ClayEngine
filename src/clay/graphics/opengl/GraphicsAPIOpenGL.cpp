#include "clay/graphics/opengl/GraphicsAPIOpenGL.h"

namespace clay {
    GraphicsAPIOpenGL::GraphicsAPIOpenGL() {}
    
    GraphicsAPIOpenGL::~GraphicsAPIOpenGL() {}

    unsigned int GraphicsAPIOpenGL::createShader(ShaderCreateInfo::Type type) {
        GLenum glType;

        switch (type) {
        case ShaderCreateInfo::Type::VERTEX:
            glType = GL_VERTEX_SHADER;
            break;
        case ShaderCreateInfo::Type::TESSELLATION_CONTROL:
            glType = GL_TESS_CONTROL_SHADER;
            break;
        case ShaderCreateInfo::Type::TESSELLATION_EVALUATION:
            glType = GL_TESS_EVALUATION_SHADER;
            break;
        case ShaderCreateInfo::Type::GEOMETRY:
            glType = GL_GEOMETRY_SHADER;
            break;
        case ShaderCreateInfo::Type::FRAGMENT:
            glType = GL_FRAGMENT_SHADER;
            break;
        case ShaderCreateInfo::Type::COMPUTE:
            glType = GL_COMPUTE_SHADER;
            break;
        default:
            throw std::runtime_error("Invalid Shader Type");
        }

        return glCreateShader(glType);
    }
    
    void GraphicsAPIOpenGL::compileShader(unsigned int shaderID, const std::string& source) {
        const char* sourceCStr = source.c_str();
        glShaderSource(shaderID, 1, &sourceCStr, nullptr);
        glCompileShader(shaderID);
    }

    void GraphicsAPIOpenGL::attachShader(unsigned int programID, unsigned int shaderID) {
        glAttachShader(programID, shaderID);
    }

    unsigned int GraphicsAPIOpenGL::createProgram() {
        return glCreateProgram();
    }

    void GraphicsAPIOpenGL::linkProgram(unsigned int programID) {
        glLinkProgram(programID);
    }

    void GraphicsAPIOpenGL::useProgram(unsigned int programID) {
        glUseProgram(programID);
    }

    void GraphicsAPIOpenGL::deleteShader(unsigned int shaderID) {
        glDeleteShader(shaderID);
    }

    void GraphicsAPIOpenGL::deleteProgram(unsigned int programID) {
        glDeleteProgram(programID);
    }

    std::string GraphicsAPIOpenGL::getShaderLog(unsigned int shaderID) {
        char log[1024];
        glGetShaderInfoLog(shaderID, sizeof(log), nullptr, log);
        return std::string(log);
    }

    std::string GraphicsAPIOpenGL::getProgramLog(unsigned int programID) {
        char log[1024];
        glGetProgramInfoLog(programID, sizeof(log), nullptr, log);
        return std::string(log);
    }

    unsigned int GraphicsAPIOpenGL::getUniformLocation(unsigned int programId, const std::string& name) {
        return glGetUniformLocation(programId, name.c_str());
    }

    void GraphicsAPIOpenGL::uniform1i(unsigned int location, unsigned int value) {
        glUniform1i(location, value);
    }
    void GraphicsAPIOpenGL::uniform1f(unsigned int location, float value) {
        glUniform1f(location, value);
    }
    void GraphicsAPIOpenGL::uniform2fv(unsigned int location, const float* value) {
        glUniform2fv(location, 1, value);
    }

    void GraphicsAPIOpenGL::uniform2f(unsigned int location, float v0, float v1) {
        glUniform2f(location, v0, v1);
    }

    void GraphicsAPIOpenGL::uniform3fv(unsigned int location, const float* value) {
        glUniform3fv(location, 1, value);
    }

    void GraphicsAPIOpenGL::uniform3f(unsigned int location, float v0, float v1, float v2) {
        glUniform3f(location, v0, v1, v2);
    }

    void GraphicsAPIOpenGL::uniform4fv(unsigned int location, const float* value) {
        glUniform4fv(location, 1, value);
    }

    void GraphicsAPIOpenGL::uniform4f(unsigned int location, float v0, float v1, float v2, float v3) {
        glUniform4f(location, v0, v1, v2, v3);
    }

    void GraphicsAPIOpenGL::uniformMatrix2fv(unsigned int location, const float* value) {
        glUniformMatrix2fv(location, 1, GL_FALSE, value);
    }

    void GraphicsAPIOpenGL::uniformMatrix3fv(unsigned int location, const float* value) {
        glUniformMatrix3fv(location, 1, GL_FALSE, value);
    }

    void GraphicsAPIOpenGL::uniformMatrix4fv(unsigned int location, const float* value) {
        glUniformMatrix4fv(location, 1, GL_FALSE, value);
    }
    
} // namespace clay
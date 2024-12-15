#pragma once
// standard lib
#include <stdexcept>
// third party
#define GLEW_STATIC
#include <GL/glew.h>
// project
#include "clay/graphics/common/IGraphicsAPI.h"

namespace clay {

class GraphicsAPIOpenGL : public IGraphicsAPI {
public:
    GraphicsAPIOpenGL();
    
    ~GraphicsAPIOpenGL();

    unsigned int createShader(ShaderCreateInfo::Type type) override;
    void compileShader(unsigned int shaderID, const std::string& source) override;
    void attachShader(unsigned int programID, unsigned int shaderID) override;
    unsigned int createProgram() override;
    void linkProgram(unsigned int programID) override;
    void useProgram(unsigned int programID) override;
    void deleteShader(unsigned int shaderID) override;
    void deleteProgram(unsigned int programID) override;

    std::string getShaderLog(unsigned int shaderID) override;
    std::string getProgramLog(unsigned int programID) override;

    unsigned int getUniformLocation(unsigned int programId, const std::string& name) override;
    void uniform1i(unsigned int location, unsigned int value) override;
    void uniform1f(unsigned int location, float value) override;
    void uniform2fv(unsigned int location, const float* value) override;
    void uniform2f(unsigned int location, float v0, float v1) override;
    void uniform3fv(unsigned int location, const float* value) override;
    void uniform3f(unsigned int location, float v0, float v1, float v2) override;
    void uniform4fv(unsigned int location, const float* value) override;
    void uniform4f(unsigned int location, float v0, float v1, float v2, float v3) override;
    void uniformMatrix2fv(unsigned int location, const float* value) override;
    void uniformMatrix3fv(unsigned int location, const float* value) override;
    void uniformMatrix4fv(unsigned int location, const float* value) override;
};


} // namespace clay
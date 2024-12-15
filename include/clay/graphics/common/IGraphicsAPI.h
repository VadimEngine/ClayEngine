#pragma once
// standard lib
#include <string>
#include <cstdint>

namespace clay {

struct ShaderCreateInfo {
    enum class Type : uint8_t {
        VERTEX,
        TESSELLATION_CONTROL,
        TESSELLATION_EVALUATION,
        GEOMETRY,
        FRAGMENT,
        COMPUTE
    } type;
    const char* sourceData;
    size_t sourceSize;
};

class IGraphicsAPI {
public:
    virtual ~IGraphicsAPI() = default;

    virtual unsigned int createShader(ShaderCreateInfo::Type) = 0;
    virtual void compileShader(unsigned int shaderID, const std::string& source) = 0;
    virtual void attachShader(unsigned int programID, unsigned int shaderID) = 0;
    virtual unsigned int createProgram() = 0;
    virtual void linkProgram(unsigned int programID) = 0;
    virtual void useProgram(unsigned int programID) = 0;
    virtual void deleteShader(unsigned int shaderID) = 0;
    virtual void deleteProgram(unsigned int programID) = 0;

    virtual std::string getShaderLog(unsigned int shaderID) = 0;
    virtual std::string getProgramLog(unsigned int programID) = 0;

    virtual unsigned int getUniformLocation(unsigned int programId, const std::string& name) = 0;
    virtual void uniform1i(unsigned int location, unsigned int value) = 0;
    virtual void uniform1f(unsigned int location, float value) = 0;
    virtual void uniform2fv(unsigned int location, const float* value) = 0;
    virtual void uniform2f(unsigned int location, float v0, float v1) = 0;
    virtual void uniform3fv(unsigned int location, const float* value) = 0;
    virtual void uniform3f(unsigned int location, float v0, float v1, float v2) = 0;
    virtual void uniform4fv(unsigned int location, const float* value) = 0;
    virtual void uniform4f(unsigned int location, float v0, float v1, float v2, float v3) = 0;
    virtual void uniformMatrix2fv(unsigned int location, const float* value) = 0;
    virtual void uniformMatrix3fv(unsigned int location, const float* value) = 0;
    virtual void uniformMatrix4fv(unsigned int location, const float* value) = 0;
};

} // namespace clay

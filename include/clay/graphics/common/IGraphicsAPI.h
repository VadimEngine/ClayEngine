#pragma once
// standard lib
#include <string>
#include <cstdint>
#include <vector>

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

    enum class Capability {
        MULTISAMPLE,
        FRAMEBUFFER_SRGB
    };

    enum class FrameBufferTarget {
        DRAW_FRAMEBUFFER,
        READ_FRAMEBUFFER,
        FRAMEBUFFER 
    };

    enum class BufferTarget {
        ARRAY_BUFFER,
        ATOMIC_COUNTER_BUFFER,
        COPY_READ_BUFFER,
        COPY_WRITE_BUFFER,
        DISPATCH_INDIRECT_BUFFER,
        DRAW_INDIRECT_BUFFER,
        ELEMENT_ARRAY_BUFFER,
        PIXEL_PACK_BUFFER,
        PIXEL_UNPACK_BUFFER,
        QUERY_BUFFER,
        SHADER_STORAGE_BUFFER,
        TEXTURE_BUFFER,
        TRANSFORM_FEEDBACK_BUFFER,
        UNIFORM_BUFFER
    };

    enum class DataUsage {
        STREAM_DRAW,
        STREAM_READ,
        STREAM_COPY,
        STATIC_DRAW,
        STATIC_READ,
        STATIC_COPY,
        DYNAMIC_DRAW,
        DYNAMIC_READ,
        DYNAMIC_COPY
    };

    enum class DataType : uint8_t {
        FLOAT,
        VEC2,
        VEC3,
        VEC4,
        INT,
        IVEC2,
        IVEC3,
        IVEC4,
        UINT,
        UVEC2,
        UVEC3,
        UVEC4,
        UBYTE,
        BYTE,
    };

    enum class PrimitiveTopology : uint8_t {
        POINT_LIST,
        LINE_LIST,
        LINE_STRIP,
        LINE_LOOP,
        TRIANGLE_LIST,
        TRIANGLE_STRIP,
        TRIANGLE_FAN,
    };

    enum class TextureTarget : uint8_t {
        TEXTURE_1D,
        TEXTURE_1D_ARRAY,
        TEXTURE_2D, 
        TEXTURE_2D_ARRAY,
        TEXTURE_2D_MULTISAMPLE,
        TEXTURE_2D_MULTISAMPLE_ARRAY
    };

    enum class TextureParameterType : uint8_t {
        TEXTURE_WRAP_S,
        TEXTURE_WRAP_T,
        TEXTURE_MIN_FILTER,
        TEXTURE_MAG_FILTER
    };

    enum class TextureParameterOption : uint8_t {
        CLAMP_TO_BORDER,
        NEAREST,
        LINEAR,
        CLAMP_TO_EDGE,
        REPEAT,
    };

    enum class TextureFormat : uint8_t {
        SRGB,
        SRGB_ALPHA,
        RGB,
        RGBA,
        RED,
        LUMINANCE, // gles only
        RGBA16F,
    };

    enum class PixelAlignment {
        PACK_ALIGNMENT,
        UNPACK_ALIGNMENT
    };

    enum class FBOTextureTarget {
        TEXTURE_2D
    };

    enum class RenderBufferTarget {
        RENDERBUFFER
    };

    enum class RenderBufferFormat {
        DEPTH_COMPONENT
    };

    enum class FrameBufferAttachment {
        DEPTH_ATTACHMENT
    };

    enum class ClearBufferTarget {
        COLOR,
        DEPTH,
        STENCIL
    };

    enum class PolygonModeFace {
        FRONT_AND_BACK
    };

    enum class PolygonModeType {
        POINT,
        LINE,
        FILL
    };

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
    virtual void uniform1i(unsigned int location, int value) = 0;
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

    virtual void clearColor(float r, float g, float b, float a) = 0;

    virtual void bindFrameBuffer(FrameBufferTarget target, unsigned int bufferId) = 0;

    virtual void enable(Capability capability) = 0;
    virtual void disable(Capability capability) = 0;

    virtual void bindVertexArray(unsigned int vao) = 0;

    virtual void genBuffer(int size, unsigned int* vaos) = 0;

    virtual void bindBuffer(BufferTarget target, unsigned int bufferId) = 0;

    virtual void bufferData(BufferTarget target, size_t size, void* data, DataUsage usage) = 0;

    virtual void enableVertexAttribArray(unsigned int index) = 0;

    virtual void vertexAttribPointer(unsigned int index, int size, DataType type, bool normalized, size_t stride, const void* offset) = 0;

    virtual void drawElements(PrimitiveTopology mode, int count, DataType type, const void* indices) = 0;

    virtual void genVertexArrays(unsigned int n, unsigned int* arrays) = 0;

    virtual unsigned int getUniformBlockIndex(unsigned int programId, const char* uniformBlockName) = 0;

    virtual void uniformBlockBinding(unsigned int programId, unsigned int uniformBlockIndex, unsigned int uniformBlockBinding) = 0;


    virtual void deleteTexture(unsigned int n, unsigned int* textureId) = 0;

    virtual void genTextures(unsigned int count, unsigned int* textures) = 0;

    virtual void texParameter(TextureTarget target, TextureParameterType paramName, TextureParameterOption paramOption) = 0;

    virtual void texImage2D(TextureTarget target,
                            unsigned int level,
                            TextureFormat internalFormat,
                            unsigned int width,
                            unsigned int height,
                            unsigned int border,
                            TextureFormat format,
                            DataType dataType,
                            const void * data) = 0;

    virtual void bindTexture(TextureTarget target, unsigned int textureId) = 0;

    virtual void pixelStore(PixelAlignment alignment, unsigned int value) = 0;

    virtual void activeTexture(unsigned int textureUnit) = 0;

    virtual void getTexImage(TextureTarget target, unsigned int level, TextureFormat format, DataType type, void* pixels) = 0;

    virtual void bindBufferRange(BufferTarget target, unsigned int index, unsigned int buffer, size_t offset, size_t size) = 0;

    virtual void genFrameBuffers(unsigned int count, unsigned int* fbos) = 0;

    virtual void framebufferTexture2D(IGraphicsAPI::FrameBufferTarget target, unsigned int attachment, FBOTextureTarget texTarget, unsigned int textureId, unsigned int level) = 0;

    virtual void genRenderBuffer(unsigned int n, unsigned int* rbos) = 0;

    virtual void bindRenderBuffer(RenderBufferTarget target, unsigned int renderBuffer) = 0;

    virtual void renderBufferStorage(RenderBufferTarget target, RenderBufferFormat format, unsigned int width, unsigned int height) = 0;

    virtual void frameBufferRenderBuffer(FrameBufferTarget frameBufferTarget, FrameBufferAttachment attachment, RenderBufferTarget renderBufferTarget, unsigned int rbo) = 0;

    virtual void drawBuffers(unsigned int n, unsigned int* bufs) = 0;

    virtual void bufferSubData(BufferTarget target, size_t offset, size_t size, const void* data) = 0;

    virtual void drawArrays(PrimitiveTopology mode, unsigned int start, unsigned int count) = 0;

    virtual void clearBuffers(const std::vector<ClearBufferTarget>& targets) = 0;

    virtual void polygonMode(PolygonModeFace face, PolygonModeType mode) = 0;

    virtual void drawBuffer(unsigned int bufferId) = 0;

};

} // namespace clay

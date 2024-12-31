#pragma once
#ifdef CLAY_ENABLE_OPENGL
// standard lib
#include <stdexcept>
// third party
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
    void uniform1i(unsigned int location, int value) override;
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

    void clearColor(float r, float g, float b, float a) override;

    void bindFrameBuffer(IGraphicsAPI::FrameBufferTarget target, unsigned int bufferId) override;

    void enable(IGraphicsAPI::Capability capability) override;
    void disable(IGraphicsAPI::Capability capability) override;

    void genVertexArrays(unsigned int n, unsigned int* arrays) override;
    void bindVertexArray(unsigned int vao) override;

    void genBuffer(int size, unsigned int* vaos) override;

    void bindBuffer(BufferTarget target, unsigned int bufferId) override;

    void bufferData(BufferTarget target, size_t size, void* data, DataUsage usage) override;

    void enableVertexAttribArray(unsigned int index) override;

    void vertexAttribPointer(unsigned int index, int size, DataType type, bool normalized, size_t stride, const void* offset) override;

    void drawElements(PrimitiveTopology mode, int count, DataType type, const void* indices) override;

    unsigned int getUniformBlockIndex(unsigned int programId, const char* uniformBlockName) override;

    void uniformBlockBinding(unsigned int programId, unsigned int uniformBlockIndex, unsigned int uniformBlockBinding) override;


    void deleteTexture(unsigned int n, unsigned int* textureId) override;

    void genTextures(unsigned int count, unsigned int* textures) override;

    void texParameter(TextureTarget target, TextureParameterType paramName, TextureParameterOption paramOption) override;

    void texImage2D(TextureTarget target,
                    unsigned int level,
                    TextureFormat internalFormat,
                    unsigned int width,
                    unsigned int height,
                    unsigned int border,
                    TextureFormat format,
                    DataType dataType,
                    const void * data) override;

    void getTexImage(TextureTarget target, unsigned int level, TextureFormat format, DataType type, void* pixels) override;

    void bindTexture(TextureTarget target, unsigned int textureId) override;

    void pixelStore(PixelAlignment alignment, unsigned int value) override;

    void activeTexture(unsigned int textureUnit) override;

    void bindBufferRange(BufferTarget target, unsigned int index, unsigned int buffer, size_t offset, size_t size) override;

    void genFrameBuffers(unsigned int count, unsigned int* fbos) override;

    void framebufferTexture2D(IGraphicsAPI::FrameBufferTarget target, unsigned int attachment, FBOTextureTarget texTarget, unsigned int textureId, unsigned int level) override;

    void genRenderBuffer(unsigned int n, unsigned int* rbos) override;

    void bindRenderBuffer(RenderBufferTarget target, unsigned int renderBuffer) override;

    void renderBufferStorage(RenderBufferTarget target, RenderBufferFormat format, unsigned int width, unsigned int height) override;

    void frameBufferRenderBuffer(FrameBufferTarget frameBufferTarget, FrameBufferAttachment attachment, RenderBufferTarget renderBufferTarget, unsigned int rbo) override;

    void drawBuffers(unsigned int n, unsigned int* bufs) override;

    void bufferSubData(BufferTarget target, size_t offset, size_t size, const void* data) override;

    void drawArrays(PrimitiveTopology mode, unsigned int start, unsigned int count) override;

    void clearBuffers(const std::vector<ClearBufferTarget>& targets) override;

    void polygonMode(PolygonModeFace face, PolygonModeType mode) override;

    void drawBuffer(unsigned int bufferId) override;

};


} // namespace clay

#endif

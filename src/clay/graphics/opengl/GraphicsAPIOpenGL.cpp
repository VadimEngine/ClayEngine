#ifdef CLAY_ENABLE_OPENGL

// standard lib
#include <memory>
// class
#include "clay/graphics/opengl/GraphicsAPIOpenGL.h"
// project
#include "clay/utils/common/Logger.h"
// third party
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>


#define ENABLE_GL_ERROR_CHECK // Enable error checking

#ifdef ENABLE_GL_ERROR_CHECK
    #define GL_CALL(func)                                      \
        do {                                                   \
            func;                                              \
            GLenum err = glGetError();                         \
            if (err != GL_NO_ERROR) {                          \
                LOG_E("OpenGL error in " #func ": %d", err);   \
            }                                                  \
        } while (0)
#else
    #define GL_CALL(func) func
#endif

namespace clay {
    GraphicsAPIOpenGL::GraphicsAPIOpenGL() {
        glewExperimental = true;

        GLenum err = glewInit();
        if (GLEW_OK != err) {
            LOG_E("Glew Init failed");
            throw std::runtime_error("GLEW Init error");
        }
        glEnable(GL_CULL_FACE);// Default Rendered in counter clockwise
        glEnable(GL_DEPTH_TEST); // Enable z-buffer
        glEnable(GL_BLEND); // Needed for text rendering
        glDepthFunc(GL_LEQUAL); // Set Depth test to replace the current fragment if the z is less then OR equal
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Enable alpha drawing
    }
    
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

        unsigned int shaderId;
        GL_CALL(shaderId = glCreateShader(glType));

        return shaderId;
    }
    
    void GraphicsAPIOpenGL::compileShader(unsigned int shaderID, const std::string& source) {
        const char* sourceCStr = source.c_str();
        GL_CALL(glShaderSource(shaderID, 1, &sourceCStr, nullptr));
        GL_CALL(glCompileShader(shaderID));
    }

    void GraphicsAPIOpenGL::attachShader(unsigned int programID, unsigned int shaderID) {
        GL_CALL(glAttachShader(programID, shaderID));
    }

    unsigned int GraphicsAPIOpenGL::createProgram() {
        unsigned int programID;
        GL_CALL(programID = glCreateProgram());
        return programID;
    }

    void GraphicsAPIOpenGL::linkProgram(unsigned int programID) {
        GL_CALL(glLinkProgram(programID));
    }

    void GraphicsAPIOpenGL::useProgram(unsigned int programID) {
        GL_CALL(glUseProgram(programID));
    }

    void GraphicsAPIOpenGL::deleteShader(unsigned int shaderID) {
        GL_CALL(glDeleteShader(shaderID));
    }

    void GraphicsAPIOpenGL::deleteProgram(unsigned int programID) {
        // TODO fix this. probably need to delete/detach shaders
        GL_CALL(glDeleteProgram(programID));
    }

    std::string GraphicsAPIOpenGL::getShaderLog(unsigned int shaderID) {
        char log[1024];
        GL_CALL(glGetShaderInfoLog(shaderID, sizeof(log), nullptr, log));
        return std::string(log);
    }

    std::string GraphicsAPIOpenGL::getProgramLog(unsigned int programID) {
        char log[1024];
        GL_CALL(glGetProgramInfoLog(programID, sizeof(log), nullptr, log));
        return std::string(log);
    }

    unsigned int GraphicsAPIOpenGL::getUniformLocation(unsigned int programId, const std::string& name) {
        unsigned int uniformLocation;
        GL_CALL(uniformLocation = glGetUniformLocation(programId, name.c_str()));
        return uniformLocation;
    }

    void GraphicsAPIOpenGL::uniform1i(unsigned int location, int value) {
        GL_CALL(glUniform1i(location, value));
    }

    void GraphicsAPIOpenGL::uniform1f(unsigned int location, float value) {
        GL_CALL(glUniform1f(location, value));
    }

    void GraphicsAPIOpenGL::uniform2fv(unsigned int location, const float* value) {
        GL_CALL(glUniform2fv(location, 1, value));
    }

    void GraphicsAPIOpenGL::uniform2f(unsigned int location, float v0, float v1) {
        GL_CALL(glUniform2f(location, v0, v1));
    }

    void GraphicsAPIOpenGL::uniform3fv(unsigned int location, const float* value) {
        GL_CALL(glUniform3fv(location, 1, value));
    }

    void GraphicsAPIOpenGL::uniform3f(unsigned int location, float v0, float v1, float v2) {
        GL_CALL(glUniform3f(location, v0, v1, v2));
    }

    void GraphicsAPIOpenGL::uniform4fv(unsigned int location, const float* value) {
        GL_CALL(glUniform4fv(location, 1, value));
    }

    void GraphicsAPIOpenGL::uniform4f(unsigned int location, float v0, float v1, float v2, float v3) {
        GL_CALL(glUniform4f(location, v0, v1, v2, v3));
    }

    void GraphicsAPIOpenGL::uniformMatrix2fv(unsigned int location, const float* value) {
        GL_CALL(glUniformMatrix2fv(location, 1, GL_FALSE, value));
    }

    void GraphicsAPIOpenGL::uniformMatrix3fv(unsigned int location, const float* value) {
        GL_CALL(glUniformMatrix3fv(location, 1, GL_FALSE, value));
    }

    void GraphicsAPIOpenGL::uniformMatrix4fv(unsigned int location, const float* value) {
        GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, value));
    }

    void GraphicsAPIOpenGL::clearColor(float r, float g, float b, float a) {
        GL_CALL(glClearColor(r,g ,b, a));
    }

   void GraphicsAPIOpenGL::bindFrameBuffer(IGraphicsAPI::FrameBufferTarget target, unsigned int bufferId) {
        GLenum glTarget;

        switch (target) {
        case IGraphicsAPI::FrameBufferTarget::FRAMEBUFFER:
            glTarget = GL_FRAMEBUFFER;
            break;
        case IGraphicsAPI::FrameBufferTarget::DRAW_FRAMEBUFFER:
            glTarget = GL_DRAW_FRAMEBUFFER;
            break;
        case IGraphicsAPI::FrameBufferTarget::READ_FRAMEBUFFER:
            glTarget = GL_READ_FRAMEBUFFER ;
            break;
        default:
            throw std::runtime_error("Invalid Frame buffer target");
        }

        GL_CALL(glBindFramebuffer(glTarget, bufferId));
   }

   void GraphicsAPIOpenGL::enable(IGraphicsAPI::Capability capability) {
        GLenum glCapability;

        switch (capability) {
        case IGraphicsAPI::Capability::MULTISAMPLE:
            glCapability = GL_MULTISAMPLE;
            break;
        case IGraphicsAPI::Capability::FRAMEBUFFER_SRGB:
            glCapability = GL_FRAMEBUFFER_SRGB;
            break;
        default:
            throw std::runtime_error("Invalid capability");
        }

        GL_CALL(glEnable(glCapability));
   }

   void GraphicsAPIOpenGL::disable(IGraphicsAPI::Capability capability) {
        GLenum glCapability;

        switch (capability) {
        case IGraphicsAPI::Capability::MULTISAMPLE:
            glCapability = GL_MULTISAMPLE;
            break;
        case IGraphicsAPI::Capability::FRAMEBUFFER_SRGB:
            glCapability = GL_FRAMEBUFFER_SRGB;
            break;
        default:
            throw std::runtime_error("Invalid capability");
        }

        GL_CALL(glDisable(glCapability));
   }

    void GraphicsAPIOpenGL::genVertexArrays(unsigned int n, unsigned int* arrays) {
        GL_CALL(glGenVertexArrays(n, arrays));
    }

    void GraphicsAPIOpenGL::bindVertexArray(unsigned int vao) {
        GL_CALL(glBindVertexArray(vao));
    }

    void GraphicsAPIOpenGL::genBuffer(int size, unsigned int* vaos) {
        GL_CALL(glGenBuffers(size, vaos));
    }

    void GraphicsAPIOpenGL::bindBuffer(IGraphicsAPI::BufferTarget target, unsigned int bufferId) {
        GLenum glTarget;

        switch (target) {
        case IGraphicsAPI::BufferTarget::ARRAY_BUFFER:
            glTarget = GL_ARRAY_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::ATOMIC_COUNTER_BUFFER:
            glTarget = GL_ATOMIC_COUNTER_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::COPY_READ_BUFFER:
            glTarget = GL_COPY_READ_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::COPY_WRITE_BUFFER:
            glTarget = GL_COPY_WRITE_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::DISPATCH_INDIRECT_BUFFER:
            glTarget = GL_DISPATCH_INDIRECT_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::DRAW_INDIRECT_BUFFER:
            glTarget = GL_DRAW_INDIRECT_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::ELEMENT_ARRAY_BUFFER:
            glTarget = GL_ELEMENT_ARRAY_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::PIXEL_PACK_BUFFER:
            glTarget = GL_PIXEL_PACK_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::PIXEL_UNPACK_BUFFER:
            glTarget = GL_PIXEL_UNPACK_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::QUERY_BUFFER:
            glTarget = GL_QUERY_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::SHADER_STORAGE_BUFFER:
            glTarget = GL_SHADER_STORAGE_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::TEXTURE_BUFFER:
            glTarget = GL_TEXTURE_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::TRANSFORM_FEEDBACK_BUFFER:
            glTarget = GL_TRANSFORM_FEEDBACK_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::UNIFORM_BUFFER:
            glTarget = GL_UNIFORM_BUFFER;
            break;
        default:
            throw std::runtime_error("Invalid buffer target");
        }

        GL_CALL(glBindBuffer(glTarget, bufferId));
    }

    void GraphicsAPIOpenGL::bufferData(IGraphicsAPI::BufferTarget target, size_t size, void* data, IGraphicsAPI::DataUsage usage) {
        GLenum glTarget;

        switch (target) {
        case IGraphicsAPI::BufferTarget::ARRAY_BUFFER:
            glTarget = GL_ARRAY_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::ATOMIC_COUNTER_BUFFER:
            glTarget = GL_ATOMIC_COUNTER_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::COPY_READ_BUFFER:
            glTarget = GL_COPY_READ_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::COPY_WRITE_BUFFER:
            glTarget = GL_COPY_WRITE_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::DISPATCH_INDIRECT_BUFFER:
            glTarget = GL_DISPATCH_INDIRECT_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::DRAW_INDIRECT_BUFFER:
            glTarget = GL_DRAW_INDIRECT_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::ELEMENT_ARRAY_BUFFER:
            glTarget = GL_ELEMENT_ARRAY_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::PIXEL_PACK_BUFFER:
            glTarget = GL_PIXEL_PACK_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::PIXEL_UNPACK_BUFFER:
            glTarget = GL_PIXEL_UNPACK_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::QUERY_BUFFER:
            glTarget = GL_QUERY_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::SHADER_STORAGE_BUFFER:
            glTarget = GL_SHADER_STORAGE_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::TEXTURE_BUFFER:
            glTarget = GL_TEXTURE_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::TRANSFORM_FEEDBACK_BUFFER:
            glTarget = GL_TRANSFORM_FEEDBACK_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::UNIFORM_BUFFER:
            glTarget = GL_UNIFORM_BUFFER;
            break;
        default:
            throw std::runtime_error("Invalid buffer target");
        }

        GLenum glUsage;

        switch (usage) {
        case IGraphicsAPI::DataUsage::STREAM_DRAW:
            glUsage = GL_STREAM_DRAW;
            break;
        case IGraphicsAPI::DataUsage::STREAM_READ:
            glUsage = GL_STREAM_READ;
            break;
        case IGraphicsAPI::DataUsage::STREAM_COPY:
            glUsage = GL_STREAM_COPY;
            break;
        case IGraphicsAPI::DataUsage::STATIC_DRAW:
            glUsage = GL_STATIC_DRAW;
            break;
        case IGraphicsAPI::DataUsage::STATIC_READ:
            glUsage = GL_STATIC_READ;
            break;
        case IGraphicsAPI::DataUsage::STATIC_COPY:
            glUsage = GL_STATIC_COPY;
            break;
        case IGraphicsAPI::DataUsage::DYNAMIC_DRAW:
            glUsage = GL_DYNAMIC_DRAW;
            break;
        case IGraphicsAPI::DataUsage::DYNAMIC_READ:
            glUsage = GL_DYNAMIC_READ;
            break;
        case IGraphicsAPI::DataUsage::DYNAMIC_COPY:
            glUsage = GL_DYNAMIC_COPY;
            break;
        default:
            throw std::runtime_error("Invalid buffer usage");
        }

        GL_CALL(glBufferData(glTarget, size, data, glUsage));

    }

    void GraphicsAPIOpenGL::enableVertexAttribArray(unsigned int index) {
        GL_CALL(glEnableVertexAttribArray(index));
    }

    void GraphicsAPIOpenGL::vertexAttribPointer(unsigned int index, int size, IGraphicsAPI::DataType type, bool normalized, size_t stride, const void* offset) {
        GLenum glType;

        switch (type) {
        case IGraphicsAPI::DataType::FLOAT:
            glType = GL_FLOAT;
            break;
        case IGraphicsAPI::DataType::INT:
            glType = GL_INT;
            break;
        default:
            throw std::runtime_error("Invalid Vertex Type");
        }
        
        GL_CALL(glVertexAttribPointer(index, size, glType, normalized, stride, offset));
    }

    void GraphicsAPIOpenGL::drawElements(IGraphicsAPI::PrimitiveTopology mode, int count, IGraphicsAPI::DataType type, const void* indices) {
        GLenum glMode;

        switch (mode) {
        case IGraphicsAPI::PrimitiveTopology::POINT_LIST:
            glMode = GL_POINTS;
            break;
        case IGraphicsAPI::PrimitiveTopology::LINE_LIST:
            glMode = GL_LINES;
            break;
        case IGraphicsAPI::PrimitiveTopology::LINE_STRIP:
            glMode = GL_LINE_STRIP;
            break;
        case IGraphicsAPI::PrimitiveTopology::LINE_LOOP:
            glMode = GL_LINE_LOOP;
            break;
        case IGraphicsAPI::PrimitiveTopology::TRIANGLE_LIST:
            glMode = GL_TRIANGLES;
            break;
        case IGraphicsAPI::PrimitiveTopology::TRIANGLE_STRIP:
            glMode = GL_TRIANGLE_STRIP;
            break;
        case IGraphicsAPI::PrimitiveTopology::TRIANGLE_FAN:
            glMode = GL_TRIANGLE_FAN;
            break;
        default:
            throw std::runtime_error("Invalid PrimitiveTopology Type");
        }
        
        GLenum glType;

        switch (type) {
        case IGraphicsAPI::DataType::FLOAT:
            glType = GL_FLOAT;
            break;
        case IGraphicsAPI::DataType::INT:
            glType = GL_INT;
            break;
        case IGraphicsAPI::DataType::UINT:
            glType = GL_UNSIGNED_INT;
            break;
        default:
            throw std::runtime_error("Invalid Index Type");
        }

        GL_CALL(glDrawElements(glMode, count, glType, indices));
    }

    unsigned int GraphicsAPIOpenGL::getUniformBlockIndex(unsigned int programId, const char* uniformBlockName) {
        unsigned int index;
        GL_CALL(index = glGetUniformBlockIndex(programId, uniformBlockName));
        return index;
    };

    void GraphicsAPIOpenGL::uniformBlockBinding(unsigned int programId, unsigned int uniformBlockIndex, unsigned int uniformBlockBinding) {
        if (uniformBlockIndex != GL_INVALID_INDEX) {
            GL_CALL(GL_CALL(glUniformBlockBinding(programId, uniformBlockIndex, uniformBlockBinding)));
        } else {
            LOG_E("Invalid uniformBlockIndex %d", uniformBlockIndex);
        }
    }

    void GraphicsAPIOpenGL::deleteTexture(unsigned int n, unsigned int* textureId) {
        // TODO FIX THIS Error: OpenGL error in glDeleteTextures(n, textureId): 1282
        GL_CALL(glDeleteTextures(n, textureId));
    }

    void GraphicsAPIOpenGL::genTextures(unsigned int count, unsigned int* textures) {
        GL_CALL(glGenTextures(count, textures));
    }

    void GraphicsAPIOpenGL::texParameter(IGraphicsAPI::TextureTarget target, IGraphicsAPI::TextureParameterType paramName, IGraphicsAPI::TextureParameterOption paramValue) {
        GLenum glTarget;

        switch (target) {
            case IGraphicsAPI::TextureTarget::TEXTURE_1D: 
                glTarget = GL_TEXTURE_1D;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_1D_ARRAY: 
                glTarget = GL_TEXTURE_1D_ARRAY;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_2D: 
                glTarget = GL_TEXTURE_2D;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_2D_ARRAY: 
                glTarget = GL_TEXTURE_2D_ARRAY;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_2D_MULTISAMPLE: 
                glTarget = GL_TEXTURE_2D_MULTISAMPLE;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_2D_MULTISAMPLE_ARRAY: 
                glTarget = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
                break;
            default:
                throw std::runtime_error("Invalid Texture target");
        }

        GLenum glParamName;

        switch (paramName) {
            case IGraphicsAPI::TextureParameterType::TEXTURE_WRAP_S: 
                glParamName = GL_TEXTURE_WRAP_S;
                break;
            case IGraphicsAPI::TextureParameterType::TEXTURE_WRAP_T: 
                glParamName = GL_TEXTURE_WRAP_T;
                break;
            case IGraphicsAPI::TextureParameterType::TEXTURE_MIN_FILTER: 
                glParamName = GL_TEXTURE_MIN_FILTER;
                break;
            case IGraphicsAPI::TextureParameterType::TEXTURE_MAG_FILTER: 
                glParamName = GL_TEXTURE_MAG_FILTER;
                break;
            default:
                throw std::runtime_error("Invalid Texture Parameter");
        }

        GLenum glParamValue;

        switch (paramValue) {
            case IGraphicsAPI::TextureParameterOption::CLAMP_TO_BORDER: 
                glParamValue = GL_CLAMP_TO_BORDER;
                break;
            case IGraphicsAPI::TextureParameterOption::NEAREST: 
                glParamValue = GL_NEAREST;
                break;
            case IGraphicsAPI::TextureParameterOption::LINEAR: 
                glParamValue = GL_LINEAR;
                break;
            case IGraphicsAPI::TextureParameterOption::CLAMP_TO_EDGE: 
                glParamValue = GL_CLAMP_TO_EDGE;
                break;
            case IGraphicsAPI::TextureParameterOption::REPEAT: 
                glParamValue = GL_REPEAT;
                break;
            default:
                throw std::runtime_error("Invalid Texture Parameter Value");
        }

        GL_CALL(glTexParameteri(glTarget, glParamName, glParamValue));
    }

    void GraphicsAPIOpenGL::texImage2D(IGraphicsAPI::TextureTarget target,
                            unsigned int level,
                            TextureFormat internalFormat,
                            unsigned int width,
                            unsigned int height,
                            unsigned int border,
                            TextureFormat format,
                            DataType dataType,
                            const void * data) {

        GLenum glTarget;

        switch (target) {
            case IGraphicsAPI::TextureTarget::TEXTURE_1D: 
                glTarget = GL_TEXTURE_1D;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_1D_ARRAY: 
                glTarget = GL_TEXTURE_1D_ARRAY;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_2D: 
                glTarget = GL_TEXTURE_2D;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_2D_ARRAY: 
                glTarget = GL_TEXTURE_2D_ARRAY;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_2D_MULTISAMPLE: 
                glTarget = GL_TEXTURE_2D_MULTISAMPLE;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_2D_MULTISAMPLE_ARRAY: 
                glTarget = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
                break;
            default:
                throw std::runtime_error("Invalid Texture target");
        }

        GLenum glInternalFormat;

        switch (internalFormat) {
            case IGraphicsAPI::TextureFormat::SRGB: 
                glInternalFormat = GL_SRGB;
                break;
            case IGraphicsAPI::TextureFormat::SRGB_ALPHA: 
                glInternalFormat = GL_SRGB_ALPHA;
                break;
            case IGraphicsAPI::TextureFormat::RGB: 
                glInternalFormat = GL_RGB;
                break;
            case IGraphicsAPI::TextureFormat::RGBA: 
                glInternalFormat = GL_RGBA;
                break;
            case IGraphicsAPI::TextureFormat::RED: 
                glInternalFormat = GL_RED;
                break;
            case IGraphicsAPI::TextureFormat::RGBA16F: 
                glInternalFormat = GL_RGBA16F;
                break;
            default:
                throw std::runtime_error("Invalid Internal texture format");
        }

        GLenum glFormat;

        switch (format) {
            case IGraphicsAPI::TextureFormat::SRGB: 
                glFormat = GL_SRGB;
                break;
            case IGraphicsAPI::TextureFormat::SRGB_ALPHA: 
                glFormat = GL_SRGB_ALPHA;
                break;
            case IGraphicsAPI::TextureFormat::RGB: 
                glFormat = GL_RGB;
                break;
            case IGraphicsAPI::TextureFormat::RGBA: 
                glFormat = GL_RGBA;
                break;
            case IGraphicsAPI::TextureFormat::RED: 
                glFormat = GL_RED;
                break;
            case IGraphicsAPI::TextureFormat::RGBA16F: 
                glFormat = GL_RGBA16F;
                break;
            default:
                throw std::runtime_error("Invalid Texture input format");
        }

        GLenum glDataType;

        switch (dataType) {
            case IGraphicsAPI::DataType::BYTE: 
                glDataType = GL_BYTE;
                break;
            case IGraphicsAPI::DataType::UBYTE: 
                glDataType = GL_UNSIGNED_BYTE;
                break;
            case IGraphicsAPI::DataType::FLOAT: 
                glDataType = GL_FLOAT;
                break;
            default:
                throw std::runtime_error("Invalid Texture Data type");
        }

        GL_CALL(glTexImage2D(glTarget, level, glInternalFormat, width, height, border, glFormat, glDataType, data));
    }

    void GraphicsAPIOpenGL::bindTexture(TextureTarget target, unsigned int textureId) {
        GLenum glTarget;

        switch (target) {
            case IGraphicsAPI::TextureTarget::TEXTURE_1D: 
                glTarget = GL_TEXTURE_1D;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_1D_ARRAY: 
                glTarget = GL_TEXTURE_1D_ARRAY;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_2D: 
                glTarget = GL_TEXTURE_2D;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_2D_ARRAY: 
                glTarget = GL_TEXTURE_2D_ARRAY;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_2D_MULTISAMPLE: 
                glTarget = GL_TEXTURE_2D_MULTISAMPLE;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_2D_MULTISAMPLE_ARRAY: 
                glTarget = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
                break;
            default:
                throw std::runtime_error("Invalid Texture target");
        }

        GL_CALL(glBindTexture(glTarget, textureId));
    }

    void GraphicsAPIOpenGL::getTexImage(TextureTarget target, unsigned int level, TextureFormat format, DataType dataType, void* pixels) {
        GLenum glTarget;

        switch (target) {
            case IGraphicsAPI::TextureTarget::TEXTURE_1D: 
                glTarget = GL_TEXTURE_1D;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_1D_ARRAY: 
                glTarget = GL_TEXTURE_1D_ARRAY;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_2D: 
                glTarget = GL_TEXTURE_2D;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_2D_ARRAY: 
                glTarget = GL_TEXTURE_2D_ARRAY;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_2D_MULTISAMPLE: 
                glTarget = GL_TEXTURE_2D_MULTISAMPLE;
                break;
            case IGraphicsAPI::TextureTarget::TEXTURE_2D_MULTISAMPLE_ARRAY: 
                glTarget = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
                break;
            default:
                throw std::runtime_error("Invalid Texture target");
        }

        GLenum glFormat;

        switch (format) {
            case IGraphicsAPI::TextureFormat::SRGB: 
                glFormat = GL_SRGB;
                break;
            case IGraphicsAPI::TextureFormat::SRGB_ALPHA: 
                glFormat = GL_SRGB_ALPHA;
                break;
            case IGraphicsAPI::TextureFormat::RGB: 
                glFormat = GL_RGB;
                break;
            case IGraphicsAPI::TextureFormat::RGBA: 
                glFormat = GL_RGBA;
                break;
            case IGraphicsAPI::TextureFormat::RED: 
                glFormat = GL_RED;
                break;
            case IGraphicsAPI::TextureFormat::RGBA16F: 
                glFormat = GL_RGBA16F;
                break;
            default:
                throw std::runtime_error("Invalid Texture input format");
        }

        GLenum glDataType;

        switch (dataType) {
            case IGraphicsAPI::DataType::BYTE: 
                glDataType = GL_BYTE;
                break;
            case IGraphicsAPI::DataType::UBYTE: 
                glDataType = GL_UNSIGNED_BYTE;
                break;
            case IGraphicsAPI::DataType::FLOAT: 
                glDataType = GL_FLOAT;
                break;
            default:
                throw std::runtime_error("Invalid Texture Data type");
        }

        GL_CALL(glGetTexImage(glTarget, 0, glFormat, glDataType, pixels));
    }

    void GraphicsAPIOpenGL::pixelStore(PixelAlignment alignment, unsigned int value) {
        GLenum GLAlignment;

        switch (alignment) {
        case PixelAlignment::PACK_ALIGNMENT:
            GLAlignment = GL_PACK_ALIGNMENT;
            break;
        case PixelAlignment::UNPACK_ALIGNMENT:
            GLAlignment = GL_UNPACK_ALIGNMENT;
            break;
        default:
            throw std::runtime_error("Pixel Alignment type");
        }
        GL_CALL(glPixelStorei(GLAlignment, value));
    }

    void GraphicsAPIOpenGL::activeTexture(unsigned int textureUnit) {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + textureUnit));
    }

    void GraphicsAPIOpenGL::bindBufferRange(BufferTarget target, unsigned int index, unsigned int buffer, size_t offset, size_t size) {
        GLenum glTarget;

        switch (target) {
        case IGraphicsAPI::BufferTarget::ARRAY_BUFFER:
            glTarget = GL_ARRAY_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::ATOMIC_COUNTER_BUFFER:
            glTarget = GL_ATOMIC_COUNTER_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::COPY_READ_BUFFER:
            glTarget = GL_COPY_READ_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::COPY_WRITE_BUFFER:
            glTarget = GL_COPY_WRITE_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::DISPATCH_INDIRECT_BUFFER:
            glTarget = GL_DISPATCH_INDIRECT_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::DRAW_INDIRECT_BUFFER:
            glTarget = GL_DRAW_INDIRECT_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::ELEMENT_ARRAY_BUFFER:
            glTarget = GL_ELEMENT_ARRAY_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::PIXEL_PACK_BUFFER:
            glTarget = GL_PIXEL_PACK_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::PIXEL_UNPACK_BUFFER:
            glTarget = GL_PIXEL_UNPACK_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::QUERY_BUFFER:
            glTarget = GL_QUERY_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::SHADER_STORAGE_BUFFER:
            glTarget = GL_SHADER_STORAGE_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::TEXTURE_BUFFER:
            glTarget = GL_TEXTURE_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::TRANSFORM_FEEDBACK_BUFFER:
            glTarget = GL_TRANSFORM_FEEDBACK_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::UNIFORM_BUFFER:
            glTarget = GL_UNIFORM_BUFFER;
            break;
        default:
            throw std::runtime_error("Invalid buffer target");
        }

        GL_CALL(glBindBufferRange(glTarget, index, buffer, offset, size));
    }

    void GraphicsAPIOpenGL::genFrameBuffers(unsigned int count, unsigned int* fbos) {
        GL_CALL(glGenFramebuffers(count, fbos));
    }

    void GraphicsAPIOpenGL::framebufferTexture2D(IGraphicsAPI::FrameBufferTarget target, unsigned int attachment, FBOTextureTarget texTarget, unsigned int textureId, unsigned int level) {
        // TODO confirm if GL_FRAMEBUFFER/GL_TEXTURE_2D can ever be different options in here
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, GL_TEXTURE_2D, textureId, level));
    }

    void GraphicsAPIOpenGL::genRenderBuffer(unsigned int n, unsigned int* rbos) {
        GL_CALL(glGenRenderbuffers(n, rbos));
    }

    void GraphicsAPIOpenGL::bindRenderBuffer(RenderBufferTarget target, unsigned int renderBuffer) {
        GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer));
    }

    void GraphicsAPIOpenGL::renderBufferStorage(RenderBufferTarget target, RenderBufferFormat format, unsigned int width, unsigned int height) {
        GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height));
    }

    void GraphicsAPIOpenGL::frameBufferRenderBuffer(FrameBufferTarget frameBufferTarget, FrameBufferAttachment attachment, RenderBufferTarget renderBufferTarget, unsigned int rbo) {
        GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo));
    }

    void GraphicsAPIOpenGL::drawBuffers(unsigned int n, unsigned int* bufs) {
        // todo handle other buffers
        std::unique_ptr<unsigned int[]> temp = std::make_unique<unsigned int[]>(n);
        for (int i = 0; i < n; ++i) {
            temp[i] = GL_COLOR_ATTACHMENT0 + bufs[i];
        }
        GL_CALL(glDrawBuffers(n, temp.get()));
    }

    void GraphicsAPIOpenGL::bufferSubData(BufferTarget target, size_t offset, size_t size, const void* data) {
        GLenum glTarget;

        switch (target) {
        case IGraphicsAPI::BufferTarget::ARRAY_BUFFER:
            glTarget = GL_ARRAY_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::ATOMIC_COUNTER_BUFFER:
            glTarget = GL_ATOMIC_COUNTER_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::COPY_READ_BUFFER:
            glTarget = GL_COPY_READ_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::COPY_WRITE_BUFFER:
            glTarget = GL_COPY_WRITE_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::DISPATCH_INDIRECT_BUFFER:
            glTarget = GL_DISPATCH_INDIRECT_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::DRAW_INDIRECT_BUFFER:
            glTarget = GL_DRAW_INDIRECT_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::ELEMENT_ARRAY_BUFFER:
            glTarget = GL_ELEMENT_ARRAY_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::PIXEL_PACK_BUFFER:
            glTarget = GL_PIXEL_PACK_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::PIXEL_UNPACK_BUFFER:
            glTarget = GL_PIXEL_UNPACK_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::QUERY_BUFFER:
            glTarget = GL_QUERY_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::SHADER_STORAGE_BUFFER:
            glTarget = GL_SHADER_STORAGE_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::TEXTURE_BUFFER:
            glTarget = GL_TEXTURE_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::TRANSFORM_FEEDBACK_BUFFER:
            glTarget = GL_TRANSFORM_FEEDBACK_BUFFER;
            break;
        case IGraphicsAPI::BufferTarget::UNIFORM_BUFFER:
            glTarget = GL_UNIFORM_BUFFER;
            break;
        default:
            throw std::runtime_error("Invalid buffer target");
        }

        GL_CALL(glBufferSubData(glTarget, offset, size, data));
    }

    void GraphicsAPIOpenGL::drawArrays(PrimitiveTopology mode, unsigned int start, unsigned int count) {
        GLenum glMode;

        switch (mode) {
        case IGraphicsAPI::PrimitiveTopology::POINT_LIST:
            glMode = GL_POINTS;
            break;
        case IGraphicsAPI::PrimitiveTopology::LINE_LIST:
            glMode = GL_LINES;
            break;
        case IGraphicsAPI::PrimitiveTopology::LINE_STRIP:
            glMode = GL_LINE_STRIP;
            break;
        case IGraphicsAPI::PrimitiveTopology::LINE_LOOP:
            glMode = GL_LINE_LOOP;
            break;
        case IGraphicsAPI::PrimitiveTopology::TRIANGLE_LIST:
            glMode = GL_TRIANGLES;
            break;
        case IGraphicsAPI::PrimitiveTopology::TRIANGLE_STRIP:
            glMode = GL_TRIANGLE_STRIP;
            break;
        case IGraphicsAPI::PrimitiveTopology::TRIANGLE_FAN:
            glMode = GL_TRIANGLE_FAN;
            break;
        default:
            throw std::runtime_error("Invalid PrimitiveTopology Type");
        }
        
        GL_CALL(glDrawArrays(glMode, start, count));
    }

    void GraphicsAPIOpenGL::clearBuffers(const std::vector<ClearBufferTarget>& targets) {
        GLuint clearMask = 0;

        for (const auto& target : targets) {
            switch (target) {
                case ClearBufferTarget::COLOR:
                    clearMask |= GL_COLOR_BUFFER_BIT;
                    break;
                case ClearBufferTarget::DEPTH:
                    clearMask |= GL_DEPTH_BUFFER_BIT;
                    break;
                case ClearBufferTarget::STENCIL:
                    clearMask |= GL_STENCIL_BUFFER_BIT;
                    break;
            }
        }

        GL_CALL(glClear(clearMask));
    }

    void GraphicsAPIOpenGL::polygonMode(PolygonModeFace face, PolygonModeType mode) {
        GLenum glMode = 0;

        switch (mode) {
            case PolygonModeType::POINT:
                glMode = GL_POINT;
                break;
            case PolygonModeType::LINE:
                glMode = GL_LINE;
                break;
            case PolygonModeType::FILL:
                glMode = GL_FILL;
                break;
        }

        glPolygonMode(GL_FRONT_AND_BACK, glMode);
    }

    void GraphicsAPIOpenGL::drawBuffer(unsigned int bufferId) {
        // TODO add support for other values
        GL_CALL(glDrawBuffer(GL_COLOR_ATTACHMENT0 + bufferId));
    }

} // namespace clay

#endif
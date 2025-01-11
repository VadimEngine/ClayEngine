#ifdef CLAY_ENABLE_OPENGL_ES

//project
#include "clay/utils/common/Logger.h"
// class
#include "clay/graphics/opengles/GraphicsAPIOpenGLES.h"


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

GraphicsAPIOpenGLES::GraphicsAPIOpenGLES(XrInstance m_xrInstance, XrSystemId systemId) {
    OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetOpenGLESGraphicsRequirementsKHR", (PFN_xrVoidFunction*)&xrGetOpenGLESGraphicsRequirementsKHR), "Failed to get InstanceProcAddr for xrGetOpenGLESGraphicsRequirementsKHR.")
    XrGraphicsRequirementsOpenGLESKHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR};
    // CRASH HERE
    OPENXR_CHECK(xrGetOpenGLESGraphicsRequirementsKHR(m_xrInstance, systemId, &graphicsRequirements), "Failed to get Graphics Requirements for OpenGLES.")

    // Create an EGL display
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglDisplay == EGL_NO_DISPLAY) {
        LOG_E("ERROR: Failed to get default EGL display.");
        return;
    }

    if (!eglInitialize(eglDisplay, nullptr, nullptr)) {
        LOG_E("ERROR: Failed to initialize EGL.");
        return;
    }

    // Choose a suitable EGL configuration
    const EGLint configAttributes[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_STENCIL_SIZE, 8,
            EGL_NONE
    };
    EGLint numConfigs;
    if (!eglChooseConfig(eglDisplay, configAttributes, &eglConfig, 1, &numConfigs) || numConfigs == 0) {
        LOG_E("ERROR: Failed to choose a valid EGLConfig.");
        return;
    }

    // Create a Pbuffer surface for offscreen rendering
    const EGLint pbufferAttributes[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};
    EGLSurface eglSurface = eglCreatePbufferSurface(eglDisplay, eglConfig, pbufferAttributes);
    if (eglSurface == EGL_NO_SURFACE) {
        LOG_E("ERROR: Failed to create EGL Pbuffer surface.");
        return;
    }

    // Create an EGL context
    eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT,
                                  (EGLint[]){EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE});
    if (eglContext == EGL_NO_CONTEXT) {
        LOG_E("ERROR: Failed to create EGL context.");
        return;
    }

    if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
        LOG_E("ERROR: Failed to make EGL context current.");
        return;
    }
}


std::vector<int64_t> GraphicsAPIOpenGLES::getSupportedDepthSwapchainFormats() {
    return {
            GL_DEPTH_COMPONENT32F,
            GL_DEPTH_COMPONENT24,
            GL_DEPTH_COMPONENT16
    };
}

void* GraphicsAPIOpenGLES::getSwapchainImage(XrSwapchain swapchain, uint32_t index) {
    return (void*)(uint64_t)swapchainImagesMap[swapchain].second[index].image;
}

int64_t GraphicsAPIOpenGLES::selectColorSwapchainFormat(const std::vector<int64_t> &formats) {
    const std::vector<int64_t> &supportSwapchainFormats = getSupportedColorSwapchainFormats();

    const std::vector<int64_t>::const_iterator &swapchainFormatIt = std::find_first_of(formats.begin(), formats.end(),
                                                                                       std::begin(supportSwapchainFormats), std::end(supportSwapchainFormats));
    if (swapchainFormatIt == formats.end()) {
        LOG_E("Unable to find supported Color Swapchain Format");
        return 0;
    }

    return *swapchainFormatIt;
}

void GraphicsAPIOpenGLES::setPipeline(void* pipeline) {
    GLuint program = (GLuint)(uint64_t)pipeline;
    glUseProgram(program);
    m_setPipeline = program;

    const PipelineCreateInfo &pipelineCI = pipelines[program];

    // InputAssemblyState
    const InputAssemblyState &IAS = pipelineCI.inputAssemblyState;
    if (IAS.primitiveRestartEnable) {
        glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    } else {
        glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    }

    // RasterisationState
    const RasterisationState &RS = pipelineCI.rasterisationState;

    if (RS.rasteriserDiscardEnable) {
        glEnable(GL_RASTERIZER_DISCARD);
    } else {
        glDisable(GL_RASTERIZER_DISCARD);
    }

    if (RS.cullMode > CullMode::NONE) {
        glEnable(GL_CULL_FACE);
        glCullFace(ToGLCullMode(RS.cullMode));
    } else {
        glDisable(GL_CULL_FACE);
    }

    glFrontFace(RS.frontFace == FrontFace::COUNTER_CLOCKWISE ? GL_CCW : GL_CW);

    GLenum polygonOffsetMode = 0;
    switch (RS.polygonMode) {
        default:
        case PolygonMode::FILL: {
            polygonOffsetMode = GL_POLYGON_OFFSET_FILL;
            break;
        }
        case PolygonMode::LINE: {
            polygonOffsetMode = 0; //GL_POLYGON_OFFSET_LINE; //None for ES
            break;
        }
        case PolygonMode::POINT: {
            polygonOffsetMode = 0; //GL_POLYGON_OFFSET_POINT; //None for ES
            break;
        }
    }
    if (RS.depthBiasEnable) {
        glEnable(polygonOffsetMode);
        // glPolygonOffsetClamp
        glPolygonOffset(RS.depthBiasSlopeFactor, RS.depthBiasConstantFactor);
    } else {
        glDisable(polygonOffsetMode);
    }

    glLineWidth(RS.lineWidth);

    // MultisampleState
    const MultisampleState &MS = pipelineCI.multisampleState;

    if (MS.sampleShadingEnable) {
        glEnable(GL_SAMPLE_SHADING);
        glMinSampleShading(MS.minSampleShading);
    } else {
        glDisable(GL_SAMPLE_SHADING);
    }

    if (MS.sampleMask > 0) {
        glEnable(GL_SAMPLE_MASK);
        glSampleMaski(0, MS.sampleMask);
    } else {
        glDisable(GL_SAMPLE_MASK);
    }

    if (MS.alphaToCoverageEnable) {
        glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    } else {
        glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }

    // DepthStencilState
    const DepthStencilState &DSS = pipelineCI.depthStencilState;

    if (DSS.depthTestEnable) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    glDepthMask(DSS.depthWriteEnable ? GL_TRUE : GL_FALSE);

    glDepthFunc(ToGLCompareOp(DSS.depthCompareOp));

    if (DSS.stencilTestEnable) {
        glEnable(GL_STENCIL_TEST);
    } else {
        glDisable(GL_STENCIL_TEST);
    }

    glStencilOpSeparate(GL_FRONT,
                        ToGLStencilCompareOp(DSS.front.failOp),
                        ToGLStencilCompareOp(DSS.front.depthFailOp),
                        ToGLStencilCompareOp(DSS.front.passOp));
    glStencilFuncSeparate(GL_FRONT,
                          ToGLCompareOp(DSS.front.compareOp),
                          DSS.front.reference,
                          DSS.front.compareMask);
    glStencilMaskSeparate(GL_FRONT, DSS.front.writeMask);

    glStencilOpSeparate(GL_BACK,
                        ToGLStencilCompareOp(DSS.back.failOp),
                        ToGLStencilCompareOp(DSS.back.depthFailOp),
                        ToGLStencilCompareOp(DSS.back.passOp));
    glStencilFuncSeparate(GL_BACK,
                          ToGLCompareOp(DSS.back.compareOp),
                          DSS.back.reference,
                          DSS.back.compareMask);
    glStencilMaskSeparate(GL_BACK, DSS.back.writeMask);

    // ColorBlendState
    const ColorBlendState &CBS = pipelineCI.colorBlendState;

    for (int i = 0; i < (int)CBS.attachments.size(); i++) {
        const ColorBlendAttachmentState &CBA = CBS.attachments[i];

        if (CBA.blendEnable) {
            glEnablei(GL_BLEND, i);
        } else {
            glDisablei(GL_BLEND, i);
        }

        glBlendEquationSeparatei(i, ToGLBlendOp(CBA.colorBlendOp), ToGLBlendOp(CBA.alphaBlendOp));

        glBlendFuncSeparatei(i,
                             ToGLBlendFactor(CBA.srcColorBlendFactor),
                             ToGLBlendFactor(CBA.dstColorBlendFactor),
                             ToGLBlendFactor(CBA.srcAlphaBlendFactor),
                             ToGLBlendFactor(CBA.dstAlphaBlendFactor));

        glColorMaski(i,
                     (((uint32_t)CBA.colorWriteMask & (uint32_t)ColorComponentBit::R_BIT) == (uint32_t)ColorComponentBit::R_BIT),
                     (((uint32_t)CBA.colorWriteMask & (uint32_t)ColorComponentBit::G_BIT) == (uint32_t)ColorComponentBit::G_BIT),
                     (((uint32_t)CBA.colorWriteMask & (uint32_t)ColorComponentBit::B_BIT) == (uint32_t)ColorComponentBit::B_BIT),
                     (((uint32_t)CBA.colorWriteMask & (uint32_t)ColorComponentBit::A_BIT) == (uint32_t)ColorComponentBit::A_BIT));
    }
    glBlendColor(CBS.blendConstants[0], CBS.blendConstants[1], CBS.blendConstants[2], CBS.blendConstants[3]);
}


std::vector<int64_t> GraphicsAPIOpenGLES::getSupportedColorSwapchainFormats() {
    GLint glMajorVersion = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
    if (glMajorVersion >= 3) {
        return {GL_RGBA8, GL_RGBA8_SNORM, GL_SRGB8_ALPHA8};
    } else {
        return {GL_RGBA8, GL_RGBA8_SNORM};
    }
}

void* GraphicsAPIOpenGLES::createImageView(const ImageViewCreateInfo &imageViewCI) {
    GLuint framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);

    GLenum attachment = imageViewCI.aspect == ImageViewCreateInfo::Aspect::COLOR_BIT ? GL_COLOR_ATTACHMENT0 : GL_DEPTH_ATTACHMENT;

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    if (imageViewCI.view == ImageViewCreateInfo::View::TYPE_2D_ARRAY) {
        //glFramebufferTextureMultiviewOVR(GL_DRAW_FRAMEBUFFER, attachment, (GLuint)(uint64_t)imageViewCI.image, imageViewCI.baseMipLevel, imageViewCI.baseArrayLayer, imageViewCI.layerCount);
    } else if (imageViewCI.view == ImageViewCreateInfo::View::TYPE_2D) {
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, GL_TEXTURE_2D, (GLuint)(uint64_t)imageViewCI.image, imageViewCI.baseMipLevel);
    } else {
        LOG_E("OPENGL: Unknown ImageView View type.");
    }

    GLenum result = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE) {
        //DEBUG_BREAK; // running into GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT  error here
        LOG_E("OPENGL: Framebuffer is not complete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    imageViews[framebuffer] = imageViewCI;
    return (void*)(uint64_t)framebuffer;
}

int64_t GraphicsAPIOpenGLES::selectDepthSwapchainFormat(const std::vector<int64_t> &formats) {
    const std::vector<int64_t> &supportSwapchainFormats = getSupportedDepthSwapchainFormats();

    const std::vector<int64_t>::const_iterator &swapchainFormatIt = std::find_first_of(formats.begin(), formats.end(),
                                                                                       std::begin(supportSwapchainFormats), std::end(supportSwapchainFormats));
    if (swapchainFormatIt == formats.end()) {
        LOG_E("Unable to find supported Depth Swapchain Format");
        return 0;
    }

    return *swapchainFormatIt;
}

GLenum GraphicsAPIOpenGLES::getGLTextureTarget(const ImageCreateInfo &imageCI) {
    GLenum target = 0;
    if (imageCI.dimension == 1) {
        if (imageCI.arrayLayers > 1) {
            target = GL_TEXTURE_1D_ARRAY;
        } else {
            target = GL_TEXTURE_1D;
        }
    } else if (imageCI.dimension == 2) {
        if (imageCI.cubemap) {
            if (imageCI.arrayLayers > 6) {
                target = GL_TEXTURE_CUBE_MAP_ARRAY;
            } else {
                target = GL_TEXTURE_CUBE_MAP;
            }
        } else {
            if (imageCI.sampleCount > 1) {
                if (imageCI.arrayLayers > 1) {
                    target = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
                } else {
                    target = GL_TEXTURE_2D_MULTISAMPLE;
                }
            } else {
                if (imageCI.arrayLayers > 1) {
                    target = GL_TEXTURE_2D_ARRAY;
                } else {
                    target = GL_TEXTURE_2D;
                }
            }
        }
    } else if (imageCI.dimension == 3) {
        target = GL_TEXTURE_3D;
    } else {
        LOG_E("OPENGL: Unknown Dimension for getGLTextureTarget(): %d", imageCI.dimension);
    }
    return target;
}

void GraphicsAPIOpenGLES::setVertexBuffers(void** vertexBuffers, size_t count) {
    const VertexInputState &vertexInputState = pipelines[m_setPipeline].vertexInputState;
    for (size_t i = 0; i < count; i++) {
        GLuint glVertexBufferID = (GLuint)(uint64_t)vertexBuffers[i];
        if (buffers[glVertexBufferID].type != BufferCreateInfo::Type::VERTEX) {
            LOG_E("OpenGL: Provided buffer is not type: VERTEX.");
        }

        glBindBuffer(GL_ARRAY_BUFFER, (GLuint)(uint64_t)vertexBuffers[i]);

        for (const VertexInputBinding &vertexBinding : vertexInputState.bindings) {
            if (vertexBinding.bindingIndex == (uint32_t)i) {
                for (const VertexInputAttribute &vertexAttribute : vertexInputState.attributes) {
                    if (vertexAttribute.bindingIndex == (uint32_t)i) {
                        GLuint attribIndex = vertexAttribute.attribIndex;
                        GLint size = ((GLint)vertexAttribute.vertexType % 4) + 1;
                        GLenum type = (GLenum)vertexAttribute.vertexType >= (GLenum)VertexType::UINT ? GL_UNSIGNED_INT : (GLenum)vertexAttribute.vertexType >= (GLenum)VertexType::INT ? GL_INT
                                                                                                                                                                                       : GL_FLOAT;
                        GLsizei stride = vertexBinding.stride;
                        const void* offset = (const void* )vertexAttribute.offset;
                        glEnableVertexAttribArray(attribIndex);
                        glVertexAttribPointer(attribIndex, size, type, false, stride, offset);
                    }
                }
            }
        }
    }
}

void GraphicsAPIOpenGLES::setIndexBuffer(void* indexBuffer) {
    GLuint glIndexBufferID = (GLuint)(uint64_t)indexBuffer;
    if (buffers[glIndexBufferID].type != BufferCreateInfo::Type::INDEX) {
        LOG_E("OpenGL: Provided buffer is not type: INDEX.");
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBufferID);
    m_setIndexBuffer = glIndexBufferID;
}

void GraphicsAPIOpenGLES::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    GLenum indexType = buffers[m_setIndexBuffer].stride == 4 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
    glDrawElementsInstanced(ToGLTopology(pipelines[m_setPipeline].inputAssemblyState.topology),indexCount, indexType, nullptr,instanceCount);
}

void GraphicsAPIOpenGLES::setDescriptor(const DescriptorInfo &descriptorInfo) {
    GLuint glResource = (GLuint)(uint64_t)descriptorInfo.resource;
    const GLuint &bindingIndex = descriptorInfo.bindingIndex;
    if (descriptorInfo.type == DescriptorInfo::Type::BUFFER) {
        glBindBufferRange(GL_UNIFORM_BUFFER, bindingIndex, glResource, (GLintptr)descriptorInfo.bufferOffset, (GLsizeiptr)descriptorInfo.bufferSize);
    } else if (descriptorInfo.type == DescriptorInfo::Type::IMAGE) {
        glActiveTexture(GL_TEXTURE0 + bindingIndex);
        glBindTexture(getGLTextureTarget(images[glResource]), glResource);
    } else if (descriptorInfo.type == DescriptorInfo::Type::SAMPLER) {
        glBindSampler(bindingIndex, glResource);
    } else {
        LOG_E("OPENGL: Unknown Descriptor Type.");
    }
}

void GraphicsAPIOpenGLES::setBufferData(void* buffer, size_t offset, size_t size, void* data) {
    GLuint glBuffer = (GLuint)(uint64_t)buffer;
    const BufferCreateInfo &bufferCI = buffers[glBuffer];

    GLenum target = 0;
    if (bufferCI.type == BufferCreateInfo::Type::VERTEX) {
        target = GL_ARRAY_BUFFER;
    } else if (bufferCI.type == BufferCreateInfo::Type::INDEX) {
        target = GL_ELEMENT_ARRAY_BUFFER;
    } else if (bufferCI.type == BufferCreateInfo::Type::UNIFORM) {
        target = GL_UNIFORM_BUFFER;
    } else {
        LOG_E("OPENGL: Unknown Buffer Type.");
    }

    if (data) {
        glBindBuffer(target, glBuffer);
        glBufferSubData(target, (GLintptr)offset, (GLsizeiptr)size, data);
        glBindBuffer(target, 0);
    }
}

void GraphicsAPIOpenGLES::setRenderAttachments(void** colorViews, size_t colorViewCount, void* depthStencilView, uint32_t width, uint32_t height, void* pipeline) {
    // Reset Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &setFramebuffer);
    setFramebuffer = 0;

    glGenFramebuffers(1, &setFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, setFramebuffer);

    // Color
    for (size_t i = 0; i < colorViewCount; i++) {
        GLenum attachment = GL_COLOR_ATTACHMENT0;

        GLuint glColorView = (GLuint)(uint64_t)colorViews[i];
        const ImageViewCreateInfo &imageViewCI = imageViews[glColorView];

        if (imageViewCI.view == ImageViewCreateInfo::View::TYPE_2D_ARRAY) {
            //glFramebufferTextureMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, (GLuint)(uint64_t)imageViewCI.image, imageViewCI.baseMipLevel, imageViewCI.baseArrayLayer, imageViewCI.layerCount);
        } else if (imageViewCI.view == ImageViewCreateInfo::View::TYPE_2D) {
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (GLuint)(uint64_t)imageViewCI.image, imageViewCI.baseMipLevel);
        } else {
            LOG_E("OPENGL: Unknown ImageView View type.");
        }
    }
    // DepthStencil
    if (depthStencilView) {
        GLuint glDepthView = (GLuint)(uint64_t)depthStencilView;
        const ImageViewCreateInfo &imageViewCI = imageViews[glDepthView];

        if (imageViewCI.view == ImageViewCreateInfo::View::TYPE_2D_ARRAY) {
            //glFramebufferTextureMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, (GLuint)(uint64_t)imageViewCI.image, imageViewCI.baseMipLevel, imageViewCI.baseArrayLayer, imageViewCI.layerCount);
        } else if (imageViewCI.view == ImageViewCreateInfo::View::TYPE_2D) {
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, (GLuint)(uint64_t)imageViewCI.image, imageViewCI.baseMipLevel);
        } else {
            LOG_E("OPENGL: Unknown ImageView View type.");
        }
    }

    GLenum result = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE) {
        LOG_E("OPENGL: Framebuffer is not complete.");
    }
}

void* GraphicsAPIOpenGLES::getGraphicsBinding() {
    graphicsBinding = {XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR};
    graphicsBinding.display = this->eglDisplay;
    graphicsBinding.config = this->eglConfig;
    graphicsBinding.context = this->eglContext;
    return &graphicsBinding;
}

void GraphicsAPIOpenGLES::setViewports(Viewport* viewports, size_t count) {
    Viewport viewport = viewports[0];
    glViewport((GLint)viewport.x, (GLint)viewport.y, (GLsizei)viewport.width, (GLsizei)viewport.height);
    glDepthRangef(viewport.minDepth, viewport.maxDepth);
}

void GraphicsAPIOpenGLES::setScissors(Rect2D* scissors, size_t count) {
    Rect2D scissor = scissors[0];
    glScissor((GLint)scissor.offset.x, (GLint)scissor.offset.y, (GLsizei)scissor.extent.width, (GLsizei)scissor.extent.height);
}

void GraphicsAPIOpenGLES::beginRendering() {
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    glGenFramebuffers(1, &setFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, setFramebuffer);
}

void GraphicsAPIOpenGLES::endRendering() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &setFramebuffer);
    setFramebuffer = 0;

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vertexArray);
    vertexArray = 0;
}

void* GraphicsAPIOpenGLES::createPipeline(const PipelineCreateInfo &pipelineCI) {
    GLuint program = glCreateProgram();

    for (const void* const &shader : pipelineCI.shaders)
        glAttachShader(program, (GLuint)(uint64_t)shader);

    glLinkProgram(program);
    glValidateProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        glDeleteProgram(program);
    }

    for (const void* const &shader : pipelineCI.shaders)
        glDetachShader(program, (GLuint)(uint64_t)shader);

    pipelines[program] = pipelineCI;

    return (void*)(uint64_t)program;
}

void GraphicsAPIOpenGLES::clearColor(void* imageView, float r, float g, float b, float a) {
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)(uint64_t)imageView);
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsAPIOpenGLES::clearDepth(void* imageView, float d) {
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)(uint64_t)imageView);
    glClearDepthf(d);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void* GraphicsAPIOpenGLES::createBuffer(const BufferCreateInfo &bufferCI) {
    GLuint buffer = 0;
    glGenBuffers(1, &buffer);

    GLenum target = 0;
    if (bufferCI.type == BufferCreateInfo::Type::VERTEX) {
        target = GL_ARRAY_BUFFER;
    } else if (bufferCI.type == BufferCreateInfo::Type::INDEX) {
        target = GL_ELEMENT_ARRAY_BUFFER;
    } else if (bufferCI.type == BufferCreateInfo::Type::UNIFORM) {
        target = GL_UNIFORM_BUFFER;
    } else {
        LOG_E("OPENGL: Unknown Buffer Type.");
    }

    glBindBuffer(target, buffer);
    glBufferData(target, (GLsizeiptr)bufferCI.size, bufferCI.data, GL_STATIC_DRAW);
    glBindBuffer(target, 0);

    buffers[buffer] = bufferCI;
    return (void*)(uint64_t)buffer;
}

void* GraphicsAPIOpenGLES::createShader(const ShaderCreateInfo &shaderCI) {
    GLenum type = 0;
    switch (shaderCI.type) {
        case ShaderCreateInfo::Type::VERTEX: {
            type = GL_VERTEX_SHADER;
            break;
        }
        case ShaderCreateInfo::Type::TESSELLATION_CONTROL: {
            type = GL_TESS_CONTROL_SHADER;
            break;
        }
        case ShaderCreateInfo::Type::TESSELLATION_EVALUATION: {
            type = GL_TESS_EVALUATION_SHADER;
            break;
        }
        case ShaderCreateInfo::Type::GEOMETRY: {
            type = GL_GEOMETRY_SHADER;
            break;
        }
        case ShaderCreateInfo::Type::FRAGMENT: {
            type = GL_FRAGMENT_SHADER;
            break;
        }
        case ShaderCreateInfo::Type::COMPUTE: {
            type = GL_COMPUTE_SHADER;
            break;
        }
        default:
            LOG_E("OPENGL: Unknown Shader Type.");
    }
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &shaderCI.sourceData, nullptr);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

        glDeleteShader(shader);
        shader = 0;
    }
    return (void*)(uint64_t)shader;
}

XrSwapchainImageBaseHeader* GraphicsAPIOpenGLES::allocateSwapchainImageData(XrSwapchain swapchain, SwapchainType type, uint32_t count) {
    swapchainImagesMap[swapchain].first = type;
    swapchainImagesMap[swapchain].second.resize(count, {XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR});
    return reinterpret_cast<XrSwapchainImageBaseHeader*>(swapchainImagesMap[swapchain].second.data());
}

void GraphicsAPIOpenGLES::destroyImageView(void*& imageView) {
    GLuint framebuffer = (GLuint)(uint64_t)imageView;
    imageViews.erase(framebuffer);
    glDeleteFramebuffers(1, &framebuffer);
    imageView = nullptr;
}

void GraphicsAPIOpenGLES::freeSwapchainImageData(XrSwapchain swapchain) {
    swapchainImagesMap[swapchain].second.clear();
    swapchainImagesMap.erase(swapchain);
}

void GraphicsAPIOpenGLES::destroyPipeline(void*& pipeline) {
    GLint program = (GLuint)(uint64_t)pipeline;
    pipelines.erase(program);
    glDeleteProgram(program);
    pipeline = nullptr;
}

void GraphicsAPIOpenGLES::destroyShader(void*& shader) {
    GLuint glShader = (GLuint)(uint64_t)shader;
    glDeleteShader(glShader);
    shader = nullptr;
}

void GraphicsAPIOpenGLES::destroyBuffer(void* &buffer) {
    GLuint glBuffer = (GLuint)(uint64_t)buffer;
    buffers.erase(glBuffer);
    glDeleteBuffers(1, &glBuffer);
    buffer = nullptr;
}


// IGraphicsAPI

unsigned int GraphicsAPIOpenGLES::createShader(ShaderCreateInfo::Type type) {
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

void GraphicsAPIOpenGLES::compileShader(unsigned int shaderID, const std::string& source) {
    const char* sourceCStr = source.c_str();
    GL_CALL(glShaderSource(shaderID, 1, &sourceCStr, nullptr));
    GL_CALL(glCompileShader(shaderID));
}

void GraphicsAPIOpenGLES::attachShader(unsigned int programID, unsigned int shaderID) {
    GL_CALL(glAttachShader(programID, shaderID));
}

unsigned int GraphicsAPIOpenGLES::createProgram() {
    unsigned int programID;
    GL_CALL(programID = glCreateProgram());
    return programID;
}

void GraphicsAPIOpenGLES::linkProgram(unsigned int programID) {
    GL_CALL(glLinkProgram(programID));
}

void GraphicsAPIOpenGLES::useProgram(unsigned int programID) {
    GL_CALL(glUseProgram(programID));
}

void GraphicsAPIOpenGLES::deleteShader(unsigned int shaderID) {
    GL_CALL(glDeleteShader(shaderID));
}

void GraphicsAPIOpenGLES::deleteProgram(unsigned int programID) {
    // TODO fix this. probably need to delete/detach shaders
    GL_CALL(glDeleteProgram(programID));
}

std::string GraphicsAPIOpenGLES::getShaderLog(unsigned int shaderID) {
    char log[1024];
    GL_CALL(glGetShaderInfoLog(shaderID, sizeof(log), nullptr, log));
    return std::string(log);
}

std::string GraphicsAPIOpenGLES::getProgramLog(unsigned int programID) {
    char log[1024];
    GL_CALL(glGetProgramInfoLog(programID, sizeof(log), nullptr, log));
    return std::string(log);
}

unsigned int GraphicsAPIOpenGLES::getUniformLocation(unsigned int programId, const std::string& name) {
    unsigned int uniformLocation;
    GL_CALL(uniformLocation = glGetUniformLocation(programId, name.c_str()));
    return uniformLocation;
}

void GraphicsAPIOpenGLES::uniform1i(unsigned int location, int value) {
    GL_CALL(glUniform1i(location, value));
}

void GraphicsAPIOpenGLES::uniform1f(unsigned int location, float value) {
    GL_CALL(glUniform1f(location, value));
}

void GraphicsAPIOpenGLES::uniform2fv(unsigned int location, const float* value) {
    GL_CALL(glUniform2fv(location, 1, value));
}

void GraphicsAPIOpenGLES::uniform2f(unsigned int location, float v0, float v1) {
    GL_CALL(glUniform2f(location, v0, v1));
}

void GraphicsAPIOpenGLES::uniform3fv(unsigned int location, const float* value) {
    GL_CALL(glUniform3fv(location, 1, value));
}

void GraphicsAPIOpenGLES::uniform3f(unsigned int location, float v0, float v1, float v2) {
    GL_CALL(glUniform3f(location, v0, v1, v2));
}

void GraphicsAPIOpenGLES::uniform4fv(unsigned int location, const float* value) {
    GL_CALL(glUniform4fv(location, 1, value));
}

void GraphicsAPIOpenGLES::uniform4f(unsigned int location, float v0, float v1, float v2, float v3) {
    GL_CALL(glUniform4f(location, v0, v1, v2, v3));
}

void GraphicsAPIOpenGLES::uniformMatrix2fv(unsigned int location, const float* value) {
    GL_CALL(glUniformMatrix2fv(location, 1, GL_FALSE, value));
}

void GraphicsAPIOpenGLES::uniformMatrix3fv(unsigned int location, const float* value) {
    GL_CALL(glUniformMatrix3fv(location, 1, GL_FALSE, value));
}

void GraphicsAPIOpenGLES::uniformMatrix4fv(unsigned int location, const float* value) {
    GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, value));
}

void GraphicsAPIOpenGLES::clearColor(float r, float g, float b, float a) {
    GL_CALL(glClearColor(r,g ,b, a));
}

void GraphicsAPIOpenGLES::bindFrameBuffer(IGraphicsAPI::FrameBufferTarget target, unsigned int bufferId) {
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

void GraphicsAPIOpenGLES::enable(IGraphicsAPI::Capability capability) {
//    GLenum glCapability;
//
//    switch (capability) {
//        case IGraphicsAPI::Capability::MULTISAMPLE:
//            glCapability = GL_MULTISAMPLE;
//            break;
//        case IGraphicsAPI::Capability::FRAMEBUFFER_SRGB:
//            glCapability = GL_FRAMEBUFFER_SRGB;
//            break;
//        default:
//            throw std::runtime_error("Invalid capability");
//    }
//
//    GL_CALL(glEnable(glCapability));
}

void GraphicsAPIOpenGLES::disable(IGraphicsAPI::Capability capability) {
//    GLenum glCapability;
//
//    switch (capability) {
//        case IGraphicsAPI::Capability::MULTISAMPLE:
//            glCapability = GL_MULTISAMPLE;
//            break;
//        case IGraphicsAPI::Capability::FRAMEBUFFER_SRGB:
//            glCapability = GL_FRAMEBUFFER_SRGB;
//            break;
//        default:
//            throw std::runtime_error("Invalid capability");
//    }
//
//    GL_CALL(glDisable(glCapability));
}

void GraphicsAPIOpenGLES::genVertexArrays(unsigned int n, unsigned int* arrays) {
    GL_CALL(glGenVertexArrays(n, arrays));
}

void GraphicsAPIOpenGLES::bindVertexArray(unsigned int vao) {
    GL_CALL(glBindVertexArray(vao));
}

void GraphicsAPIOpenGLES::genBuffer(int size, unsigned int* vaos) {
    GL_CALL(glGenBuffers(size, vaos));
}

void GraphicsAPIOpenGLES::bindBuffer(IGraphicsAPI::BufferTarget target, unsigned int bufferId) {
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
//        case IGraphicsAPI::BufferTarget::QUERY_BUFFER:
//            glTarget = GL_QUERY_BUFFER;
//            break;
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

void GraphicsAPIOpenGLES::bufferData(IGraphicsAPI::BufferTarget target, size_t size, void* data, IGraphicsAPI::DataUsage usage) {
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
//        case IGraphicsAPI::BufferTarget::QUERY_BUFFER:
//            glTarget = GL_QUERY_BUFFER;
//            break;
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

void GraphicsAPIOpenGLES::enableVertexAttribArray(unsigned int index) {
    GL_CALL(glEnableVertexAttribArray(index));
}

void GraphicsAPIOpenGLES::vertexAttribPointer(unsigned int index, int size, IGraphicsAPI::DataType type, bool normalized, size_t stride, const void* offset) {
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

void GraphicsAPIOpenGLES::drawElements(IGraphicsAPI::PrimitiveTopology mode, int count, IGraphicsAPI::DataType type, const void* indices) {
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

unsigned int GraphicsAPIOpenGLES::getUniformBlockIndex(unsigned int programId, const char* uniformBlockName) {
    unsigned int index;
    GL_CALL(index = glGetUniformBlockIndex(programId, uniformBlockName));
    return index;
};

void GraphicsAPIOpenGLES::uniformBlockBinding(unsigned int programId, unsigned int uniformBlockIndex, unsigned int uniformBlockBinding) {
    if (uniformBlockIndex != GL_INVALID_INDEX) {
        GL_CALL(GL_CALL(glUniformBlockBinding(programId, uniformBlockIndex, uniformBlockBinding)));
    } else {
        LOG_E("Invalid uniformBlockIndex %d", uniformBlockIndex);
    }
}

void GraphicsAPIOpenGLES::deleteTexture(unsigned int n, unsigned int* textureId) {
    // TODO FIX THIS Error: OpenGL error in glDeleteTextures(n, textureId): 1282
    GL_CALL(glDeleteTextures(n, textureId));
}

void GraphicsAPIOpenGLES::genTextures(unsigned int count, unsigned int* textures) {
    GL_CALL(glGenTextures(count, textures));
}

void GraphicsAPIOpenGLES::texParameter(IGraphicsAPI::TextureTarget target, IGraphicsAPI::TextureParameterType paramName, IGraphicsAPI::TextureParameterOption paramValue) {
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

void GraphicsAPIOpenGLES::texImage2D(IGraphicsAPI::TextureTarget target,
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
            glInternalFormat = GL_SRGB8_ALPHA8;
            break;
        case IGraphicsAPI::TextureFormat::RGB:
            glInternalFormat = GL_RGB;
            break;
        case IGraphicsAPI::TextureFormat::RGBA:
            glInternalFormat = GL_RGBA;
            break;
        case IGraphicsAPI::TextureFormat::RED:
            glInternalFormat = GL_LUMINANCE;
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
            glFormat = GL_SRGB8_ALPHA8;
            break;
        case IGraphicsAPI::TextureFormat::RGB:
            glFormat = GL_RGB;
            break;
        case IGraphicsAPI::TextureFormat::RGBA:
            glFormat = GL_RGBA;
            break;
        case IGraphicsAPI::TextureFormat::RED:
            glFormat = GL_LUMINANCE;
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

void GraphicsAPIOpenGLES::bindTexture(TextureTarget target, unsigned int textureId) {
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

void GraphicsAPIOpenGLES::getTexImage(IGraphicsAPI::TextureTarget target, unsigned int level, IGraphicsAPI::TextureFormat format, DataType dataType, void* pixels) {
//    GLenum glTarget;
//
//    switch (target) {
//        case IGraphicsAPI::TextureTarget::TEXTURE_1D:
//            glTarget = GL_TEXTURE_1D;
//            break;
//        case IGraphicsAPI::TextureTarget::TEXTURE_1D_ARRAY:
//            glTarget = GL_TEXTURE_1D_ARRAY;
//            break;
//        case IGraphicsAPI::TextureTarget::TEXTURE_2D:
//            glTarget = GL_TEXTURE_2D;
//            break;
//        case IGraphicsAPI::TextureTarget::TEXTURE_2D_ARRAY:
//            glTarget = GL_TEXTURE_2D_ARRAY;
//            break;
//        case IGraphicsAPI::TextureTarget::TEXTURE_2D_MULTISAMPLE:
//            glTarget = GL_TEXTURE_2D_MULTISAMPLE;
//            break;
//        case IGraphicsAPI::TextureTarget::TEXTURE_2D_MULTISAMPLE_ARRAY:
//            glTarget = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
//            break;
//        default:
//            throw std::runtime_error("Invalid Texture target");
//    }
//
//    GLenum glFormat;
//
//    switch (format) {
//        case IGraphicsAPI::TextureFormat::SRGB:
//            glFormat = GL_SRGB;
//            break;
//        case IGraphicsAPI::TextureFormat::SRGB_ALPHA:
//            glFormat = GL_SRGB_ALPHA;
//            break;
//        case IGraphicsAPI::TextureFormat::RGB:
//            glFormat = GL_RGB;
//            break;
//        case IGraphicsAPI::TextureFormat::RGBA:
//            glFormat = GL_RGBA;
//            break;
//        case IGraphicsAPI::TextureFormat::RED:
//            glFormat = GL_RED;
//            break;
//        case IGraphicsAPI::TextureFormat::RGBA16F:
//            glFormat = GL_RGBA16F;
//            break;
//        default:
//            throw std::runtime_error("Invalid Texture input format");
//    }
//
//    GLenum glDataType;
//
//    switch (dataType) {
//        case IGraphicsAPI::DataType::BYTE:
//            glDataType = GL_BYTE;
//            break;
//        case IGraphicsAPI::DataType::UBYTE:
//            glDataType = GL_UNSIGNED_BYTE;
//            break;
//        case IGraphicsAPI::DataType::FLOAT:
//            glDataType = GL_FLOAT;
//            break;
//        default:
//            throw std::runtime_error("Invalid Texture Data type");
//    }
//
//    GL_CALL(glGetTexImage(glTarget, 0, glFormat, glDataType, pixels));
}

void GraphicsAPIOpenGLES::pixelStore(IGraphicsAPI::PixelAlignment alignment, unsigned int value) {
    GLenum GLAlignment;

    switch (alignment) {
        case IGraphicsAPI::PixelAlignment::PACK_ALIGNMENT:
            GLAlignment = GL_PACK_ALIGNMENT;
            break;
        case IGraphicsAPI::PixelAlignment::UNPACK_ALIGNMENT:
            GLAlignment = GL_UNPACK_ALIGNMENT;
            break;
        default:
            throw std::runtime_error("Pixel Alignment type");
    }
    GL_CALL(glPixelStorei(GLAlignment, value));
}

void GraphicsAPIOpenGLES::activeTexture(unsigned int textureUnit) {
    GL_CALL(glActiveTexture(GL_TEXTURE0 + textureUnit));
}

void GraphicsAPIOpenGLES::bindBufferRange(IGraphicsAPI::BufferTarget target, unsigned int index, unsigned int buffer, size_t offset, size_t size) {
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
//        case IGraphicsAPI::BufferTarget::QUERY_BUFFER:
//            glTarget = GL_QUERY_BUFFER;
//            break;
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

void GraphicsAPIOpenGLES::genFrameBuffers(unsigned int count, unsigned int* fbos) {
    GL_CALL(glGenFramebuffers(count, fbos));
}

void GraphicsAPIOpenGLES::framebufferTexture2D(IGraphicsAPI::FrameBufferTarget target, unsigned int attachment, IGraphicsAPI::FBOTextureTarget texTarget, unsigned int textureId, unsigned int level) {
    // TODO confirm if GL_FRAMEBUFFER/GL_TEXTURE_2D can ever be different options in here
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, GL_TEXTURE_2D, textureId, level));
}

void GraphicsAPIOpenGLES::genRenderBuffer(unsigned int n, unsigned int* rbos) {
    GL_CALL(glGenRenderbuffers(n, rbos));
}

void GraphicsAPIOpenGLES::bindRenderBuffer(IGraphicsAPI::RenderBufferTarget target, unsigned int renderBuffer) {
    GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer));
}

void GraphicsAPIOpenGLES::renderBufferStorage(IGraphicsAPI::RenderBufferTarget target, IGraphicsAPI::RenderBufferFormat format, unsigned int width, unsigned int height) {
    GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height));
}

void GraphicsAPIOpenGLES::frameBufferRenderBuffer(IGraphicsAPI::FrameBufferTarget frameBufferTarget, IGraphicsAPI::FrameBufferAttachment attachment, IGraphicsAPI::RenderBufferTarget renderBufferTarget, unsigned int rbo) {
    GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo));
}

void GraphicsAPIOpenGLES::drawBuffers(unsigned int n, unsigned int* bufs) {
    // todo handle other buffers
    std::unique_ptr<unsigned int[]> temp = std::make_unique<unsigned int[]>(n);
    for (int i = 0; i < n; ++i) {
        temp[i] = GL_COLOR_ATTACHMENT0 + bufs[i];
    }
    GL_CALL(glDrawBuffers(n, temp.get()));
}

void GraphicsAPIOpenGLES::bufferSubData(IGraphicsAPI::BufferTarget target, size_t offset, size_t size, const void* data) {
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
//        case IGraphicsAPI::BufferTarget::QUERY_BUFFER:
//            glTarget = GL_QUERY_BUFFER;
//            break;
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

void GraphicsAPIOpenGLES::drawArrays(IGraphicsAPI::PrimitiveTopology mode, unsigned int start, unsigned int count) {
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

void GraphicsAPIOpenGLES::clearBuffers(const std::vector<IGraphicsAPI::ClearBufferTarget>& targets) {
    GLuint clearMask = 0;

    for (const auto& target : targets) {
        switch (target) {
            case IGraphicsAPI::ClearBufferTarget::COLOR:
                clearMask |= GL_COLOR_BUFFER_BIT;
                break;
            case IGraphicsAPI::ClearBufferTarget::DEPTH:
                clearMask |= GL_DEPTH_BUFFER_BIT;
                break;
            case IGraphicsAPI::ClearBufferTarget::STENCIL:
                clearMask |= GL_STENCIL_BUFFER_BIT;
                break;
        }
    }

    GL_CALL(glClear(clearMask));
}

void GraphicsAPIOpenGLES::polygonMode(IGraphicsAPI::PolygonModeFace face, IGraphicsAPI::PolygonModeType mode) {
//    GLenum glMode = 0;
//
//    switch (mode) {
//        case PolygonModeType::POINT:
//            glMode = GL_POINT;
//            break;
//        case PolygonModeType::LINE:
//            glMode = GL_LINE;
//            break;
//        case PolygonModeType::FILL:
//            glMode = GL_FILL;
//            break;
//    }
//
//    glPolygonMode(GL_FRONT_AND_BACK, glMode);
}

void GraphicsAPIOpenGLES::drawBuffer(unsigned int bufferId) {
    // TODO add support for other values
    //GL_CALL(glDrawBuffer(GL_COLOR_ATTACHMENT0 + bufferId));
}







    
} // namespace clay

#endif
// standard lib
// third party
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
// project
// class
#include "clay/graphics/common/Renderer.h"

namespace clay {

const int Renderer::MAX_LIGHTS = 16;

Renderer::Renderer(const glm::vec2& screenDim, ShaderProgram& spriteShader, ShaderProgram& text2Shader,
                   ShaderProgram& mvpShader, Mesh& rectPlane, ShaderProgram& frameBufferShader, 
                   ShaderProgram& bloomFinalShader, IGraphicsAPI& graphicsAPI)
    : mSpriteShader_(spriteShader),
    mMVPShader_(mvpShader),
    mTextShader_(text2Shader),
    mRectPlane_(rectPlane),
    mBlurShader_(frameBufferShader),
    mBloomFinalShader_(bloomFinalShader),
    mAttachments_{0, 1},
    mGraphicsAPI_(graphicsAPI) {
    mDefaultProjection_ = glm::ortho(0.0f, screenDim.x, 0.0f, screenDim.y);

    // Rect
    float verticesRect[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    // todo use this
    unsigned int indicesRect[] = {
        2, 1, 0,
        0, 3, 2
    };

    unsigned int VBORect, EBORect;

    mGraphicsAPI_.genVertexArrays(1, &mRectVAO_);
    mGraphicsAPI_.genBuffer(1, &VBORect);
    mGraphicsAPI_.genBuffer(1, &EBORect);

    mGraphicsAPI_.bindVertexArray(mRectVAO_);
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, VBORect);
    mGraphicsAPI_.bufferData(IGraphicsAPI::BufferTarget::ARRAY_BUFFER,  sizeof(verticesRect),verticesRect, IGraphicsAPI::DataUsage::STATIC_DRAW);

    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, EBORect);
    mGraphicsAPI_.bufferData(IGraphicsAPI::BufferTarget::ARRAY_BUFFER,  sizeof(indicesRect), indicesRect, IGraphicsAPI::DataUsage::STATIC_DRAW);

    // Set the vertex attribute pointers
    mGraphicsAPI_.vertexAttribPointer(0, 3, IGraphicsAPI::DataType::FLOAT, false, 3 * sizeof(float), (void*)0);
    mGraphicsAPI_.enableVertexAttribArray(0);

    // quad for frame buffer texture
    {
        float verticesRect2[] = {
            // Positions         // Texture Coords
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,  // Bottom Left
            1.0f, -1.0f, 0.0f,  1.0f, 0.0f,  // Bottom Right
            1.0f,  1.0f, 0.0f,  1.0f, 1.0f,  // Top Right
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f   // Top Left
        };

        unsigned int indicesRect2[] = {
            0, 1, 2,
            0, 2, 3
        };

        unsigned int VBORect2, EBORect2;

        mGraphicsAPI_.genVertexArrays(1, &mFrameVAO_);
        mGraphicsAPI_.genBuffer(1, &VBORect2);
        mGraphicsAPI_.genBuffer(1, &EBORect2);

        mGraphicsAPI_.bindVertexArray(mFrameVAO_);
        mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, VBORect2);
        mGraphicsAPI_.bufferData(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, sizeof(verticesRect2), verticesRect2, IGraphicsAPI::DataUsage::STATIC_DRAW);

        mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ELEMENT_ARRAY_BUFFER, EBORect2);
        mGraphicsAPI_.bufferData(IGraphicsAPI::BufferTarget::ELEMENT_ARRAY_BUFFER, sizeof(indicesRect2), indicesRect2, IGraphicsAPI::DataUsage::STATIC_DRAW);

        // Set the vertex attribute pointers
        // Position attribute
        mGraphicsAPI_.vertexAttribPointer(0, 3, IGraphicsAPI::DataType::FLOAT, false, 5 * sizeof(float), (void*)0);
        mGraphicsAPI_.enableVertexAttribArray(0);
        // Texture coordinate attribute
        mGraphicsAPI_.vertexAttribPointer(1, 2, IGraphicsAPI::DataType::FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        mGraphicsAPI_.enableVertexAttribArray(1);
    }

    // Set up Line properties
    float lineVertices[] = {
        0,0,0,
        0,0,0,
    };

    mGraphicsAPI_.genVertexArrays(1, &mLineVAO_);
    mGraphicsAPI_.genBuffer(1, &mLineVBO_);

    mGraphicsAPI_.bindVertexArray(mLineVAO_);
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, mLineVBO_);
    mGraphicsAPI_.bufferData(IGraphicsAPI::BufferTarget::ARRAY_BUFFER,  sizeof(lineVertices), lineVertices, IGraphicsAPI::DataUsage::STATIC_DRAW);

    // Set the vertex attribute pointers
    mGraphicsAPI_.vertexAttribPointer(0, 3, IGraphicsAPI::DataType::FLOAT, false, 3 * sizeof(float), (void*)0);
    mGraphicsAPI_.enableVertexAttribArray(0);

    // Create Camera UBO to hold View and Projection matrix
    mGraphicsAPI_.genBuffer(1, &mCameraUBO_);
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, mCameraUBO_);
    mGraphicsAPI_.bufferData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, IGraphicsAPI::DataUsage::STATIC_DRAW);
    // define the range of the buffer that links to a uniform binding point
    mGraphicsAPI_.bindBufferRange(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0, mCameraUBO_, 0, 2 * sizeof(glm::mat4));
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0);

    // Create Light UBO to hold light data
    mGraphicsAPI_.genBuffer(1, &mLightUBO_);
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, mLightUBO_);
    // Calculate the size of the light buffer data with padding
    size_t lightBufferSize = sizeof(glm::vec4)                // numLights
                            + MAX_LIGHTS * sizeof(glm::vec4)  // lightPositions
                            + MAX_LIGHTS * sizeof(glm::vec4); // lightColors

    mGraphicsAPI_.bufferData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, lightBufferSize, NULL, IGraphicsAPI::DataUsage::STATIC_DRAW);
    // Bind the buffer to the uniform binding point 1 (as an example)
    mGraphicsAPI_.bindBufferRange(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 1, mLightUBO_, 0, lightBufferSize);
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0);

    {
        // set up floating point framebuffer to render scene to
        mGraphicsAPI_.genFrameBuffers(1, &hdrFBO_);
        mGraphicsAPI_.bindFrameBuffer(IGraphicsAPI::FrameBufferTarget::FRAMEBUFFER, hdrFBO_);
        mGraphicsAPI_.genTextures(2, colorBuffers_);

        for (unsigned int i = 0; i < 2; i++) {
            mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, colorBuffers_[i]);
            mGraphicsAPI_.texImage2D(
                IGraphicsAPI::TextureTarget::TEXTURE_2D,
                0,
                IGraphicsAPI::TextureFormat::RGBA16F,
                screenDim.x,
                screenDim.y,
                0,
                IGraphicsAPI::TextureFormat::RGBA,
                IGraphicsAPI::DataType::FLOAT,
                NULL
            );
            graphicsAPI.texParameter(IGraphicsAPI::TextureTarget::TEXTURE_2D, IGraphicsAPI::TextureParameterType::TEXTURE_MIN_FILTER, IGraphicsAPI::TextureParameterOption::LINEAR);
            graphicsAPI.texParameter(IGraphicsAPI::TextureTarget::TEXTURE_2D, IGraphicsAPI::TextureParameterType::TEXTURE_MAG_FILTER, IGraphicsAPI::TextureParameterOption::LINEAR);
            graphicsAPI.texParameter(IGraphicsAPI::TextureTarget::TEXTURE_2D, IGraphicsAPI::TextureParameterType::TEXTURE_WRAP_S, IGraphicsAPI::TextureParameterOption::CLAMP_TO_EDGE);
            graphicsAPI.texParameter(IGraphicsAPI::TextureTarget::TEXTURE_2D, IGraphicsAPI::TextureParameterType::TEXTURE_WRAP_T, IGraphicsAPI::TextureParameterOption::CLAMP_TO_EDGE);

            // attach texture to framebuffer
            graphicsAPI.framebufferTexture2D(IGraphicsAPI::FrameBufferTarget::FRAMEBUFFER, i, IGraphicsAPI::FBOTextureTarget::TEXTURE_2D, colorBuffers_[i], 0);
        }

        // create and attach depth buffer (renderbuffer)
        unsigned int rboDepth;
        mGraphicsAPI_.genRenderBuffer(1, &rboDepth);
        mGraphicsAPI_.bindRenderBuffer(IGraphicsAPI::RenderBufferTarget::RENDERBUFFER, rboDepth);
        mGraphicsAPI_.renderBufferStorage(IGraphicsAPI::RenderBufferTarget::RENDERBUFFER, IGraphicsAPI::RenderBufferFormat::DEPTH_COMPONENT, screenDim.x, screenDim.y);
        mGraphicsAPI_.frameBufferRenderBuffer(IGraphicsAPI::FrameBufferTarget::FRAMEBUFFER, IGraphicsAPI::FrameBufferAttachment::DEPTH_ATTACHMENT, IGraphicsAPI::RenderBufferTarget::RENDERBUFFER, rboDepth);
        // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering        
        mGraphicsAPI_.drawBuffers(2, mAttachments_);
        mGraphicsAPI_.bindFrameBuffer(IGraphicsAPI::FrameBufferTarget::FRAMEBUFFER, 0);

        // ping-pong-framebuffer for blurring
        mGraphicsAPI_.genFrameBuffers(2, pingpongFBO_);
        mGraphicsAPI_.genFrameBuffers(2, pingpongColorbuffers_);
        for (unsigned int i = 0; i < 2; i++) {
            mGraphicsAPI_.bindFrameBuffer(IGraphicsAPI::FrameBufferTarget::FRAMEBUFFER, pingpongFBO_[i]);
            mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, pingpongColorbuffers_[i]);
            mGraphicsAPI_.texImage2D(
                IGraphicsAPI::TextureTarget::TEXTURE_2D,
                0,
                IGraphicsAPI::TextureFormat::RGBA16F,
                screenDim.x,
                screenDim.y,
                0,
                IGraphicsAPI::TextureFormat::RGBA,
                IGraphicsAPI::DataType::FLOAT,
                NULL
            );
            mGraphicsAPI_.texParameter(IGraphicsAPI::TextureTarget::TEXTURE_2D, IGraphicsAPI::TextureParameterType::TEXTURE_MIN_FILTER, IGraphicsAPI::TextureParameterOption::LINEAR);
            mGraphicsAPI_.texParameter(IGraphicsAPI::TextureTarget::TEXTURE_2D, IGraphicsAPI::TextureParameterType::TEXTURE_MAG_FILTER, IGraphicsAPI::TextureParameterOption::LINEAR);
            // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
            mGraphicsAPI_.texParameter(IGraphicsAPI::TextureTarget::TEXTURE_2D, IGraphicsAPI::TextureParameterType::TEXTURE_WRAP_S, IGraphicsAPI::TextureParameterOption::CLAMP_TO_EDGE);
            mGraphicsAPI_.texParameter(IGraphicsAPI::TextureTarget::TEXTURE_2D, IGraphicsAPI::TextureParameterType::TEXTURE_WRAP_T, IGraphicsAPI::TextureParameterOption::CLAMP_TO_EDGE);
            mGraphicsAPI_.framebufferTexture2D(IGraphicsAPI::FrameBufferTarget::FRAMEBUFFER, 0, IGraphicsAPI::FBOTextureTarget::TEXTURE_2D, pingpongColorbuffers_[i], 0);
        }

    }
    // by default, disable bloom
    setBloom(false);
}

Renderer::~Renderer() {}

void Renderer::setCamera(const Camera* camera) {
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, mCameraUBO_);

    if (camera != nullptr) {
        mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->getViewMatrix()));
        mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera->getProjectionMatrix()));
    } else {
        mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4(1)));
        mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(mDefaultProjection_));
    }
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0);
}

void Renderer::setLightSources(const std::vector<std::unique_ptr<LightSource>>& lights) const {
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, mLightUBO_);

    // Prepare data for the UBO
    std::vector<glm::vec4> lightPositions(MAX_LIGHTS); // Use vec4 for alignment
    std::vector<glm::vec4> lightColors(MAX_LIGHTS);    // Use vec4 for alignment
    int numPointLights = 0;

    for (const auto& light : lights) {
        if (numPointLights < MAX_LIGHTS) {
            lightPositions[numPointLights] = glm::vec4(light->getPosition(), 0.0f); // Add zero padding
            lightColors[numPointLights] = light->getColor();
            numPointLights++;
        }
    }

    // Fill UBO data
    glm::vec4 temp = {numPointLights, 0, 0, 0};
    mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0, sizeof(glm::vec4), &temp);// numLights

    // Set padding
    size_t offset = sizeof(glm::vec4); // Offset for padding
    mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, offset, sizeof(glm::vec4) * MAX_LIGHTS, lightPositions.data()); // lightPositions

    // Set light colors
    offset += sizeof(glm::vec4) * MAX_LIGHTS;
    mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, offset, sizeof(glm::vec4) * MAX_LIGHTS, lightColors.data()); // lightColors

    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0);
}

void Renderer::setLightSources(const std::vector<LightSource*>& lights) const {
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, mLightUBO_);

    // Prepare data for the UBO
    std::vector<glm::vec4> lightPositions(MAX_LIGHTS); // Use vec4 for alignment
    std::vector<glm::vec4> lightColors(MAX_LIGHTS);    // Use vec4 for alignment
    int numPointLights = 0;

    for (const auto& light : lights) {
        if (numPointLights < MAX_LIGHTS) {
            lightPositions[numPointLights] = glm::vec4(light->getPosition(), 0.0f); // Add zero padding
            lightColors[numPointLights] = light->getColor();
            numPointLights++;
        }
    }

    // Fill UBO data
    glm::vec4 temp = {numPointLights, 0, 0, 0};
    mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0, sizeof(glm::vec4), &temp);// numLights

    // Set padding
    size_t offset = sizeof(glm::vec4); // Offset for padding
    mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, offset, sizeof(glm::vec4) * MAX_LIGHTS, lightPositions.data()); // lightPositions

    // Set light colors
    offset += sizeof(glm::vec4) * MAX_LIGHTS;
    mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, offset, sizeof(glm::vec4) * MAX_LIGHTS, lightColors.data()); // lightColors

    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::UNIFORM_BUFFER, 0);
}

void Renderer::renderSprite(unsigned int textureId, const glm::mat4& modelMat, const glm::vec4& theColor) const {
    mSpriteShader_.bind();
    mGraphicsAPI_.activeTexture(0);
    mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, textureId);

    mSpriteShader_.setMat4("uModel", modelMat);

    // Draw the whole texture
    mSpriteShader_.setInt("uTexture", 0);
    mSpriteShader_.setVec2("uSubImageTopLeft", {0.f, 0.f});
    mSpriteShader_.setVec2("uSubImageSize", {1.f, 1.f});
    mSpriteShader_.setVec4("uColor", theColor);

    mRectPlane_.render(mSpriteShader_);
}

void Renderer::renderSprite(SpriteSheet::Sprite& theSprite, const glm::mat4& modelMat, const glm::vec4& theColor) const {
    mSpriteShader_.bind();
    mGraphicsAPI_.activeTexture(0);
    mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, theSprite.parentSpriteSheet.getTextureId());

    mSpriteShader_.setMat4("uModel", modelMat);
    mSpriteShader_.setInt("uTexture", 0);

    float subImageTopLeftX = static_cast<float>(theSprite.gridIndex.x * theSprite.spriteSize.x) / theSprite.parentSpriteSheet.getSheetSize()[0];
    float subImageTopLeftY = static_cast<float>(theSprite.gridIndex.y * theSprite.spriteSize.y) / theSprite.parentSpriteSheet.getSheetSize()[1];
    mSpriteShader_.setVec2("uSubImageTopLeft", {subImageTopLeftX, subImageTopLeftY});
    float normalWidth = (float)theSprite.spriteSize[0] / (float)theSprite.parentSpriteSheet.getSheetSize()[0];
    float normalHeight = (float)theSprite.spriteSize[1] / (float)theSprite.parentSpriteSheet.getSheetSize()[1];
    mSpriteShader_.setVec2("uSubImageSize", {normalWidth, normalHeight});
    mSpriteShader_.setVec4("uColor", theColor);

    mRectPlane_.render(mSpriteShader_);
}

void Renderer::renderText(const std::string& text, const glm::vec2& position, const Font& font, float scale, const glm::vec3& color) {
    float xPos = position.x;	
    mTextShader_.bind();
    // TODO alpha color
    mTextShader_.setVec3("textColor", color);
    mGraphicsAPI_.activeTexture(0);
    mGraphicsAPI_.bindVertexArray(font.getVAO());

    for (const char& c : text) {
        const Font::Character* ch = font.getCharInfo(c);
        if (ch != nullptr) {
            float xpos = xPos + ch->bearing.x * scale;
            float ypos = position.y - (ch->size.y - ch->bearing.y) * scale;

            float w = ch->size.x * scale;
            float h = ch->size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            // render glyph texture over quad
            mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, ch->textureId);
            // update content of VBO memory
            mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, font.getVBO());
            mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, 0, sizeof(vertices), vertices);

            mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, 0);
            // TODO set only once?
            mTextShader_.setMat4("uModel", glm::mat4(1));
            // render quad
            mGraphicsAPI_.drawArrays(IGraphicsAPI::PrimitiveTopology::TRIANGLE_LIST, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
            xPos += (ch->advance >> 6) * scale;
        }
    }
}

void Renderer::renderTextCentered(const std::string& text, const glm::vec2& position, const Font& font, float scale, const glm::vec4& color) {
    // activate corresponding render state	
    mTextShader_.bind();
    mTextShader_.setVec3("textColor", color);
    mGraphicsAPI_.activeTexture(0);
    mGraphicsAPI_.bindVertexArray(font.getVAO());

     // Calculate the total width of the text
    float textWidth = 0;

    // Calculate the total width of the text
    float totalWidth = 0.0f;
    for (const char& c : text) {
        const Font::Character* ch = font.getCharInfo(c);
        if (ch != nullptr) {
            totalWidth += (ch->advance >> 6) * scale;
        }
    }

    float startX = position.x - totalWidth / 2.0f;

    // iterate through all characters
    for (const char& c : text) {
        const Font::Character* ch = font.getCharInfo(c);
        if (ch != nullptr) {
            float xpos = startX + ch->bearing.x * scale;
            float ypos = position.y - (ch->size.y - ch->bearing.y) * scale;

            float w = ch->size.x * scale;
            float h = ch->size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            // render glyph texture over quad
            mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, ch->textureId);
            // update content of VBO memory
            mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, font.getVBO());
            mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, 0, sizeof(vertices), vertices);

            mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, 0);
            mTextShader_.setMat4("uModel", glm::mat4(1));
            // render quad
            
            mGraphicsAPI_.drawArrays(IGraphicsAPI::PrimitiveTopology::TRIANGLE_LIST, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            startX += (ch->advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
    }
    
    mGraphicsAPI_.bindVertexArray(0);
    mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, 0);
}

void Renderer::renderTextNormalized(const std::string& text, const glm::mat4& modelMat, const Font& font, const glm::vec3& scale, const glm::vec3& color) {
    // activate corresponding render state	
    mTextShader_.bind();
    mTextShader_.setVec3("textColor", color);
    mGraphicsAPI_.activeTexture(0);
    mGraphicsAPI_.bindVertexArray(font.getVAO());

    // Calculate the total width of the text
    float totalWidth = 0.0f;
    for (const char& c : text) {
        const Font::Character* ch = font.getCharInfo(c);
        if (ch != nullptr) {
            totalWidth += (ch->advance >> 6) * scale.x;
        }
    }

    float startX = -totalWidth / 2.0f; // Center horizontally around the origin

    // iterate through all characters
    for (const char& c : text) {
        const Font::Character* ch = font.getCharInfo(c);

        if (ch != nullptr) {
            float xpos = startX + ch->bearing.x * scale.x;
            float ypos = - (ch->size.y - ch->bearing.y) * scale.y; // Adjust for Y-axis to center vertically around the origin

            float w = ch->size.x * scale.x;
            float h = ch->size.y * scale.y;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            // render glyph texture over quad
            mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, ch->textureId);
            // update content of VBO memory
            mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, font.getVBO());
            mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, 0, sizeof(vertices), vertices);

            mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, 0);
            // Apply the model matrix to the shader
            mTextShader_.setMat4("uModel", modelMat);

            // render quad
            mGraphicsAPI_.drawArrays(IGraphicsAPI::PrimitiveTopology::TRIANGLE_LIST, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            startX += (ch->advance >> 6) * scale.x; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
    }
    mGraphicsAPI_.bindVertexArray(0);
    mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, 0);
}

void Renderer::renderRectangleSimple(const glm::mat4& modelMat, const glm::vec4& theColor) const {
    // TODO fix this
    mMVPShader_.bind();
    mMVPShader_.setMat4("uModel", modelMat);
    mMVPShader_.setVec4("uColor", theColor);
    mGraphicsAPI_.bindVertexArray(mRectVAO_);
    mGraphicsAPI_.drawArrays(IGraphicsAPI::PrimitiveTopology::LINE_LOOP, 0, 4);
    mGraphicsAPI_.bindVertexArray(0);
}

void Renderer::renderLineSimple(const glm::vec3& startPoint, const glm::vec3& endPoint, const glm::mat4& modelMat, const glm::vec4& theColor) const {
    // TODO fix this
    // Update vertices
    float vertices[] = {
        startPoint.x, startPoint.y, startPoint.z,
        endPoint.x, endPoint.y, endPoint.z
    };

    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, mLineVBO_);
    mGraphicsAPI_.bufferSubData(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    // draw line
    mMVPShader_.bind();
    mMVPShader_.setMat4("uModel", modelMat);
    mMVPShader_.setVec4("uColor", theColor);

    mGraphicsAPI_.bindVertexArray(mLineVAO_);
    mGraphicsAPI_.drawArrays(IGraphicsAPI::PrimitiveTopology::LINE_LIST, 0, 2);

    mGraphicsAPI_.bindVertexArray(0);
}

unsigned int Renderer::getHDRFBO() const {
    return hdrFBO_;
}

void Renderer::renderHDR() {
    mBlurShader_.bind();  // Use the shader to render the quad
    mBlurShader_.setInt("image", 0); // Texture unit 0
    mGraphicsAPI_.bindVertexArray(mFrameVAO_);

    // blur with ping pong
    bool horizontal = true, first_iteration = true;
    const int blurAmount = 10;
    mGraphicsAPI_.activeTexture(0);

    for (int i = 0; i < blurAmount; ++i) {
        mGraphicsAPI_.bindFrameBuffer(IGraphicsAPI::FrameBufferTarget::FRAMEBUFFER, pingpongFBO_[horizontal]);
        mBlurShader_.setInt("horizontal", horizontal);
        mGraphicsAPI_.bindTexture(
            IGraphicsAPI::TextureTarget::TEXTURE_2D,
            first_iteration ? colorBuffers_[1] : pingpongColorbuffers_[!horizontal]
        ); // bind texture of other framebuffer (or scene if first iteration)
        mGraphicsAPI_.drawElements(IGraphicsAPI::PrimitiveTopology::TRIANGLE_LIST, 6, IGraphicsAPI::DataType::UINT, 0);

        horizontal = !horizontal;
        if (first_iteration) {
            first_iteration = false;
        }
    }
    mGraphicsAPI_.bindFrameBuffer(IGraphicsAPI::FrameBufferTarget::FRAMEBUFFER, 0);
    // clear the default buffer
    mGraphicsAPI_.clearColor(0.0f, 0.0f, 0.0f, 1.0f);
    mGraphicsAPI_.clearBuffers({IGraphicsAPI::ClearBufferTarget::COLOR, IGraphicsAPI::ClearBufferTarget::DEPTH});
    bool bloom = true;

    mBloomFinalShader_.bind();
    mBloomFinalShader_.setInt("scene", 0);
    mBloomFinalShader_.setInt("bloomBlur", 1);
    mBloomFinalShader_.setInt("uGammaCorrect", mGammaCorrect_);

    mGraphicsAPI_.activeTexture(0);
    mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, colorBuffers_[0]);
    mGraphicsAPI_.activeTexture(1);
    mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, pingpongColorbuffers_[!horizontal]);
    mBloomFinalShader_.setInt("bloom", bloom);
    mBloomFinalShader_.setFloat("exposure", mExposure_);
    mGraphicsAPI_.drawElements(IGraphicsAPI::PrimitiveTopology::TRIANGLE_LIST, 6, IGraphicsAPI::DataType::UINT, 0);

    mGraphicsAPI_.bindFrameBuffer(IGraphicsAPI::FrameBufferTarget::FRAMEBUFFER, 0);
    mGraphicsAPI_.bindVertexArray(0);
}

void Renderer::setBloom(bool enable) {
    if (enable) {
        mGraphicsAPI_.bindFrameBuffer(IGraphicsAPI::FrameBufferTarget::FRAMEBUFFER, getHDRFBO());
        mGraphicsAPI_.drawBuffers(2, mAttachments_);
    } else {
        mGraphicsAPI_.bindFrameBuffer(IGraphicsAPI::FrameBufferTarget::FRAMEBUFFER, getHDRFBO());
        mGraphicsAPI_.drawBuffers(1, mAttachments_);
    }
}

void Renderer::setExposure(float newExposure) {
    mExposure_ = newExposure;
}

float Renderer::getExposure() const {
    return mExposure_;
}

void Renderer::enableGammaCorrect(bool enable) {
    mGammaCorrect_ = enable;
}

void Renderer::clearBuffers(const glm::vec4& color0, const glm::vec4 color1) {
    mGraphicsAPI_.bindFrameBuffer(IGraphicsAPI::FrameBufferTarget::FRAMEBUFFER, hdrFBO_);

    mGraphicsAPI_.drawBuffer(0);
    mGraphicsAPI_.clearColor(color0.r, color0.g, color0.b, color0.a);
    mGraphicsAPI_.clearBuffers({IGraphicsAPI::ClearBufferTarget::COLOR, IGraphicsAPI::ClearBufferTarget::DEPTH});

    mGraphicsAPI_.drawBuffer(1);
    mGraphicsAPI_.clearColor(color1.r, color1.g, color1.b, color1.a);
    mGraphicsAPI_.clearBuffers({IGraphicsAPI::ClearBufferTarget::COLOR, IGraphicsAPI::ClearBufferTarget::DEPTH});
}

void Renderer::enableWireFrame(bool enabled) const {
    if (enabled) {
        mGraphicsAPI_.polygonMode(IGraphicsAPI::PolygonModeFace::FRONT_AND_BACK, IGraphicsAPI::PolygonModeType::LINE);
    } else {
        mGraphicsAPI_.polygonMode(IGraphicsAPI::PolygonModeFace::FRONT_AND_BACK, IGraphicsAPI::PolygonModeType::FILL);
    }
}


} // namespace clay
// class
#include "clay/graphics/common/Renderer.h"

namespace clay {

const int Renderer::MAX_LIGHTS = 16;

Renderer::Renderer(const glm::vec2& screenDim, ShaderProgram& spriteShader, ShaderProgram& text2Shader,
                   ShaderProgram& mvpShader, Mesh& rectPlane, ShaderProgram& frameBufferShader, ShaderProgram& bloomFinalShader)
    : mSpriteShader_(spriteShader),
    mMVPShader_(mvpShader),
    mTextShader_(text2Shader),
    mRectPlane_(rectPlane),
    mBlurShader_(frameBufferShader),
    mBloomFinalShader_(bloomFinalShader) {
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

    glGenVertexArrays(1, &mRectVAO_);
    glGenBuffers(1, &VBORect);
    glGenBuffers(1, &EBORect);

    glBindVertexArray(mRectVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBORect);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRect), verticesRect, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBORect);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesRect), indicesRect, GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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

        glGenVertexArrays(1, &mFrameVAO_);
        glGenBuffers(1, &VBORect2);
        glGenBuffers(1, &EBORect2);

        glBindVertexArray(mFrameVAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBORect2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRect2), verticesRect2, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBORect2);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesRect2), indicesRect2, GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Texture coordinate attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    // Set up Line properties
    float lineVertices[] = {
        0,0,0,
        0,0,0,
    };

    glGenVertexArrays(1, &mLineVAO_);
    glGenBuffers(1, &mLineVBO_);

    glBindVertexArray(mLineVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, mLineVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);

    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Create Camera UBO to hold View and Projection matrix
    glGenBuffers(1, &mCameraUBO_);
    glBindBuffer(GL_UNIFORM_BUFFER, mCameraUBO_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_STATIC_DRAW);
    // define the range of the buffer that links to a uniform binding point
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, mCameraUBO_, 0, 2 * sizeof(glm::mat4));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Create Light UBO to hold light data
    glGenBuffers(1, &mLightUBO_);
    glBindBuffer(GL_UNIFORM_BUFFER, mLightUBO_);
    // Calculate the size of the light buffer data with padding
    size_t lightBufferSize = sizeof(glm::vec4)                    // numLights
                            + MAX_LIGHTS * sizeof(glm::vec4) // lightPositions
                            + MAX_LIGHTS * sizeof(glm::vec4); // lightColors

    glBufferData(GL_UNIFORM_BUFFER, lightBufferSize, NULL, GL_STATIC_DRAW);
    // Bind the buffer to the uniform binding point 1 (as an example)
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, mLightUBO_, 0, lightBufferSize);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    {
        // set up floating point framebuffer to render scene to
        glGenFramebuffers(1, &hdrFBO_);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO_);
        glGenTextures(2, colorBuffers_);
        for (unsigned int i = 0; i < 2; i++) {
            glBindTexture(GL_TEXTURE_2D, colorBuffers_[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenDim.x, screenDim.y, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            // attach texture to framebuffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers_[i], 0);
        }

        // create and attach depth buffer (renderbuffer)
        unsigned int rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenDim.x, screenDim.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
        glDrawBuffers(2, mAttachments_);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // ping-pong-framebuffer for blurring
        glGenFramebuffers(2, pingpongFBO_);
        glGenTextures(2, pingpongColorbuffers_);
        for (unsigned int i = 0; i < 2; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO_[i]);
            glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers_[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenDim.x, screenDim.y, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers_[i], 0);
        }

    }
    // by default, disable bloom
    setBloom(false);
}

Renderer::~Renderer() {}

void Renderer::setCamera(const Camera* camera) {
    glBindBuffer(GL_UNIFORM_BUFFER, mCameraUBO_);

    if (camera != nullptr) {
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->getViewMatrix()));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera->getProjectionMatrix()));
    } else {
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4(1)));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(mDefaultProjection_));
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Renderer::setLightSources(const std::vector<std::unique_ptr<LightSource>>& lights) const {
    glBindBuffer(GL_UNIFORM_BUFFER, mLightUBO_);

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
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), &temp); // numLights

    // Set padding
    size_t offset = sizeof(glm::vec4); // Offset for padding

    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec4) * MAX_LIGHTS, lightPositions.data()); // lightPositions

    // Set light colors
    offset += sizeof(glm::vec4) * MAX_LIGHTS;
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec4) * MAX_LIGHTS, lightColors.data()); // lightColors

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Renderer::setLightSources(const std::vector<LightSource*>& lights) const {
        glBindBuffer(GL_UNIFORM_BUFFER, mLightUBO_);

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
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), &temp); // numLights

    // Set padding
    size_t offset = sizeof(glm::vec4); // Offset for padding

    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec4) * MAX_LIGHTS, lightPositions.data()); // lightPositions

    // Set light colors
    offset += sizeof(glm::vec4) * MAX_LIGHTS;
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec4) * MAX_LIGHTS, lightColors.data()); // lightColors

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Renderer::renderSprite(unsigned int textureId, const glm::mat4& modelMat, const glm::vec4& theColor) const {
    mSpriteShader_.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, theSprite.parentSpriteSheet.getTextureId());

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
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(font.getVAO());

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
            glBindTexture(GL_TEXTURE_2D, ch->textureId);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, font.getVBO());
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // TODO set only once?
            mTextShader_.setMat4("uModel", glm::mat4(1));
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
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
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(font.getVAO());

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
            glBindTexture(GL_TEXTURE_2D, ch->textureId);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, font.getVBO());
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            mTextShader_.setMat4("uModel", glm::mat4(1));
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            startX += (ch->advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::renderTextNormalized(const std::string& text, const glm::mat4& modelMat, const Font& font, const glm::vec3& scale, const glm::vec3& color) {
    // activate corresponding render state	
    mTextShader_.bind();
    mTextShader_.setVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(font.getVAO());

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
            glBindTexture(GL_TEXTURE_2D, ch->textureId);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, font.getVBO());
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // Apply the model matrix to the shader
            mTextShader_.setMat4("uModel", modelMat);

            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            startX += (ch->advance >> 6) * scale.x; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::renderRectangleSimple(const glm::mat4& modelMat, const glm::vec4& theColor) const {
    // TODO fix this
    mMVPShader_.bind();
    mMVPShader_.setMat4("uModel", modelMat);
    mMVPShader_.setVec4("uColor", theColor);

    glBindVertexArray(mRectVAO_);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);
}

void Renderer::renderLineSimple(const glm::vec3& startPoint, const glm::vec3& endPoint, const glm::mat4& modelMat, const glm::vec4& theColor) const {
    // TODO fix this
    // Update vertices
    float vertices[] = {
        startPoint.x, startPoint.y, startPoint.z,
        endPoint.x, endPoint.y, endPoint.z
    };

    glBindBuffer(GL_ARRAY_BUFFER, mLineVBO_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    // draw line
    mMVPShader_.bind();
    mMVPShader_.setMat4("uModel", modelMat);
    mMVPShader_.setVec4("uColor", theColor);

    glBindVertexArray(mLineVAO_);
    glDrawArrays(GL_LINES, 0, 2); // Draw a line using the two vertices

    glBindVertexArray(0);
}

GLuint Renderer::getHDRFBO() const {
    return hdrFBO_;
}

void Renderer::renderHDR() {
    mBlurShader_.bind();  // Use the shader to render the quad
    mBlurShader_.setInt("image", 0); // Texture unit 0
    glBindVertexArray(mFrameVAO_);

    // blur with ping pong
    bool horizontal = true, first_iteration = true;
    const int blurAmount = 10;
    glActiveTexture(GL_TEXTURE0);

    for (int i = 0; i < blurAmount; ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO_[horizontal]);
        mBlurShader_.setInt("horizontal", horizontal);
        glBindTexture(
            GL_TEXTURE_2D,
            first_iteration ? colorBuffers_[1] : pingpongColorbuffers_[!horizontal]
        );  // bind texture of other framebuffer (or scene if first iteration)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // clear the default buffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    bool bloom = true;

    mBloomFinalShader_.bind();
    mBloomFinalShader_.setInt("scene", 0);
    mBloomFinalShader_.setInt("bloomBlur", 1);
    mBloomFinalShader_.setInt("uGammaCorrect", mGammaCorrect_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers_[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers_[!horizontal]);
    mBloomFinalShader_.setInt("bloom", bloom);
    mBloomFinalShader_.setFloat("exposure", mExposure_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::setBloom(bool enable) {
    if (enable) {
        glBindFramebuffer(GL_FRAMEBUFFER, getHDRFBO());
        glDrawBuffers(2, mAttachments_);
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, getHDRFBO());
        glDrawBuffers(1, mAttachments_);
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
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO_);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glClearColor(color0.r, color0.g, color0.b, color0.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawBuffer(GL_COLOR_ATTACHMENT1);
    glClearColor(color1.r, color1.g, color1.b, color1.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

} // namespace clay
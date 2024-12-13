// class
#include "Clay/Application/App.h"

namespace clay {

bool App::sOpenGLInitialized_ = false;

App::App()
 : mWindow_("OpenGL Application", 800, 600) {
    // Initialize OpenGL and imgui
    initializeOpenGL();
    ImGuiComponent::initializeImGui(mWindow_.getGLFWWindow());
    // Load/build Application resources
    loadResources();
    // Renderer and Scene (must be called after OpenGL is initialized)
    mpRenderer_ = std::make_unique<Renderer>(
        glm::vec2{800, 600},
        *(mResources_.getResource<Shader>("TextureSurface")),
        *(mResources_.getResource<Shader>("Text")),
        *(mResources_.getResource<Shader>("MVPShader")),
        *(mResources_.getResource<Mesh>("RectPlane")),
        *(mResources_.getResource<Shader>("Blur")),
        *(mResources_.getResource<Shader>("BloomFinal"))
    );
}

App::~App() {
    // Clean Application resources
    ImGuiComponent::deinitialize();
    glfwTerminate();
    // Play "0" audio to clear audio buffer
    mAudioManger_.playSound(0);
}

void App::run() {
    mWindow_.setWindowDisplay(true);
    // Update and render while application is running
    while (isRunning()) {
        update();
        render();
    }
}

void App::update() {
    // Calculate time since last update (in seconds)
    std::chrono::duration<float> dt = (std::chrono::high_resolution_clock::now() - mLastTime_);
    mLastTime_ = std::chrono::high_resolution_clock::now();
    // Update application content
    mWindow_.update(dt.count());
    // Update list in reverse order and delete any marked for removal
    for (auto it = mScenes_.rbegin(); it != mScenes_.rend();) {
        if ((*it)->isRemove()) {
            // Erase and update the iterator
            it = decltype(it)(mScenes_.erase(std::next(it).base()));
        } else {
            (*it)->update(dt.count());
            ++it;
        }
    }
    InputHandler& handler = getWindow().getInputHandler();

    // Propagate key events to the scenes
    while (const auto keyEvent = handler.getKeyEvent()) {
        for (auto it = mScenes_.rbegin(); it != mScenes_.rend(); ++it) {
            if (keyEvent.value().getType() == InputHandler::KeyEvent::Type::PRESS) {
                (*it)->onKeyPress(keyEvent.value().getCode());
            } else if (keyEvent.value().getType() == InputHandler::KeyEvent::Type::RELEASE) {
                (*it)->onKeyRelease(keyEvent.value().getCode());
            }
        }
    }

    // Propagate mouse events to scenes
    while (const auto mouseEvent = handler.getMouseEvent()) {
        for (auto it = mScenes_.rbegin(); it != mScenes_.rend(); ++it) {
            if (mouseEvent.value().getType() == InputHandler::MouseEvent::Type::PRESS) {
                (*it)->onMousePress(mouseEvent.value());
            } else if (mouseEvent.value().getType() == InputHandler::MouseEvent::Type::RELEASE) {
                (*it)->onMouseRelease(mouseEvent.value());
            } else if (mouseEvent.value().getType() == InputHandler::MouseEvent::Type::SCROLL_UP ||
                mouseEvent.value().getType() == InputHandler::MouseEvent::Type::SCROLL_DOWN) {
                (*it)->onMouseWheel(mouseEvent.value());
            }
        }
    }
}

void App::render() {
    // Set background color from scene
    if (!mScenes_.empty()) {
        glm::vec4 sceneBackgroundColor = mScenes_.front()->getBackgroundColor();
        glClearColor(sceneBackgroundColor.r, sceneBackgroundColor.g, sceneBackgroundColor.b, sceneBackgroundColor.a);
        mpRenderer_->clearBuffers(
            {sceneBackgroundColor.r, sceneBackgroundColor.g, sceneBackgroundColor.b, sceneBackgroundColor.a},
            {0,0,0,1}
        );
    } else {
        mpRenderer_->clearBuffers(
            {0,0,0,1},
            {0,0,0,1}
        );
    }
    glBindFramebuffer(GL_FRAMEBUFFER, mpRenderer_->getHDRFBO());
    mpRenderer_->setBloom(false);
    glEnable(GL_FRAMEBUFFER_SRGB); // for gamma correction
    // Render list in reverse order
    for (auto it = mScenes_.rbegin(); it != mScenes_.rend(); ++it) {
       (*it)->render(*mpRenderer_);
    }
    mpRenderer_->renderHDR();

    // render guis on top (avoid gamma correction)
    glDisable(GL_FRAMEBUFFER_SRGB); // for gamma correction
    for (auto it = mScenes_.rbegin(); it != mScenes_.rend(); ++it) {
       (*it)->renderGUI();
    }

    mWindow_.render();
}

bool App::isRunning() const {
    // TODO more running conditions. For now running is when the window is open
    return mWindow_.isRunning();
}

void App::quit() {
    // TODO do any saving
    // Set window to close
    glfwSetWindowShouldClose(mWindow_.getGLFWWindow(), true);
}

void App::setScene(Scene* newScene) {
    mScenes_.push_back(std::unique_ptr<Scene>(newScene));
}

Window& App::getWindow() {
    return mWindow_;
}

void App::setAntiAliasing(unsigned int sampleSize) {
    // Set sample size
    glfwWindowHint(GLFW_SAMPLES, sampleSize);
    // Enable/disable anti-aliasing based on sample size
    if (sampleSize != 0) {
        glEnable(GL_MULTISAMPLE);
    } else {
        glDisable(GL_MULTISAMPLE);
    }
}

void App::loadResources() {
    // Shaders
    mResources_.loadResource<Shader>(
        {
        Resource::RESOURCE_PATH / "Shaders/AssimpLight.vert",
        Resource::RESOURCE_PATH / "Shaders/AssimpLight.frag"
        },
        "AssimpLight"
    );
    mResources_.loadResource<Shader>(
        {
        Resource::RESOURCE_PATH / "Shaders/Assimp.vert",
        Resource::RESOURCE_PATH / "Shaders/Assimp.frag"
        },
        "Assimp"
    );
    mResources_.loadResource<Shader>(
        {
        Resource::RESOURCE_PATH / "Shaders/MVPTexShader.vert",
        Resource::RESOURCE_PATH / "Shaders/MVPTexShader.frag"
        },
        "MVPTexShader"
    );
    mResources_.loadResource<Shader>(
        {
        Resource::RESOURCE_PATH / "Shaders/Text.vert",
        Resource::RESOURCE_PATH / "Shaders/Text.frag"
        },
        "Text"
    );
    mResources_.loadResource<Shader>(
        {
        Resource::RESOURCE_PATH / "Shaders/MVPShader.vert",
        Resource::RESOURCE_PATH / "Shaders/MVPShader.frag"
        },
        "MVPShader"
    );
    mResources_.loadResource<Shader>(
        {
        Resource::RESOURCE_PATH / "Shaders/TextureSurface.vert",
        Resource::RESOURCE_PATH / "Shaders/TextureSurface.frag"
        },
         "TextureSurface"
    );
    mResources_.loadResource<Shader>(
        {
        Resource::RESOURCE_PATH / "Shaders/Blur.vert",
        Resource::RESOURCE_PATH / "Shaders/Blur.frag"
        },
        "Blur"
    );
    mResources_.loadResource<Shader>(
        {
        Resource::RESOURCE_PATH / "Shaders/BloomFinal.vert",
        Resource::RESOURCE_PATH / "Shaders/BloomFinal.frag"
        },
        "BloomFinal"
    );
    // Textures
    mResources_.loadResource<Texture>(
        {Resource::RESOURCE_PATH / "Sprites.png"},
         "SpriteSheet"
    );
    mResources_.loadResource<Texture>(
        {Resource::RESOURCE_PATH / "V.png"},
         "SampleTexture"
    );
    // Single white pixel
    std::vector<unsigned char> whitePixel{0xFF, 0xFF, 0xFF};
    mResources_.addResource(std::move(std::make_unique<Texture>(whitePixel.data(), 1, 1, 3)), "Blank");

    // Mesh
    mResources_.addResource(
        std::move(std::make_unique<Mesh>(
            std::vector<Mesh::Vertex>{
                // Position, normal, UV, tangent, bitangent
                // Back
                {{-.5f, -.5f, -.50f}, {0.0f,  0.0f, -1.0f},  {0.0f, 0.0f},{0,0,0},{0,0,0}},
                {{.5f,  .5f, -.5f}, {0.0f,  0.0f, -1.0f},  {1.0f, 1.0f},{0,0,0},{0,0,0}},
                {{.5f, -.5f, -.5f}, {0.0f,  0.0f, -1.0f},  {1.0f, 0.0f},{0,0,0},{0,0,0}},
                {{.5f,  .5f, -.5f}, {0.0f,  0.0f, -1.0f},  {1.0f, 1.0f},{0,0,0},{0,0,0}},
                {{-.5f, -.5f, -.5f}, {0.0f,  0.0f, -1.0f},  {0.0f, 0.0f},{0,0,0},{0,0,0}},
                {{-.5f,  .5f, -.5f}, {0.0f,  0.0f, -1.0f},  {0.0f, 1.0f},{0,0,0},{0,0,0}},
                // Front
                {{-.5f, -.5f,  .5f},  {0.0f,  0.0f,  1.0f},  {0.0f, 0.0f},{0,0,0},{0,0,0}},
                {{.5f, -.5f,  .5f},  {0.0f,  0.0f,  1.0f},  {1.0f, 0.0f},{0,0,0},{0,0,0}},
                {{.5f,  .5f,  .5f},  {0.0f,  0.0f,  1.0f},  {1.0f, 1.0f},{0,0,0},{0,0,0}},
                {{.5f,  .5f,  .5f},  {0.0f,  0.0f,  1.0f},  {1.0f, 1.0f},{0,0,0},{0,0,0}},
                {{-.5f,  .5f,  .5f},  {0.0f,  0.0f,  1.0f},  {0.0f, 1.0f},{0,0,0},{0,0,0}},
                {{-.5f, -.5f,  .5f},  {0.0f,  0.0f,  1.0f},  {0.0f, 0.0f},{0,0,0},{0,0,0}},
                // Left
                {{-.5f,  .5f,  .5f}, {-1.0f,  0.0f,  0.0f},  {1.0f, 1.0f},{0,0,0},{0,0,0}},
                {{-.5f,  .5f, -.5f}, {-1.0f,  0.0f,  0.0f},  {0.0f, 1.0f},{0,0,0},{0,0,0}},
                {{-.5f, -.5f, -.5f}, {-1.0f,  0.0f,  0.0f},  {0.0f, 0.0f},{0,0,0},{0,0,0}},
                {{-.5f, -.5f, -.5f}, {-1.0f,  0.0f,  0.0f},  {0.0f, 0.0f},{0,0,0},{0,0,0}},
                {{-.5f, -.5f,  .5f}, {-1.0f,  0.0f,  0.0f},  {1.0f, 0.0f},{0,0,0},{0,0,0}},
                {{-.5f,  .5f,  .5f}, {-1.0f,  0.0f,  0.0f},  {1.0f, 1.0f},{0,0,0},{0,0,0}},
                // Right
                {{.5f,  .5f,  .5f},  {1.0f,  0.0f,  0.0f},  {0.0f, 1.0f},{0,0,0},{0,0,0}},
                {{.5f, -.5f, -.5f},  {1.0f,  0.0f,  0.0f},  {1.0f, 0.0f},{0,0,0},{0,0,0}},
                {{.5f,  .5f, -.5f},  {1.0f,  0.0f,  0.0f},  {1.0f, 1.0f},{0,0,0},{0,0,0}},
                {{.5f, -.5f, -.5f},  {1.0f,  0.0f,  0.0f},  {1.0f, 0.0f},{0,0,0},{0,0,0}},
                {{.5f,  .5f,  .5f},  {1.0f,  0.0f,  0.0f},  {0.0f, 1.0f},{0,0,0},{0,0,0}},
                {{.5f, -.5f,  .5f},  {1.0f,  0.0f,  0.0f},  {0.0f, 0.0f},{0,0,0},{0,0,0}},
                // Bottom
                {{-.5f, -.5f, -.5f},  {0.0f, -1.0f,  0.0f},  {0.0f, 1.0f},{0,0,0},{0,0,0}},
                {{.5f, -.5f, -.5f},  {0.0f, -1.0f,  0.0f},  {1.0f, 1.0f},{0,0,0},{0,0,0}},
                {{.5f, -.5f,  .5f},  {0.0f, -1.0f,  0.0f},  {1.0f, 0.0f},{0,0,0},{0,0,0}},
                {{.5f, -.5f,  .5f},  {0.0f, -1.0f,  0.0f},  {1.0f, 0.0f},{0,0,0},{0,0,0}},
                {{-.5f, -.5f,  .5f},  {0.0f, -1.0f,  0.0f},  {0.0f, 0.0f},{0,0,0},{0,0,0}},
                {{-.5f, -.5f, -.5f},  {0.0f, -1.0f,  0.0f},  {0.0f, 1.0f},{0,0,0},{0,0,0}},
                // Top
                {{-.5f,  .5f, -.5f},  {0.0f,  1.0f,  0.0f},  {0.0f, 1.0f},{0,0,0},{0,0,0}},
                {{.5f,  .5f,  .5f},  {0.0f,  1.0f,  0.0f},  {1.0f, 0.0f},{0,0,0},{0,0,0}},
                {{.5f,  .5f, -.5f},  {0.0f,  1.0f,  0.0f},  {1.0f, 1.0f},{0,0,0},{0,0,0}},
                {{.5f,  .5f,  .5f},  {0.0f,  1.0f,  0.0f},  {1.0f, 0.0f},{0,0,0},{0,0,0}},
                {{-.5f,  .5f, -.5f},  {0.0f,  1.0f,  0.0f},  {0.0f, 1.0f},{0,0,0},{0,0,0}},
                {{-.5f,  .5f,  .5f},  {0.0f,  1.0f,  0.0f},  {0.0f, 0.0f},{0,0,0},{0,0,0}},
            },
            std::vector<unsigned int>{
                0,1,2,
                3,4,5,
                6,7,8,
                9,10,11,
                12,13,14,
                15,16,17,
                18,19,20,
                21,22,23,
                24,25,26,
                27,28,29,
                30,31,32,
                33,34,35
            }
        )),
        "Cube"
    );
    mResources_.addResource(
        std::move(std::make_unique<Mesh>(
            std::vector<Mesh::Vertex>{
                // Position, normal, UV, tangent, bitangent
                {{-.5f, -.5f, 0.0f}, {0.0f,  0.0f, -1.0f},  {0.0f,  0.0f},{0,0,0},{0,0,0}},
                {{.5f, -.5f, 0.0f}, {0.0f,  0.0f, -1.0f},  {1.0f,  0.0f},{0,0,0},{0,0,0}},
                {{.5f,  .5f, 0.0f}, {0.0f,  0.0f, -1.0f},  {1.0f,  1.0f},{0,0,0},{0,0,0}},
                {{.5f,  .5f, 0.0f}, {0.0f,  0.0f, -1.0f},  {1.0f,  1.0f},{0,0,0},{0,0,0}},
                {{-.5f,  .5f, 0.0f}, {0.0f,  0.0f, -1.0f},  {0.0f,  1.0f},{0,0,0},{0,0,0}},
                {{-.5f, -.5f, 0.0f}, {0.0f,  0.0f, -1.0f},  {0.0f,  0.0f},{0,0,0},{0,0,0}},
            },
            std::vector<unsigned int>{
                0,1,2,
                3,4,5,
            }
        )),
        "RectPlane"
    );
    mResources_.loadResource<Mesh>(
        {Resource::RESOURCE_PATH / "Sphere.obj"},
        "Sphere"
    );
    // Circle plane
    {
        int segments = 16; // Edges on Circle
        std::vector<Mesh::Vertex> vertices;
        std::vector<unsigned int> indices;

        float radius = .5f;
        float x, y, z;
        for (int i = 0; i < segments; ++i) {
            float theta = 2.0f * std::numbers::pi_v<float> * float(i) / float(segments);
            x = radius * cosf(theta);
            y = radius * sinf(theta);
            z = 0.0f;

            // UV coordinates
            float u = 0.5f * (x / radius + 1.0f);
            float v = 0.5f * (y / radius + 1.0f);

            // normal is always (0, 0, 1) for a plane
            vertices.push_back({{x, y, z}, {0.0f, 0.0f, 1.0f}, {u, v}, {0, 0, 0}, {0, 0, 0}});
        }

        // Triangulating the circle
        for (int i = 0; i < segments - 2; ++i) {
            indices.push_back(0);
            indices.push_back(i + 1);
            indices.push_back(i + 2);
        }

        // Closing the circle
        indices.push_back(0);
        indices.push_back(segments - 1);
        indices.push_back(1);

        mResources_.addResource(
            std::move(std::make_unique<Mesh>(
                vertices,
                indices
            )),
            "CircularPlane"
        );
    }
    // Audio
    mResources_.loadResource<Audio>({Resource::RESOURCE_PATH / "audio/beep_deep_1.wav"}, "Blip_Deep");
    mResources_.loadResource<Audio>({Resource::RESOURCE_PATH / "audio/Blip_1.wav"}, "Blip1");
    mResources_.loadResource<Audio>({Resource::RESOURCE_PATH / "audio/button_click_1.wav"}, "Button_click");
    mResources_.loadResource<Audio>({Resource::RESOURCE_PATH / "audio/PatakasWorld.wav"}, "PatakasWorld");
    // Fonts
    mResources_.loadResource<Font>({Resource::RESOURCE_PATH / "fonts/Consolas.ttf"}, "Consolas");

    // SpriteSheet
    mResources_.addResource(
        std::move(std::make_unique<SpriteSheet>(
            *mResources_.getResource<Texture>("SpriteSheet"),
            glm::ivec2{16,16}
        )),
        "SpriteSheet1"
    );
}

void App::initializeOpenGL() {
    // Only initialize if this if the first time
    if (!sOpenGLInitialized_) {
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

        sOpenGLInitialized_ = true;
    }
}

AudioManager& App::getAudioManger() {
    return mAudioManger_;
}

Resource& App::getResources() {
    return mResources_;
}

Renderer& App::getRenderer() {
    return *mpRenderer_.get();
}

} // namespace clay
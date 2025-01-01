// standard lib
#include <numbers>
#include <fstream>
// third party
// project
#include "clay/graphics/opengl/GraphicsAPIOpenGL.h"
#include "clay/application/Logger.h"
// class
#include "clay/application/App.h"
#include "clay/gui/ImGuiComponent.h" // include after app

namespace clay {

bool App::sOpenGLInitialized_ = false;

App::App()
 : mWindow_("OpenGL Application", 800, 600) {
    // Initialize OpenGL and imgui
    initializeOpenGL(); // remove this?
    mGraphicsAPI_ = new GraphicsAPIOpenGL();
    mResources_.mGraphicsAPI_ = mGraphicsAPI_;
    ImGuiComponent::initializeImGui(mWindow_.getGLFWWindow());
    // Load/build Application resources
    loadResources();
    // Renderer and Scene (must be called after OpenGL is initialized)
    mpRenderer_ = std::make_unique<Renderer>(
        glm::vec2{800, 600},
        *(mResources_.getResource<ShaderProgram>("TextureSurface")),
        *(mResources_.getResource<ShaderProgram>("Text")),
        *(mResources_.getResource<ShaderProgram>("MVPShader")),
        *(mResources_.getResource<Mesh>("RectPlane")),
        *(mResources_.getResource<ShaderProgram>("Blur")),
        *(mResources_.getResource<ShaderProgram>("BloomFinal")),
        *mGraphicsAPI_
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
    std::chrono::duration<float> dt = (std::chrono::steady_clock::now() - mLastTime_);
    mLastTime_ = std::chrono::steady_clock::now();
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
        mGraphicsAPI_->clearColor(sceneBackgroundColor.r, sceneBackgroundColor.g, sceneBackgroundColor.b, sceneBackgroundColor.a);
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
    mGraphicsAPI_->bindFrameBuffer(IGraphicsAPI::FrameBufferTarget::FRAMEBUFFER, mpRenderer_->getHDRFBO());
    mpRenderer_->setBloom(false);
    mGraphicsAPI_->enable(IGraphicsAPI::Capability::FRAMEBUFFER_SRGB);
    //glEnable(GL_FRAMEBUFFER_SRGB); // for gamma correction
    // Render list in reverse order
    for (auto it = mScenes_.rbegin(); it != mScenes_.rend(); ++it) {
       (*it)->render(*mpRenderer_);
    }
    mpRenderer_->renderHDR();

    // render guis on top (avoid gamma correction)
    mGraphicsAPI_->disable(IGraphicsAPI::Capability::FRAMEBUFFER_SRGB);
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
        mGraphicsAPI_->enable(IGraphicsAPI::Capability::MULTISAMPLE);
    } else {
        mGraphicsAPI_->disable(IGraphicsAPI::Capability::MULTISAMPLE);
    }
}

void App::loadResources() {
    // Shaders
    {
        auto vertexShaderFileData = Resource::loadFileToMemory((Resource::RESOURCE_PATH / "shaders/AssimpLight.vert").string());
        auto fragmentShaderFileData = Resource::loadFileToMemory((Resource::RESOURCE_PATH / "shaders/AssimpLight.frag").string());

        std::unique_ptr<ShaderProgram> shader = std::make_unique<ShaderProgram>(*mGraphicsAPI_);
        shader->addShader({
            ShaderCreateInfo::Type::VERTEX,
            std::string(reinterpret_cast<char*>(vertexShaderFileData.data.get()), vertexShaderFileData.size).c_str(),
            vertexShaderFileData.size
        });
        shader->addShader({
            ShaderCreateInfo::Type::FRAGMENT,
            std::string(reinterpret_cast<char*>(fragmentShaderFileData.data.get()), fragmentShaderFileData.size).c_str(),
        });

        shader->linkProgram();
        // add to resource
        mResources_.addResource<ShaderProgram>(std::move(shader), "AssimpLight");
    }
    {
        auto vertexShaderFileData = Resource::loadFileToMemory((Resource::RESOURCE_PATH / "shaders/Assimp.vert").string());
        auto fragmentShaderFileData = Resource::loadFileToMemory((Resource::RESOURCE_PATH / "shaders/Assimp.frag").string());
        // TODO use size so null string conversion for null terminator is not needed
        std::unique_ptr<ShaderProgram> shader = std::make_unique<ShaderProgram>(*mGraphicsAPI_);
        shader->addShader({
            ShaderCreateInfo::Type::VERTEX,
            std::string(reinterpret_cast<char*>(vertexShaderFileData.data.get()), vertexShaderFileData.size).c_str(),
            vertexShaderFileData.size
        });
        shader->addShader({
            ShaderCreateInfo::Type::FRAGMENT,
            std::string(reinterpret_cast<char*>(fragmentShaderFileData.data.get()), fragmentShaderFileData.size).c_str(),
            fragmentShaderFileData.size
        });

        shader->linkProgram();
        // add to resource
        mResources_.addResource<ShaderProgram>(std::move(shader), "Assimp");
    }
    {
        auto vertexShaderFileData = Resource::loadFileToMemory((Resource::RESOURCE_PATH / "shaders/MVPTexShader.vert").string());
        auto fragmentShaderFileData = Resource::loadFileToMemory((Resource::RESOURCE_PATH / "shaders/MVPTexShader.frag").string());
        // TODO use size so null string conversion for null terminator is not needed
        std::unique_ptr<ShaderProgram> shader = std::make_unique<ShaderProgram>(*mGraphicsAPI_);
        shader->addShader({
            ShaderCreateInfo::Type::VERTEX,
            std::string(reinterpret_cast<char*>(vertexShaderFileData.data.get()), vertexShaderFileData.size).c_str(),
            vertexShaderFileData.size
        });
        shader->addShader({
            ShaderCreateInfo::Type::FRAGMENT,
            std::string(reinterpret_cast<char*>(fragmentShaderFileData.data.get()), fragmentShaderFileData.size).c_str(),
            fragmentShaderFileData.size
        });

        shader->linkProgram();
        // add to resource
        mResources_.addResource<ShaderProgram>(std::move(shader), "MVPTexShader");
    }
    {
        auto vertexShaderFileData = Resource::loadFileToMemory((Resource::RESOURCE_PATH / "shaders/Text.vert").string());
        auto fragmentShaderFileData = Resource::loadFileToMemory((Resource::RESOURCE_PATH / "shaders/Text.frag").string());
        // TODO use size so null string conversion for null terminator is not needed
        std::unique_ptr<ShaderProgram> shader = std::make_unique<ShaderProgram>(*mGraphicsAPI_);
        shader->addShader({
            ShaderCreateInfo::Type::VERTEX,
            std::string(reinterpret_cast<char*>(vertexShaderFileData.data.get()), vertexShaderFileData.size).c_str(),
            vertexShaderFileData.size
        });
        shader->addShader({
            ShaderCreateInfo::Type::FRAGMENT,
            std::string(reinterpret_cast<char*>(fragmentShaderFileData.data.get()), fragmentShaderFileData.size).c_str(),
            fragmentShaderFileData.size
        });

        shader->linkProgram();
        // add to resource
        mResources_.addResource<ShaderProgram>(std::move(shader), "Text");
    }
    {
        auto vertexShaderFileData = Resource::loadFileToMemory((Resource::RESOURCE_PATH / "shaders/MVPShader.vert").string());
        auto fragmentShaderFileData = Resource::loadFileToMemory((Resource::RESOURCE_PATH / "shaders/MVPShader.frag").string());
        // TODO use size so null string conversion for null terminator is not needed
        std::unique_ptr<ShaderProgram> shader = std::make_unique<ShaderProgram>(*mGraphicsAPI_);
        shader->addShader({
            ShaderCreateInfo::Type::VERTEX,
            std::string(reinterpret_cast<char*>(vertexShaderFileData.data.get()), vertexShaderFileData.size).c_str(),
            vertexShaderFileData.size
        });
        shader->addShader({
            ShaderCreateInfo::Type::FRAGMENT,
            std::string(reinterpret_cast<char*>(fragmentShaderFileData.data.get()), fragmentShaderFileData.size).c_str(),
            fragmentShaderFileData.size
        });

        shader->linkProgram();
        // add to resource
        mResources_.addResource<ShaderProgram>(std::move(shader), "MVPShader");
    }
    {
        auto vertexShaderFileData = Resource::loadFileToMemory((Resource::RESOURCE_PATH / "shaders/TextureSurface.vert").string());
        auto fragmentShaderFileData = Resource::loadFileToMemory((Resource::RESOURCE_PATH / "shaders/TextureSurface.frag").string());
        // TODO use size so null string conversion for null terminator is not needed
        std::unique_ptr<ShaderProgram> shader = std::make_unique<ShaderProgram>(*mGraphicsAPI_);
        shader->addShader({
            ShaderCreateInfo::Type::VERTEX,
            std::string(reinterpret_cast<char*>(vertexShaderFileData.data.get()), vertexShaderFileData.size).c_str(),
            vertexShaderFileData.size
        });
        shader->addShader({
            ShaderCreateInfo::Type::FRAGMENT,
            std::string(reinterpret_cast<char*>(fragmentShaderFileData.data.get()), fragmentShaderFileData.size).c_str(),
            fragmentShaderFileData.size
        });

        shader->linkProgram();
        // add to resource
        mResources_.addResource<ShaderProgram>(std::move(shader), "TextureSurface");
    }
    {
        auto vertexShaderFileData = Resource::loadFileToMemory((Resource::RESOURCE_PATH / "shaders/Blur.vert").string());
        auto fragmentShaderFileData = Resource::loadFileToMemory((Resource::RESOURCE_PATH / "shaders/Blur.frag").string());
        // TODO use size so null string conversion for null terminator is not needed
        std::unique_ptr<ShaderProgram> shader = std::make_unique<ShaderProgram>(*mGraphicsAPI_);
        shader->addShader({
            ShaderCreateInfo::Type::VERTEX,
            std::string(reinterpret_cast<char*>(vertexShaderFileData.data.get()), vertexShaderFileData.size).c_str(),
            vertexShaderFileData.size
        });
        shader->addShader({
            ShaderCreateInfo::Type::FRAGMENT,
            std::string(reinterpret_cast<char*>(fragmentShaderFileData.data.get()), fragmentShaderFileData.size).c_str(),
            fragmentShaderFileData.size
        });

        shader->linkProgram();
        // add to resource
        mResources_.addResource<ShaderProgram>(std::move(shader), "Blur");
    }
    {
        auto vertexShaderFileData = Resource::loadFileToMemory((Resource::RESOURCE_PATH / "shaders/BloomFinal.vert").string());
        auto fragmentShaderFileData = Resource::loadFileToMemory((Resource::RESOURCE_PATH / "shaders/BloomFinal.frag").string());
        // TODO use size so null string conversion for null terminator is not needed
        std::unique_ptr<ShaderProgram> shader = std::make_unique<ShaderProgram>(*mGraphicsAPI_);
        shader->addShader({
            ShaderCreateInfo::Type::VERTEX,
            std::string(reinterpret_cast<char*>(vertexShaderFileData.data.get()), vertexShaderFileData.size).c_str(),
            vertexShaderFileData.size
        });
        shader->addShader({
            ShaderCreateInfo::Type::FRAGMENT,
            std::string(reinterpret_cast<char*>(fragmentShaderFileData.data.get()), fragmentShaderFileData.size).c_str(),
            fragmentShaderFileData.size
        });

        shader->linkProgram();
        // add to resource
        mResources_.addResource<ShaderProgram>(std::move(shader), "BloomFinal");
    }
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
    mResources_.addResource(std::move(std::make_unique<Texture>(*mGraphicsAPI_, whitePixel.data(), 1, 1, 3)), "Blank");

    // Mesh
    mResources_.addResource(
        std::move(std::make_unique<Mesh>(
            *mGraphicsAPI_,
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
        "Cube" // TODO REPLACE WITH LOADING Cube.obj
    );
    mResources_.addResource(
        std::move(std::make_unique<Mesh>(
            *mGraphicsAPI_,
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
                *mGraphicsAPI_,
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

IGraphicsAPI& App::getGraphicsAPI() {
    return *mGraphicsAPI_;
}

} // namespace clay
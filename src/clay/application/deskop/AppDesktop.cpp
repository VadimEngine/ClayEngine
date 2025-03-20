#ifdef CLAY_PLATFORM_DESKTOP
// standard lib
// #include <numbers>
#include <fstream>
// third party
// project
#include "clay/graphics/opengl/GraphicsAPIOpenGL.h"
#include "clay/utils/common/Logger.h"
#include "clay/utils/desktop/UtilsDesktop.h"
// class
#include "clay/application/desktop/AppDesktop.h"
#include "clay/gui/desktop/ImGuiComponent.h" // include after app

namespace clay {

bool AppDesktop::sOpenGLInitialized_ = false;

AppDesktop::AppDesktop() {}

AppDesktop::~AppDesktop() {
    // Clean Application resources
    ImGuiComponent::deinitialize();
    mResources_.releaseAll();
    glfwTerminate();
    // Play "0" audio to clear audio buffer
    mAudioManager_.playSound(0);
}

void AppDesktop::initialize() {
    // Initialize OpenGL and imgui
    initializeOpenGL(); // remove this?
    mGraphicsAPI_ = new GraphicsAPIOpenGL();
    mResources_.mGraphicsAPI_ = mGraphicsAPI_;
    ImGuiComponent::initializeImGui(((WindowDesktop*)mpWindow_.get())->getGLFWWindow());
    // Load/build Application resources
    loadResources();
    // Renderer and Scene (must be called after OpenGL is initialized)
    mpRenderer_ = std::make_unique<RendererOpenGL>(
        mpWindow_->getDimensions(),
        *(mResources_.getResource<ShaderProgram>("TextureSurface")),
        *(mResources_.getResource<ShaderProgram>("Text")),
        *(mResources_.getResource<ShaderProgram>("MVPShader")),
        *(mResources_.getResource<Mesh>("RectPlane")),
        *(mResources_.getResource<ShaderProgram>("Blur")),
        *(mResources_.getResource<ShaderProgram>("BloomFinal")),
        *mGraphicsAPI_
    );
}

void AppDesktop::run() {
    mpWindow_->enableDisplay(true);
    // Update and render while application is running
    while (isRunning()) {
        update();
        render();
    }
}

void AppDesktop::update() {
    // Calculate time since last update (in seconds)
    std::chrono::duration<float> dt = (std::chrono::steady_clock::now() - mLastTime_);
    mLastTime_ = std::chrono::steady_clock::now();
    // Update application content
    mpWindow_->update(dt.count());
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
    InputHandlerDesktop* handler = (InputHandlerDesktop*)mpWindow_->getInputHandler();

    // Propagate key events to the scenes
    while (auto keyEvent = handler->getKeyEvent()) {
        for (auto it = mScenes_.rbegin(); it != mScenes_.rend(); ++it) {
            (*it)->onInputEvent(keyEvent.value());
        }
    }

    // Propagate mouse events to scenes
    while (auto mouseEvent = handler->getMouseEvent()) {
        for (auto it = mScenes_.rbegin(); it != mScenes_.rend(); ++it) {
            (*it)->onInputEvent(mouseEvent.value());
        }
    }
}

void AppDesktop::render() {
    // Set background color from scene
    if (!mScenes_.empty()) {
        glm::vec4 sceneBackgroundColor = mScenes_.front()->getBackgroundColor();
        mGraphicsAPI_->clearColor(sceneBackgroundColor.r, sceneBackgroundColor.g, sceneBackgroundColor.b, sceneBackgroundColor.a);
        mpRenderer_->clearBuffers(
            sceneBackgroundColor,
            {0,0,0,1},
            {0,0,0,1}
        );
    } else {
        mpRenderer_->clearBuffers(
            {0,0,0,1},
            {0,0,0,1},
            {0,0,0,1}
        );
    }

    mGraphicsAPI_->bindFrameBuffer(IGraphicsAPI::FrameBufferTarget::FRAMEBUFFER, 0);

    // Render list in reverse order
    IGraphicsContext gContext(*mpRenderer_);
    for (auto it = mScenes_.rbegin(); it != mScenes_.rend(); ++it) {
       (*it)->render(gContext);
    }

    // render guis on top (avoid gamma correction)
    mGraphicsAPI_->disable(IGraphicsAPI::Capability::FRAMEBUFFER_SRGB);
    for (auto it = mScenes_.rbegin(); it != mScenes_.rend(); ++it) {
       (*it)->renderGUI();
    }
    // swap buffers
    mpWindow_->render();
}

bool AppDesktop::isRunning() const {
    // TODO more running conditions. For now running is when the window is open
    return mpWindow_->isRunning();
}

void AppDesktop::quit() {
    // TODO do any saving
    // Set window to close
    glfwSetWindowShouldClose(((WindowDesktop*)mpWindow_.get())->getGLFWWindow(), true);
}

void AppDesktop::setScene(BaseScene* newScene) {
    mScenes_.push_back(std::unique_ptr<BaseScene>(newScene));
}

IWindow* AppDesktop::getWindow() {
    return mpWindow_.get();
}

void AppDesktop::setWindow(std::unique_ptr<IWindow> pWindow) {
    mpWindow_ = std::move(pWindow);
}

void AppDesktop::setAntiAliasing(unsigned int sampleSize) {
   // Set sample size
   glfwWindowHint(GLFW_SAMPLES, sampleSize);
   // Enable/disable anti-aliasing based on sample size
   if (sampleSize != 0) {
       mGraphicsAPI_->enable(IGraphicsAPI::Capability::MULTISAMPLE);
   } else {
       mGraphicsAPI_->disable(IGraphicsAPI::Capability::MULTISAMPLE);
   }
}

void AppDesktop::initializeOpenGL() {
    // Only initialize if this if the first time
    if (!sOpenGLInitialized_) {
        sOpenGLInitialized_ = true;
    }
}

AudioManager& AppDesktop::getAudioManager() {
    return mAudioManager_;
}

Resources& AppDesktop::getResources() {
    return mResources_;
}

RendererOpenGL& AppDesktop::getRenderer() {
    return *mpRenderer_.get();
}

IGraphicsAPI* AppDesktop::getGraphicsAPI() {
    return mGraphicsAPI_;
}

} // namespace clay

#endif
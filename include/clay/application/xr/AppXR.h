#pragma once
#ifdef CLAY_PLATFORM_XR

// standard lib
#include <string>
#include <vector>
// third party
// project
#include "clay/graphics/opengles/GraphicsAPIOpenGLES.h" // include this on top
#include "clay/graphics/opengles/RendererOpenGLES.h"
#include "clay/audio/AudioManager.h"
#include "clay/application/xr/InputHandlerXR.h"
#include "clay/application/common/Resources.h"
#include "clay/application/common/BaseScene.h"
#include "clay/application/common/IApp.h"
#include "clay/gui/xr/WindowXR.h"

namespace clay {

class AppXR : public IApp {
public:
    struct AndroidAppState {
        ANativeWindow* nativeWindow = nullptr;
        bool resumed = false;
    };

    struct SwapchainInfo {
        XrSwapchain swapchain = XR_NULL_HANDLE;
        int64_t swapchainFormat = 0;
        std::vector<void*> imageViews;
    };

    struct RenderLayerInfo {
        XrTime predictedDisplayTime = 0;
        std::vector<XrCompositionLayerBaseHeader*> layers;
        XrCompositionLayerProjection layerProjection = {XR_TYPE_COMPOSITION_LAYER_PROJECTION};
        std::vector<XrCompositionLayerProjectionView> layerProjectionViews;
    };

    static void AndroidAppHandleCmd(struct android_app* app, int32_t cmd);

    AppXR(android_app* pAndroidApp);

    ~AppXR();

    virtual void initialize();

    void run();

    void setScene(BaseScene* newScene) override;

    InputHandlerXR& getInputHandler();

    AAssetManager* getAssetManager();

    Resources& getResources() override;

    AudioManager& getAudioManager() override;

    IGraphicsAPI* getGraphicsAPI() override;

    IWindow* getWindow() override;

    utils::FileData loadFileToMemory_XR(const std::string& filePath);

    utils::ImageData loadImageFileToMemory_XR(const std::string &filePath);

    // must be called before initialize. Must load the the shaders "TextShader",  "TextureFlipShader", and mesh "RectPlane"
    virtual void createResources();

protected:
    void createInstance();

    void getInstanceProperties();

    void getSystemID();

    void getViewConfigurationViews();

    void getEnvironmentBlendModes();

    void createSession();

    void createReferenceSpace();

    void createSwapchains();

    void pollSystemEvents();

    void pollEvents();

    void renderFrame();

    void destroySwapchains();

    void destroyReferenceSpace();

    void destroyResources();

    void destroySession();

    void destroyInstance();

    bool renderLayer(RenderLayerInfo &renderLayerInfo);

protected:
    AndroidAppState mAndroidAppState_;
    android_app* mpAndroidApp_;
    XrInstance mXRInstance_ = XR_NULL_HANDLE;
    std::vector<const char*> mActiveAPILayers_ = {};
    std::vector<const char*> mActiveInstanceExtensions_ = {};
    XrFormFactor mFormFactor_ = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    XrSystemId mSystemID_ = {};
    XrSession mSession_ = {};
    bool mApplicationRunning_ = true;
    bool mSessionRunning_ = false;
    std::vector<std::string> mInstanceExtensions = {};
    std::vector<std::string> mApiLayers_ = {};
    XrSystemProperties mSystemProperties_ = {XR_TYPE_SYSTEM_PROPERTIES};
    XrSpace mLocalSpace_ = XR_NULL_HANDLE;
    XrSpace mHeadSpace_ = XR_NULL_HANDLE;
    std::vector<SwapchainInfo> mColorSwapchainInfos_ = {};
    std::vector<SwapchainInfo> mDepthSwapchainInfos_ = {};
    std::vector<SwapchainInfo> mStencilSwapchainInfos_ = {};
    XrViewConfigurationType mViewConfiguration_ = XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM;
    std::vector<XrViewConfigurationView> mViewConfigurationViews_;
    XrSessionState mSessionState_ = XR_SESSION_STATE_UNKNOWN;
    XrEnvironmentBlendMode mEnvironmentBlendMode_ = XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM;
    std::vector<XrEnvironmentBlendMode> mEnvironmentBlendModes_ = {};
    std::vector<XrEnvironmentBlendMode> mApplicationEnvironmentBlendModes_ = {XR_ENVIRONMENT_BLEND_MODE_OPAQUE, XR_ENVIRONMENT_BLEND_MODE_ADDITIVE};
    std::vector<XrViewConfigurationType> mViewConfigurations_;
    std::vector<XrViewConfigurationType> mApplicationViewConfigurations_ = {XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO};
    std::unique_ptr<GraphicsAPIOpenGLES> mGraphicsAPI_ = nullptr;
    std::unique_ptr<RendererOpenGLES> mRenderer_ = nullptr;
    std::vector<std::unique_ptr<BaseScene>> mScenes_;
    Resources mResources_;
    InputHandlerXR mInputHandler_;
    AudioManager mAudioManger_;
    WindowXR mWindow_;
};

} // namespace clay

#endif
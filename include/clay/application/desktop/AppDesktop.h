#pragma once
#ifdef CLAY_PLATFORM_DESKTOP

// standard lib
#include <chrono>
#include <list>
// third party
// project
#include "clay/graphics/common/IGraphicsAPI.h"
#include "clay/graphics/common/ShaderProgram.h"
#include "clay/application/common/Resources.h"
#include "clay/application/common/BaseScene.h"
#include "clay/audio/AudioManager.h"
#include "clay/graphics/common/Renderer.h"
#include "clay/gui/desktop/WindowDesktop.h"
#include "clay/gui/common/IWindow.h"
#include "clay/application/common/IApp.h"

namespace clay {

class AppDesktop: public IApp {
public:
    /** Constructor */
    AppDesktop();

    /** Destructor */
    ~AppDesktop();

    void initialize();

    /** Start and do the run loop */
    void run();

    /** Update the application components */
    void update();

    /** Render the application components */
    void render();

    /** If the application is currently running */
    bool isRunning() const;

    /**
     * Quits the application. Quit should be handled by the Application and not the window to ensure
     * proper closure of the application
     */
    void quit();

    /**
     * Set the current Scene of the application
     * @param newScene The new Scene
     */
    void setScene(BaseScene* newScene);

    /**
     * Set Anti-Aliasing sample size. If the size is 0 then anti aliasing is disabled
     * @param sampleSize Anti aliasing sample size
     */
    void setAntiAliasing(unsigned int sampleSize);

    /** Get Application Window */
    IWindow* getWindow();

    void setWindow(std::unique_ptr<IWindow> pWindow);

    /** Get the audio manager for this application */
    AudioManager& getAudioManger();

    /** Get application resources */
    Resources& getResources();

    /** Get the Renderer for this App */
    Renderer& getRenderer();

    // TODO USE THIS IN SCENES TO PASS TO RESOURCES
    IGraphicsAPI* getGraphicsAPI();

private:
    /** Load/Build the common resources for the scenes in this application */
    void loadResources();
    /** Initialize OpenGL if not already initialized */
    static void initializeOpenGL();

    /** If OpenGL is already initalized. Used to prevent redundant initialization */
    static bool sOpenGLInitialized_;
    /** Time of last update call */
    std::chrono::steady_clock::time_point mLastTime_;
    /** The window for this application*/
    std::unique_ptr<IWindow> mpWindow_;
    /** The current Scenes of the application. List to allow controlled scene deleting */
    std::list<std::unique_ptr<BaseScene>> mScenes_;
    /** Renderer used to render the scenes */
    std::unique_ptr<Renderer> mpRenderer_;
    /** Audio manager */
    AudioManager mAudioManger_;
    /** Resource for this application that can be shared with child scenes */
    Resources mResources_;

    IGraphicsAPI* mGraphicsAPI_ = nullptr;
};
} // namespace clay

#endif
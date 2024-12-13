#pragma once
// standard lib
#include <chrono>
#include <list>
#include <numbers>
// third party
#define GLEW_STATIC
#include <GL/glew.h>
// project
#include "Clay/Application/Logger.h"
#include "Clay/Application/Resource.h"
#include "Clay/Application/Scene.h"
#include "Clay/Audio/AudioManager.h"
#include "Clay/Graphics/Renderer.h"
#include "Clay/GUI/ImGuiComponent.h"
#include "Clay/GUI/Window.h"

namespace clay {

class App {
public:
    /** Constructor */
    App();

    /** Destructor */
    ~App();

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
    void setScene(Scene* newScene);

    /**
     * Set Anti-Aliasing sample size. If the size is 0 then anti aliasing is disabled
     * @param sampleSize Anti aliasing sample size
     */
    void setAntiAliasing(unsigned int sampleSize);

    /** Get Application Window */
    Window& getWindow();

    /** Get the audio manager for this application */
    AudioManager& getAudioManger();

    /** Get application resources */
    Resource& getResources();

    /** Get the Renderer for this App */
    Renderer& getRenderer();

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
    Window mWindow_;
    /** The current Scenes of the application. List to allow controlled scene deleting */
    std::list<std::unique_ptr<Scene>> mScenes_;
    /** Renderer used to render the scenes */
    std::unique_ptr<Renderer> mpRenderer_;
    /** Audio manager */
    AudioManager mAudioManger_;
    /** Resource for this application that can be shared with child scenes */
    Resource mResources_;
};
} // namespace clay
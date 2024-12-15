#pragma once
// third party
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/vec4.hpp>
// project
#include "clay/application/InputHandler.h"
#include "clay/application/Resource.h"
#include "clay/entity/Entity.h"
#include "clay/graphics/common/Camera.h"
#include "clay/graphics/common/Renderer.h"

namespace clay {

// Forward declare App
class App;

class Scene {
public:
    /**
     * Constructor
     * @param theApp Parent app handling this Scene
     */
    Scene(App& theApp);

    /** Destructor */
    virtual ~Scene() = default;

    /**
     * Update scene content
     * @param dt time since last update in seconds
     */
    virtual void update(const float dt) = 0;

    /**
     * Render this scene
     * @param renderer Rendering helper
     */
    virtual void render(Renderer& renderer) = 0;

    /**
     * Render just the gui for this scene
     */
    virtual void renderGUI() = 0;

    /** Build/load resources for this scene */
    virtual void assembleResources();

    /**
     * Set the Scene background color
     * @param newColor background color
     */
    void setBackgroundColor(glm::vec4 newColor);

    /**
     * Get this Scene background color
     */
    glm::vec4 getBackgroundColor() const;

    /** Get this Scene's parent App */
    clay::App& getApp();

    /** Get the current focused Camera */
    Camera* getFocusCamera();

    /** Set this Scene to be deleted */
    void setRemove(const bool remove);

    /** If this scene is set for removal */
    bool isRemove() const;

    /** Set this Scene to be deleted */
    void setRunning(const bool running);

    /** If this scene is set for removal */
    bool isRunning() const;

    /** @brief Get this scene's resources */
    Resource& getResources();

    /**
     * On keyboard key press handler
     * @param code key code for pressed key
     */
    virtual void onKeyPress(unsigned int code);

    /**
     * On keyboard key release handler
     * @param code key code for released key
     */
    virtual void onKeyRelease(unsigned int code);

    /**
     * On Mouse button press handler
     * @param mouseEvent Mouse event details
     */
    virtual void onMousePress(const InputHandler::MouseEvent& mouseEvent);

    /**
     * On Mouse Button release handler
     * @param mouseEvent Mouse event details
     */
    virtual void onMouseRelease(const InputHandler::MouseEvent& mouseEvent);

    /**
     * @brief One mouse wheel event handler
     *
     * @param mouseEvent Mouse event
     */
    virtual void onMouseWheel(const InputHandler::MouseEvent& mouseEvent);

protected:
    /** Parent app handling this Scene*/
    App& mApp_;
    /** Resource for this Scene */
    Resource mResources_;
    /** Camera for this scene */
    std::unique_ptr<Camera> mpSceneCamera_;
    /** The current focused camera the scene is rendered through */
    Camera* mpFocusCamera_;
    /** Background color when rendering this scene */
    glm::vec4 mBackgroundColor_ = {0.0f, 0.0f, 0.0f, 1.0f};
    /** If this scene should be deleted */
    bool mIsRemove_ = false;
    /** If this scene is currently updating */
    bool mIsRunning_ = true;
};

} // namespace clay
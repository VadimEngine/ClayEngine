#pragma once
// third party
#include <glm/vec4.hpp>
// project
#include "clay/application/common/IInputHandler.h"
#include "clay/application/common/Resources.h"
#include "clay/entity/Entity.h"
#include "clay/graphics/common/Camera.h"
#include "clay/graphics/common/Renderer.h"

namespace clay {

// Forward declare IApp
class IApp;

class BaseScene {
public:
    /**
     * Constructor
     * @param theApp Parent app handling this Scene
     */
    BaseScene(IApp& theApp);

    /** Destructor */
    virtual ~BaseScene() = default;

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
    clay::IApp& getApp();

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
    Resources& getResources();

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

    // TODO remove desktop from here. Have 1 method that takes in all events
    /**
     * On Mouse button press handler
     * @param mouseEvent Mouse event details
     */
    virtual void onMousePress(const IInputHandler::MouseEvent& mouseEvent);

    /**
     * On Mouse Button release handler
     * @param mouseEvent Mouse event details
     */
    virtual void onMouseRelease(const IInputHandler::MouseEvent& mouseEvent);

    /**
     * @brief One mouse wheel event handler
     *
     * @param mouseEvent Mouse event
     */
    virtual void onMouseWheel(const IInputHandler::MouseEvent& mouseEvent);

protected:
    /** Parent app handling this Scene*/
    IApp& mApp_;
    /** Resource for this Scene */
    Resources mResources_;
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
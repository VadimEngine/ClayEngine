#pragma once
#ifdef CLAY_PLATFORM_VR

// third party
#include <glm/vec4.hpp>
// project
#include <clay/graphics/common/IGraphicsContext.h>
#include <clay/application/xr/CameraXR.h>

namespace clay {

// Forward declare AppXR
class AppXR;

class SceneXR {
public:
    SceneXR(AppXR* theApp);

    virtual ~SceneXR();

    virtual void update(float dt) = 0;

    virtual void render(clay::IGraphicsContext& gContext) = 0;

    /** If this scene is set for removal */
    bool isRemove() const;

    /** Set this Scene to be deleted */
    void setRunning(const bool running);

    /** If this scene is set for removal */
    bool isRunning() const;

    /**
     * Set the Scene background color
     * @param newColor background color
     */
    void setBackgroundColor(glm::vec4 newColor);

    /**
     * Get this Scene background color
     */
    glm::vec4 getBackgroundColor() const;

    void setRemove(const bool remove);

    AppXR* getApp();

protected:
    AppXR* mpApp_ = nullptr;
    clay::CameraXR mCamera_;
    glm::vec4 mBackgroundColor_ = {0.0f, 0.0f, 0.0f, 1.0f};
    /** If this scene should be deleted */
    bool mIsRemove_ = false;
    /** If this scene is currently updating */
    bool mIsRunning_ = true;
};

} // namespace clay

#endif

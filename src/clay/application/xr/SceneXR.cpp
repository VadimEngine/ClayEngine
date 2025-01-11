#ifdef CLAY_PLATFORM_VR

// project
#include "clay/application/xr/AppXR.h"
// class
#include "clay/application/xr/SceneXR.h"

namespace clay {

SceneXR::SceneXR(AppXR* theApp)
: mpApp_(theApp){}

SceneXR::~SceneXR() {}

/** If this scene is set for removal */
bool SceneXR::isRemove() const {
    return mIsRemove_;
}

/** Set this Scene to be deleted */
void SceneXR::setRunning(const bool running) {
    mIsRunning_ = running;
}

/** If this scene is set for removal */
bool SceneXR::isRunning() const {
    return mIsRunning_;
}

void SceneXR::setBackgroundColor(glm::vec4 newColor) {
    mBackgroundColor_ = newColor;
}

glm::vec4 SceneXR::getBackgroundColor() const {
    return mBackgroundColor_;
}

AppXR* SceneXR::getApp() {
    return mpApp_;
}

void SceneXR::setRemove(const bool remove) {
    mIsRemove_ = remove;
}

} // namespace clay

#endif
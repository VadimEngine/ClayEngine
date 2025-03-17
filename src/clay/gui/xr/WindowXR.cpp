#ifdef CLAY_PLATFORM_XR

// class
#include "clay/gui/xr/WindowXR.h"

namespace clay {

glm::ivec2 WindowXR::getDimensions() const {
    // TODO return correct dimension
    return {0,0};
}

IInputHandler *WindowXR::getInputHandler() {
    return nullptr;
}

void WindowXR::enableDisplay(bool enabled) {}

void WindowXR::update(float dt) {}

void WindowXR::render() {}

bool WindowXR::isRunning() const {
    return true;
}

} // namespace clay

#endif
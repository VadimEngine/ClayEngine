#pragma once
#ifdef CLAY_PLATFORM_XR
#include "clay/gui/common/IWindow.h"

namespace clay {

class WindowXR : public IWindow {

    glm::ivec2 getDimensions() const override;

    IInputHandler* getInputHandler() override;

    void enableDisplay(bool enabled) override;

    void update(float dt) override;

    void render() override;

    bool isRunning() const override;
};

} // namespace clay

#endif
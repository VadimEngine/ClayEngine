#pragma once
// third party
#include <glm/vec2.hpp>
// project
#include "clay/application/common/IInputHandler.h"

namespace clay {

class IWindow {
public:
    virtual ~IWindow() = default;

    virtual glm::ivec2 getDimensions() const = 0;

    virtual IInputHandler* getInputHandler() = 0;

    virtual void enableDisplay(bool enabled) = 0;

    virtual void update(float dt) = 0;

    virtual void render() = 0;

    virtual bool isRunning() const = 0;

    
};
    
} // namespace clay

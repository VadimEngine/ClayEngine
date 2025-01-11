#pragma once
#include "clay/gui/common/IWindow.h"
#include "clay/graphics/common/IGraphicsAPI.h"
#include "clay/application/common/Resources.h"
#include "clay/application/common/BaseScene.h"

namespace clay {

class IApp {
public:
    virtual ~IApp() = default;

    virtual IWindow* getWindow() = 0;

    virtual IGraphicsAPI* getGraphicsAPI() = 0;

    virtual Resources& getResources() = 0;

    virtual void setScene(BaseScene* newScene) = 0;
};
    
} // namespace clay

#pragma once
#ifdef CLAY_PLATFORM_XR
// third party
#include <openxr/openxr.h>
// project
#include "clay/graphics/common/IGraphicsContext.h"
#include "clay/graphics/opengles/GraphicsAPIOpenGLES.h"
#include "clay/graphics/opengles/RendererOpenGLES.h"

namespace clay {

class GraphicsContextXR : public IGraphicsContext {
public:
    GraphicsContextXR(XrView& v, GraphicsAPIOpenGLES& api, RendererOpenGLES& renderer)
        : IGraphicsContext(renderer), view(v), graphicsAPI(api) {}

    inline RendererOpenGLES& getRendererES() {
        return static_cast<RendererOpenGLES&>(renderer);
    }

    XrView view;
    GraphicsAPIOpenGLES& graphicsAPI;
};
} // namespace clay
#endif
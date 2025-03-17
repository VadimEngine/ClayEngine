#pragma once
#include "clay/graphics/common/IRenderer.h"

namespace clay {

class IGraphicsContext {
public:

    IGraphicsContext(IRenderer& renderer) : renderer(renderer) {}

    virtual ~IGraphicsContext() = default;

    IRenderer& renderer;

};

} // namespace clay
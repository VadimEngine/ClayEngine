#pragma once
#ifdef CLAY_PLATFORM_VR
// standard lib
// third party
#include <openxr/openxr.h>
// project
#include "clay/graphics/common/IGraphicsContext.h"

namespace clay {

class GraphicsContextXR : public IGraphicsContext {
public:
    XrView view;
};

}

#endif
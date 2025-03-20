#include "clay/graphics/common/IRenderer.h"

namespace clay {

void IRenderer::setCameraUBO(unsigned int cameraUBO) {
    mCurrentCameraUBO_ = cameraUBO;
}

unsigned int IRenderer::getCurrentCameraUBO() const {
    return mCurrentCameraUBO_;
}

unsigned int IRenderer::getCurrentLightUBO() const {
    return mCurrentLightUBO_;
}

} // namespace clay
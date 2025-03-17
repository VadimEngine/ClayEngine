#include "clay/graphics/common/IRenderer.h"

namespace clay {

void IRenderer::setUBO(unsigned int ubo) {
    mCurrentUBO_ = ubo;
}

unsigned int IRenderer::getCurrentUBO() const {
    return mCurrentUBO_;
}

} // namespace clay
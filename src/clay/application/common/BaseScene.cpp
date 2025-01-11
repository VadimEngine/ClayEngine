// standard lib
// third party
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// ClayEngine
#include "clay/application/desktop/AppDesktop.h"
#include "clay/application/common/IApp.h"
// class
#include "clay/application/common/BaseScene.h"

namespace clay {

BaseScene::BaseScene(IApp& theApp)
    : mApp_(theApp), mpSceneCamera_(std::make_unique<Camera>()), mpFocusCamera_(mpSceneCamera_.get()) {
    // Set camera aspect ratio
    glm::vec2 screenDim = mApp_.getWindow()->getDimensions();
    mpSceneCamera_->setAspectRatio(static_cast<float>(screenDim.x)/static_cast<float>(screenDim.y));
    mResources_.mGraphicsAPI_ = (mApp_.getGraphicsAPI());
}

void BaseScene::assembleResources() {}

void BaseScene::setBackgroundColor(glm::vec4 newColor) {
    mBackgroundColor_ = newColor;
}

glm::vec4 BaseScene::getBackgroundColor() const {
    return mBackgroundColor_;
}

IApp& BaseScene::getApp() {
    return mApp_;
}

Camera* BaseScene::getFocusCamera() {
    return mpFocusCamera_;
}

bool BaseScene::isRemove() const {
    return mIsRemove_;
}

void BaseScene::setRemove(const bool remove) {
    mIsRemove_ = remove;
}

void BaseScene::setRunning(const bool running) {
    mIsRunning_ = running;
}

bool BaseScene::isRunning() const {
    return mIsRunning_;
}

Resources& BaseScene::getResources() {
    return mResources_;
}

void BaseScene::onKeyPress(unsigned int code) {}

void BaseScene::onKeyRelease(unsigned int code) {}

void BaseScene::onMousePress(const IInputHandler::MouseEvent& mouseEvent) {}

void BaseScene::onMouseRelease(const IInputHandler::MouseEvent& mouseEvent) {}

void BaseScene::onMouseWheel(const IInputHandler::MouseEvent& mouseEvent) {}

} // namespace clay
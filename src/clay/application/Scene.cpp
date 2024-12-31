// standard lib
// third party
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// ClayEngine
#include "clay/application/App.h"
// class
#include "clay/application/Scene.h"

namespace clay {

Scene::Scene(App& theApp)
    : mApp_(theApp), mpSceneCamera_(std::make_unique<Camera>()), mpFocusCamera_(mpSceneCamera_.get()) {
    // Set camera aspect ratio
    glm::vec2 screenDim = mApp_.getWindow().getWindowDimensions();
    mpSceneCamera_->setAspectRatio(static_cast<float>(screenDim.x)/static_cast<float>(screenDim.y));
    mResources_.mGraphicsAPI_ = &(mApp_.getGraphicsAPI());
}

void Scene::assembleResources() {}

void Scene::setBackgroundColor(glm::vec4 newColor) {
    mBackgroundColor_ = newColor;
}

glm::vec4 Scene::getBackgroundColor() const {
    return mBackgroundColor_;
}

App& Scene::getApp() {
    return mApp_;
}

Camera* Scene::getFocusCamera() {
    return mpFocusCamera_;
}

bool Scene::isRemove() const {
    return mIsRemove_;
}

void Scene::setRemove(const bool remove) {
    mIsRemove_ = remove;
}

void Scene::setRunning(const bool running) {
    mIsRunning_ = running;
}

bool Scene::isRunning() const {
    return mIsRunning_;
}

Resource& Scene::getResources() {
    return mResources_;
}


void Scene::onKeyPress(unsigned int code) {}

void Scene::onKeyRelease(unsigned int code) {}

void Scene::onMousePress(const InputHandler::MouseEvent& mouseEvent) {}

void Scene::onMouseRelease(const InputHandler::MouseEvent& mouseEvent) {}

void Scene::onMouseWheel(const InputHandler::MouseEvent& mouseEvent) {}

} // namespace clay
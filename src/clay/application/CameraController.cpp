// forward declare
#include "clay/graphics/Camera.h"
// class
#include "clay/application/CameraController.h"

namespace clay {

CameraController::CameraController(Camera* pCamera, InputHandler& theInputHandler)
    :mpCamera_(pCamera), mInputHandler_(theInputHandler) {}

void CameraController::update(float dt) {
    // Move/rotate/zoom camera based on key inputs
    // Move
    if (mInputHandler_.isKeyPressed(GLFW_KEY_W)) {
        mpCamera_->move(mpCamera_->getForward(), mpCamera_->getMoveSpeed() * dt);
    }
    if (mInputHandler_.isKeyPressed(GLFW_KEY_S)) {
        mpCamera_->move(mpCamera_->getForward(), -mpCamera_->getMoveSpeed() * dt);
    }
    if (mInputHandler_.isKeyPressed(GLFW_KEY_A)) {
        mpCamera_->move(mpCamera_->getRight(), -mpCamera_->getMoveSpeed() * dt);
    }
    if (mInputHandler_.isKeyPressed(GLFW_KEY_D)) {
        mpCamera_->move(mpCamera_->getRight(), mpCamera_->getMoveSpeed() * dt);
    }
    if (mInputHandler_.isKeyPressed(GLFW_KEY_SPACE)) {
        mpCamera_->move({0,1,0}, mpCamera_->getMoveSpeed() * dt);
    }
    if (mInputHandler_.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        mpCamera_->move({0,1,0}, -mpCamera_->getMoveSpeed() * dt);
    }
    // Rotate TODO FIX THIS ROLL does not work and there seems to be a gimbal lock
    if (mInputHandler_.isKeyPressed(GLFW_KEY_LEFT)) {
        mpCamera_->rotate(mpCamera_->getUp(), mpCamera_->getRotationSpeed() * dt);
    }
    if (mInputHandler_.isKeyPressed(GLFW_KEY_RIGHT)) {
        mpCamera_->rotate(mpCamera_->getUp(), -mpCamera_->getRotationSpeed() * dt);
    }
    if (mInputHandler_.isKeyPressed(GLFW_KEY_UP)) {
        mpCamera_->rotate(mpCamera_->getRight(), mpCamera_->getRotationSpeed() * dt);
    }
    if (mInputHandler_.isKeyPressed(GLFW_KEY_DOWN)) {
        mpCamera_->rotate(mpCamera_->getRight(), -mpCamera_->getRotationSpeed() * dt);
    }
    if (mInputHandler_.isKeyPressed(GLFW_KEY_Q)) {
        mpCamera_->rotate(mpCamera_->getForward(), 200.0f * dt);
    }
    if (mInputHandler_.isKeyPressed(GLFW_KEY_E)) {
        mpCamera_->rotate(mpCamera_->getForward(), -200.0f * dt);
    }
    // Speed
    if (mInputHandler_.isKeyPressed(GLFW_KEY_PERIOD)) {
        mpCamera_->zoom(-mpCamera_->getZoomSpeed() * dt);
    }
    if (mInputHandler_.isKeyPressed(GLFW_KEY_COMMA)) {
        mpCamera_->zoom(mpCamera_->getZoomSpeed() * dt);
    }
}

void CameraController::setCamera(Camera* newCamera) {
    mpCamera_ = newCamera;
}

Camera* CameraController::getCamera() {
    return mpCamera_;
}

} // namespace clay
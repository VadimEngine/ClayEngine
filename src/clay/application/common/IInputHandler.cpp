#include "clay/application/common/IInputHandler.h"

namespace clay {

IInputHandler::KeyEvent::KeyEvent(Type type, unsigned int code)
    : mType_(type), mCode_(code) {
}

IInputHandler::KeyEvent::KeyEvent::Type IInputHandler::KeyEvent::getType() const {
    return mType_;
}

unsigned int IInputHandler::KeyEvent::getCode() const {
    return mCode_;
}

IInputHandler::MouseEvent::MouseEvent(Type type, Button button, const glm::ivec2& position)
    : mType_(type), button(button), mPosition_(position) {
}

IInputHandler::MouseEvent::Type IInputHandler::MouseEvent::getType() const {
    return mType_;
}

IInputHandler::MouseEvent::Button IInputHandler::MouseEvent::getButton() const {
    return button;
}

glm::ivec2 IInputHandler::MouseEvent::getPosition() const {
    return mPosition_;
}

} // namespace clay
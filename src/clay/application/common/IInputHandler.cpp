#include "clay/application/common/IInputHandler.h"

namespace clay {
#ifdef CLAY_PLATFORM_DESKTOP

KeyEvent::KeyEvent(Type type, unsigned int code)
    : InputEvent(InputEvent::EventType::KEY), mKeyType_(type), mCode_(code) {
}

KeyEvent::KeyEvent::Type KeyEvent::getType() const {
    return mKeyType_;
}

unsigned int KeyEvent::getCode() const {
    return mCode_;
}

MouseEvent::MouseEvent(MouseEvent::Type type, Button button, const glm::ivec2& position)
    : InputEvent(InputEvent::EventType::MOUSE), mMouseType_(type), button(button), mPosition_(position) {
}

MouseEvent::Type MouseEvent::getType() const {
    return mMouseType_;
}

MouseEvent::Button MouseEvent::getButton() const {
    return button;
}

glm::ivec2 MouseEvent::getPosition() const {
    return mPosition_;
}
#endif
} // namespace clay
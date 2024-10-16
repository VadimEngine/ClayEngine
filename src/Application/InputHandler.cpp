// class
#include "Clay/Application/InputHandler.h"

namespace clay {

// EVENT

InputHandler::KeyEvent::KeyEvent(Type type, unsigned int code)
    : mType_(type), mCode_(code) {
}

InputHandler::KeyEvent::KeyEvent::Type InputHandler::KeyEvent::getType() const {
    return mType_;
}

unsigned int InputHandler::KeyEvent::getCode() const {
    return mCode_;
}

InputHandler::MouseEvent::MouseEvent(Type type, Button button, const glm::ivec2& position)
    : mType_(type), button(button), mPosition_(position) {
}

InputHandler::MouseEvent::Type InputHandler::MouseEvent::getType() const {
    return mType_;
}

InputHandler::MouseEvent::Button InputHandler::MouseEvent::getButton() const {
    return button;
}

glm::ivec2 InputHandler::MouseEvent::getPosition() const {
    return mPosition_;
}

// END OF EVENT

// Input Handler

InputHandler::InputHandler() {}

InputHandler::~InputHandler() {}

void InputHandler::onKeyPressed(int keyCode) {
    mKeyStates_[keyCode] = true;
    mKeyEventQueue_.push(InputHandler::KeyEvent(InputHandler::KeyEvent::Type::PRESS, keyCode));
    trimBuffer(mKeyEventQueue_);
}

void InputHandler::onKeyReleased(int keyCode) {
    mKeyStates_[keyCode] = false;
    mKeyEventQueue_.push(InputHandler::KeyEvent(InputHandler::KeyEvent::Type::RELEASE, keyCode));
    trimBuffer(mKeyEventQueue_);
}

bool InputHandler::isKeyPressed(int keyCode) const {
    return mKeyStates_[keyCode];
}

void InputHandler::clearKeys() {
    mKeyStates_.reset();
}

std::optional<InputHandler::KeyEvent> InputHandler::getKeyEvent() {
    if (mKeyEventQueue_.size() > 0) {
        InputHandler::KeyEvent e = mKeyEventQueue_.front();
        mKeyEventQueue_.pop();
        return e;
    } else {
        return std::nullopt;
    }
}

// START MOUSE

bool InputHandler::isMouseButtonPressed(InputHandler::MouseEvent::Button button) {
    return mMouseStates_[static_cast<int>(button)];
}

void InputHandler::onMousePress(InputHandler::MouseEvent::Button button) {
    mMouseEventQueue_.push(InputHandler::MouseEvent(
        InputHandler::MouseEvent::Type::PRESS,
        button,
        mMousePosition_
    ));

    trimBuffer(mMouseEventQueue_);
}

void InputHandler::onMouseRelease(InputHandler::MouseEvent::Button button) {
    mMouseEventQueue_.push(InputHandler::MouseEvent(
        InputHandler::MouseEvent::Type::RELEASE,
        button,
        mMousePosition_
    ));

    trimBuffer(mMouseEventQueue_);
}

void InputHandler::onMouseMove(int x, int y) {
    auto button = InputHandler::MouseEvent::Button::NONE;

    for (int i = 0; i < static_cast<int>(InputHandler::MouseEvent::Button::NUM_BUTTONS); ++i) {
        if (mMouseStates_[i]) {
            button = static_cast<InputHandler::MouseEvent::Button>(i);
            break;
        }
    }
    mMousePosition_ = {x, y};
    mMouseEventQueue_.push(InputHandler::MouseEvent(
        InputHandler::MouseEvent::Type::MOVE,
        button,
        mMousePosition_
    ));

    trimBuffer(mMouseEventQueue_);
}

std::optional<InputHandler::MouseEvent> InputHandler::getMouseEvent() {
    if (mMouseEventQueue_.size() > 0) {
        InputHandler::MouseEvent e = mMouseEventQueue_.front();
        mMouseEventQueue_.pop();
        return e;
    } else {
        return std::nullopt;
    }
}

glm::ivec2 InputHandler::getMousePosition() {
    return mMousePosition_;
}

void InputHandler::onMouseWheel(float yOffset) {
    auto button = InputHandler::MouseEvent::Button::NONE;

    mMouseEventQueue_.push(InputHandler::MouseEvent(
        (yOffset > 0) ?
            InputHandler::MouseEvent::Type::SCROLL_UP :
            InputHandler::MouseEvent::Type::SCROLL_DOWN,
        button,
        mMousePosition_
    ));
    trimBuffer(mMouseEventQueue_);
}

// END MOUSE

template<typename T>
void InputHandler::trimBuffer(T eventQueue) {
    while (eventQueue.size() > kMaxQueueSize) {
        eventQueue.pop();
    }
}

// Explicit instantiate template for expected types
template void InputHandler::InputHandler::trimBuffer(std::queue<KeyEvent> eventQueue);
template void InputHandler::InputHandler::trimBuffer(std::queue<MouseEvent> eventQueue);

// End of Input Handler

} // namespace clay
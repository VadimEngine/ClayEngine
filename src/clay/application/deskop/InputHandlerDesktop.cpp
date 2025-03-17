#ifdef CLAY_PLATFORM_DESKTOP

// class
#include "clay/application/desktop/InputHandlerDesktop.h"

namespace clay {

InputHandlerDesktop::InputHandlerDesktop() {}

InputHandlerDesktop::~InputHandlerDesktop() {}

void InputHandlerDesktop::onKeyPressed(int keyCode) {
    mKeyStates_[keyCode] = true;
    mKeyEventQueue_.push(KeyEvent(KeyEvent::Type::PRESS, keyCode));
    trimBuffer(mKeyEventQueue_);
}

void InputHandlerDesktop::onKeyReleased(int keyCode) {
    mKeyStates_[keyCode] = false;
    mKeyEventQueue_.push(KeyEvent(KeyEvent::Type::RELEASE, keyCode));
    trimBuffer(mKeyEventQueue_);
}

bool InputHandlerDesktop::isKeyPressed(int keyCode) const {
    return mKeyStates_[keyCode];
}

void InputHandlerDesktop::clearKeys() {
    mKeyStates_.reset();
}

std::optional<KeyEvent> InputHandlerDesktop::getKeyEvent() {
    if (mKeyEventQueue_.size() > 0) {
        KeyEvent e = mKeyEventQueue_.front();
        mKeyEventQueue_.pop();
        return e;
    } else {
        return std::nullopt;
    }
}

// START MOUSE

bool InputHandlerDesktop::isMouseButtonPressed(MouseEvent::Button button) {
    return mMouseStates_[static_cast<int>(button)];
}

void InputHandlerDesktop::onMousePress(MouseEvent::Button button) {
    mMouseEventQueue_.push(MouseEvent(
        MouseEvent::Type::PRESS,
        button,
        mMousePosition_
    ));

    trimBuffer(mMouseEventQueue_);
}

void InputHandlerDesktop::onMouseRelease(MouseEvent::Button button) {
    mMouseEventQueue_.push(MouseEvent(
        MouseEvent::Type::RELEASE,
        button,
        mMousePosition_
    ));

    trimBuffer(mMouseEventQueue_);
}

void InputHandlerDesktop::onMouseMove(int x, int y) {
    auto button = MouseEvent::Button::NONE;

    for (int i = 0; i < static_cast<int>(MouseEvent::Button::NUM_BUTTONS); ++i) {
        if (mMouseStates_[i]) {
            button = static_cast<MouseEvent::Button>(i);
            break;
        }
    }
    mMousePosition_ = {x, y};
    mMouseEventQueue_.push(MouseEvent(
        MouseEvent::Type::MOVE,
        button,
        mMousePosition_
    ));

    trimBuffer(mMouseEventQueue_);
}

std::optional<MouseEvent> InputHandlerDesktop::getMouseEvent() {
    if (mMouseEventQueue_.size() > 0) {
        MouseEvent e = mMouseEventQueue_.front();
        mMouseEventQueue_.pop();
        return e;
    } else {
        return std::nullopt;
    }
}

glm::ivec2 InputHandlerDesktop::getMousePosition() {
    return mMousePosition_;
}

void InputHandlerDesktop::onMouseWheel(float yOffset) {
    auto button = MouseEvent::Button::NONE;

    mMouseEventQueue_.push(MouseEvent(
        (yOffset > 0) ?
            MouseEvent::Type::SCROLL_UP :
            MouseEvent::Type::SCROLL_DOWN,
        button,
        mMousePosition_
    ));
    trimBuffer(mMouseEventQueue_);
}

// END MOUSE

template<typename T>
void InputHandlerDesktop::trimBuffer(T eventQueue) {
    while (eventQueue.size() > kMaxQueueSize) {
        eventQueue.pop();
    }
}

// Explicit instantiate template for expected types
template void InputHandlerDesktop::InputHandlerDesktop::trimBuffer(std::queue<KeyEvent> eventQueue);
template void InputHandlerDesktop::InputHandlerDesktop::trimBuffer(std::queue<MouseEvent> eventQueue);


} // namespace clay

#endif
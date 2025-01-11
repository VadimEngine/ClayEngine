#ifdef CLAY_PLATFORM_DESKTOP

// class
#include "clay/application/desktop/InputHandlerDesktop.h"

namespace clay {

InputHandlerDesktop::InputHandlerDesktop() {}

InputHandlerDesktop::~InputHandlerDesktop() {}

void InputHandlerDesktop::onKeyPressed(int keyCode) {
    mKeyStates_[keyCode] = true;
    mKeyEventQueue_.push(IInputHandler::KeyEvent(IInputHandler::KeyEvent::Type::PRESS, keyCode));
    trimBuffer(mKeyEventQueue_);
}

void InputHandlerDesktop::onKeyReleased(int keyCode) {
    mKeyStates_[keyCode] = false;
    mKeyEventQueue_.push(InputHandlerDesktop::KeyEvent(IInputHandler::KeyEvent::Type::RELEASE, keyCode));
    trimBuffer(mKeyEventQueue_);
}

bool InputHandlerDesktop::isKeyPressed(int keyCode) const {
    return mKeyStates_[keyCode];
}

void InputHandlerDesktop::clearKeys() {
    mKeyStates_.reset();
}

std::optional<IInputHandler::KeyEvent> InputHandlerDesktop::getKeyEvent() {
    if (mKeyEventQueue_.size() > 0) {
        IInputHandler::KeyEvent e = mKeyEventQueue_.front();
        mKeyEventQueue_.pop();
        return e;
    } else {
        return std::nullopt;
    }
}

// START MOUSE

bool InputHandlerDesktop::isMouseButtonPressed(IInputHandler::MouseEvent::Button button) {
    return mMouseStates_[static_cast<int>(button)];
}

void InputHandlerDesktop::onMousePress(IInputHandler::MouseEvent::Button button) {
    mMouseEventQueue_.push(IInputHandler::MouseEvent(
        IInputHandler::MouseEvent::Type::PRESS,
        button,
        mMousePosition_
    ));

    trimBuffer(mMouseEventQueue_);
}

void InputHandlerDesktop::onMouseRelease(IInputHandler::MouseEvent::Button button) {
    mMouseEventQueue_.push(IInputHandler::MouseEvent(
        IInputHandler::MouseEvent::Type::RELEASE,
        button,
        mMousePosition_
    ));

    trimBuffer(mMouseEventQueue_);
}

void InputHandlerDesktop::onMouseMove(int x, int y) {
    auto button = IInputHandler::MouseEvent::Button::NONE;

    for (int i = 0; i < static_cast<int>(IInputHandler::MouseEvent::Button::NUM_BUTTONS); ++i) {
        if (mMouseStates_[i]) {
            button = static_cast<IInputHandler::MouseEvent::Button>(i);
            break;
        }
    }
    mMousePosition_ = {x, y};
    mMouseEventQueue_.push(IInputHandler::MouseEvent(
        IInputHandler::MouseEvent::Type::MOVE,
        button,
        mMousePosition_
    ));

    trimBuffer(mMouseEventQueue_);
}

std::optional<IInputHandler::MouseEvent> InputHandlerDesktop::getMouseEvent() {
    if (mMouseEventQueue_.size() > 0) {
        IInputHandler::MouseEvent e = mMouseEventQueue_.front();
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
    auto button = IInputHandler::MouseEvent::Button::NONE;

    mMouseEventQueue_.push(IInputHandler::MouseEvent(
        (yOffset > 0) ?
            IInputHandler::MouseEvent::Type::SCROLL_UP :
            IInputHandler::MouseEvent::Type::SCROLL_DOWN,
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
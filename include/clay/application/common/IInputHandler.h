#pragma once
// third party
#include <glm/vec2.hpp>

namespace clay {

class InputEvent {
public:
    enum class EventType {
#ifdef CLAY_PLATFORM_DESKTOP
        // if desktop
        KEY,
        MOUSE,
#endif
#ifdef CLAY_PLATFORM_XR
        XR
#endif
    };

    InputEvent(EventType type)
        : type_(type) {}

    virtual ~InputEvent() = default;

    const EventType type_;
};

#ifdef CLAY_PLATFORM_DESKTOP
class KeyEvent: public InputEvent {
public:
    enum class Type {
        PRESS, RELEASE
    };
private:
    Type mKeyType_;
    unsigned int mCode_;
public:
    KeyEvent(Type type, unsigned int code);
    Type getType() const;
    unsigned int getCode() const;
};
class MouseEvent: public InputEvent {
public:
    enum class Type {
        PRESS,
        RELEASE,
        SCROLL_UP, // can down and up be merged to just mouse scroll? will need an extra field to hold the dir/amount
        SCROLL_DOWN,
        MOVE,
        ENTER,
        LEAVE,
    };
    enum class Button {
        LEFT=0, MIDDLE, RIGHT, NUM_BUTTONS, NONE
    };
private:
    Type mMouseType_;
    Button button;
    glm::ivec2 mPosition_;
public:
    MouseEvent(Type type, Button button, const glm::ivec2& position);
    Type getType() const;
    Button getButton() const;
    glm::ivec2 getPosition() const;
};
#endif

class IInputHandler {
public:

    virtual ~IInputHandler() = default;
};

} // namespace clay
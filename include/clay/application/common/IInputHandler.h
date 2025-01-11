#pragma once
// third party
#include <glm/vec2.hpp>

namespace clay {

class IInputHandler {
public:
    class KeyEvent {
    public:
        enum class Type {
            PRESS, RELEASE
        };
    private:
        Type mType_;
        unsigned int mCode_;
    public:
        KeyEvent(Type type, unsigned int code);
        Type getType() const;
        unsigned int getCode() const;
    };

    class MouseEvent {
    public:
        enum class Type {
            PRESS,
            RELEASE,
            SCROLL_UP,
            SCROLL_DOWN,
            MOVE,
            ENTER,
            LEAVE,
        };
        enum class Button {
            LEFT=0, MIDDLE, RIGHT, NUM_BUTTONS, NONE
        };
    private:
        Type mType_;
        Button button;
        glm::ivec2 mPosition_;
    public:
        MouseEvent(Type type, Button button, const glm::ivec2& position);
        Type getType() const;
        Button getButton() const;
        glm::ivec2 getPosition() const;
    };


    virtual ~IInputHandler() = default;

};

} // namespace clay
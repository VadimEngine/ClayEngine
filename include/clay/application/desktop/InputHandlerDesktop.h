#pragma once
#ifdef CLAY_PLATFORM_DESKTOP

// standard lib
#include <bitset>
#include <optional>
#include <queue>
// third party
#include <glm/vec2.hpp>
// project
#include "clay/application/common/IInputhandler.h"

namespace clay {

class InputHandlerDesktop: public IInputHandler {
public:
    /** Constructor */
    InputHandlerDesktop();

    /** Destructor*/
    ~InputHandlerDesktop();

    /**
     * Track the key that is pressed
     * @param keyCode Keycode for pressed key
     */
    void onKeyPressed(int keyCode);

    /**
     * Track the key that is released
     * @param keyCode Keycode for pressed key
     */
    void onKeyReleased(int keyCode);

    /** Check if a key is marked as pressed */
    bool isKeyPressed(int keyCode) const;

    /** Set all tracked keys as not pressed. Used when losing focus on application */
    void clearKeys();

    /** */
    std::optional<KeyEvent> getKeyEvent();

    bool isMouseButtonPressed(MouseEvent::Button button);

    std::optional<MouseEvent> getMouseEvent();

    void onMousePress(MouseEvent::Button button);

    void onMouseRelease(MouseEvent::Button button);

    void onMouseMove(int x, int y);

    glm::ivec2 getMousePosition();

    void onMouseWheel(float yOffset);

private:
    /** Helper method to keep keyBuffer within the size limit */
    template<typename T>
    void trimBuffer(T eventQueue);

    /** Number of key codes to check status of*/
    static constexpr unsigned int kKeys = 512;
    /** Max number of key events in the event queue */
    static constexpr unsigned int kMaxQueueSize = 16;

	/** Bitset to track which keys are currently pressed*/
    std::bitset<kKeys> mKeyStates_;
    /** Queue of key events to use for single updates on a key event */
    std::queue<KeyEvent> mKeyEventQueue_;

    /** Current mouse position */
    glm::ivec2 mMousePosition_;

    /** Bitset to track current mouse button state */
    std::bitset<3> mMouseStates_;
    /** Queue of mouse events to use for single updates on a mouse event */
    std::queue<MouseEvent> mMouseEventQueue_;
};
} // namespace clay

#endif
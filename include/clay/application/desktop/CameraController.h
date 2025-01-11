#pragma once

#ifdef CLAY_PLATFORM_DESKTOP

// third party
// project
#include "clay/application/desktop/InputHandlerDesktop.h"

namespace clay {

// forward declare Camera
class Camera;

class CameraController {
private:
    /** Camera being controlled (pointer to allow replacing camera) */
    Camera* mpCamera_ = nullptr;
    /** Input handler to get input from */
    InputHandlerDesktop& mInputHandler_;

public:
    /**
     * Constructor.
     * @param pCamera Camera to be controlled
     * @param pInputHandler Input handler to listen to
     */
    CameraController(Camera* pCamera, InputHandlerDesktop& theInputHandler);

    /**
     * Update the camera based on inputs
     * @param dt Time since last update (in seconds)
     */
    void update(float dt);

    /** Set the camera to be controlled */
    void setCamera(Camera* newCamera);

    /** Get the camera this controller is controlling */
    Camera* getCamera();
};

} // namespace clay

#endif
#pragma once
#define AL_LIBTYPE_STATIC
// standard lib
#include <stdio.h>
#include <stdexcept>
// third party
#include <AL/al.h>
#include <AL/alc.h>

namespace clay {

class SoundDevice {
public:
    /** Constructor */
    SoundDevice();
    /** Destructor */
    ~SoundDevice();
private:
    /** Pointer to found audio device */
    ALCdevice* mpALCDevice_;
    /** Pointer to context to allow track audio state*/
    ALCcontext* mpALCContext_;
};

} // namespace clay
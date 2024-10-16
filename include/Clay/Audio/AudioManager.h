#pragma once
// standard lib
#include <inttypes.h>
// third party
#include <AL/alext.h>
#include <sndfile.h>
// project
#include "Clay/Audio/SoundDevice.h"
#include "Clay/Audio/SoundSource.h"

namespace clay {

class AudioManager {
public:
    /** Constructor */
    AudioManager();

    /** Destructor */
    ~AudioManager();

    /**
     * Play the audio if it has been loaded
     * @param audioId Audio buffer id
     */
    void playSound(ALuint audioId);

    /**
     * Set the gain of the audio source
     * @param newGain Gain to set (0-1.0f)
     */
    void setGain(float newGain);

    /** Get the current Gain */
    float getGain() const;

private:
    /** Sound Device*/
    SoundDevice* mSoundDevice_;
    /** Sound Source */
    SoundSource* mSoundSource_;
};

} // namespace clay
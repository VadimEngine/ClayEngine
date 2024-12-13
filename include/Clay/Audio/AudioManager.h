#pragma once
// standard lib
#include <inttypes.h>
#include <memory>
// third party
#include <AL/alext.h>
#include <sndfile.h>
#include <stdexcept>
// project
#include "Clay/Audio/SoundDevice.h"
#include "Clay/Audio/SoundSource.h"
#include "Clay/Application/Logger.h"

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
    std::unique_ptr<SoundDevice> mpSoundDevice_;
    /** Sound Source */
    std::unique_ptr<SoundSource> mpSoundSource_;
    /** If audio was set up successfully */
    bool mAudioInitialized_ = false;
};

} // namespace clay
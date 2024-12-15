// class
#include "clay/audio/AudioManager.h"

namespace clay {

AudioManager::AudioManager() {
    try {
        mpSoundDevice_ = std::make_unique<SoundDevice>();
        mpSoundSource_ = std::make_unique<SoundSource>();
        mAudioInitialized_ = true;
    } catch (const std::exception& e) {
        LOG_E("AudioManager error: %s", e.what());
    }
}

AudioManager::~AudioManager() {}

void AudioManager::playSound(ALuint audioId) {
    if (mAudioInitialized_) {
        mpSoundSource_->play(audioId);
    }
}

void AudioManager::setGain(float newGain) {
    if (mAudioInitialized_) {
        mpSoundSource_->setGain(newGain);
    }
}

float AudioManager::getGain() const {
    if (mAudioInitialized_) {
        return mpSoundSource_->getGain();
    } else {
        return 0;
    }
}

} // namespace clay
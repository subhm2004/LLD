#ifndef I_AUDIO_OUTPUT_DEVICE_HPP
#define I_AUDIO_OUTPUT_DEVICE_HPP
#include "../models/Song.hpp"

class IAudioOutputDevice {
public:
    virtual ~IAudioOutputDevice() {}
    virtual void playAudio(Song* song) = 0;
};
#endif // I_AUDIO_OUTPUT_DEVICE_HPP

#
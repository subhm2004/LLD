// ============================================================================
//  device/IAudioOutputDevice.h — ADAPTER pattern ka "TARGET" interface 🎯
// ----------------------------------------------------------------------------
//  Ye wo interface hai jo hamara AudioEngine EXPECT karta hai — sirf
//  playAudio(song). Engine bas itna jaanta hai; use farq nahi padta device
//  bluetooth hai, wired ya headphones.
//
//  ADAPTER pattern ka funda: humare paas external speaker APIs hain jinke
//  method-naam ALAG hain (playSoundViaBluetooth, playSoundViaCable,
//  playSoundViaJack) — engine unse SEEDHA baat nahi kar sakta. Adapters
//  in APIs ko IS interface (playAudio) me "translate" karte hain. Engine
//  ek hi bhasha bolta hai, adapter uski translation karta hai! (L16 Adapter)
// ============================================================================
#ifndef I_AUDIO_OUTPUT_DEVICE_HPP
#define I_AUDIO_OUTPUT_DEVICE_HPP
#include "../models/Song.h"

class IAudioOutputDevice {
public:
    virtual ~IAudioOutputDevice() {}
    virtual void playAudio(Song* song) = 0;
};
#endif // I_AUDIO_OUTPUT_DEVICE_HPP

#
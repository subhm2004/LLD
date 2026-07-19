// ============================================================================
//  device/HeadphonesAdapter.h — ADAPTER: headphones ke liye
// ----------------------------------------------------------------------------
//  Teesra adapter — playAudio(song) [target] ko playSoundViaJack(payload)
//  [headphones API] me translate karta hai. Teeno adapters ka structure
//  BILKUL SAME hai, sirf wrapped API aur uske method-naam alag. Yahi
//  Adapter pattern ki khoobi: alag-alag external APIs, ek uniform interface.
// ============================================================================
#ifndef HEADPHONES_ADAPTER_HPP
#define HEADPHONES_ADAPTER_HPP

#include "../models/Song.h"
#include "IAudioOutputDevice.h"
#include "../external/HeadphonesAPI.h"

using namespace std;

class HeadphonesAdapter : public IAudioOutputDevice {
private:
    HeadphonesAPI* headphonesApi;
public:
    HeadphonesAdapter(HeadphonesAPI* api) {
        headphonesApi = api;
    }

    void playAudio(Song* song) override {
        string payload = song->getTitle() + " by " + song->getArtist();
        headphonesApi->playSoundViaJack(payload);
    }
};


#endif // HEADPHONES_ADAPTER_HPP

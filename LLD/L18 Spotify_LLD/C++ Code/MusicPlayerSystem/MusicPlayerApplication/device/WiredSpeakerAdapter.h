// ============================================================================
//  device/WiredSpeakerAdapter.h — ADAPTER: wired speaker ke liye
// ----------------------------------------------------------------------------
//  BluetoothSpeakerAdapter jaisa hi — playAudio(song) [target] ko
//  playSoundViaCable(payload) [wired API] me translate karta hai. Song ko
//  string payload me convert karke external API ko forward. Engine ko farq
//  nahi ki andar wired hai ya bluetooth — sab IAudioOutputDevice hain.
// ============================================================================
#ifndef WIRED_SPEAKER_ADAPTER_HPP
#define WIRED_SPEAKER_ADAPTER_HPP

#include "../models/Song.h"
#include "IAudioOutputDevice.h"
#include "../external/WiredSpeakerAPI.h"

using namespace std;

class WiredSpeakerAdapter : public IAudioOutputDevice {
private:
    WiredSpeakerAPI* wiredApi;
public:
    WiredSpeakerAdapter(WiredSpeakerAPI* api) {
        wiredApi = api;
    }

    void playAudio(Song* song) override {
        string payload = song->getTitle() + " by " + song->getArtist();
        wiredApi->playSoundViaCable(payload);
    }
};


#endif // WIRED_SPEAKER_ADAPTER_HPP
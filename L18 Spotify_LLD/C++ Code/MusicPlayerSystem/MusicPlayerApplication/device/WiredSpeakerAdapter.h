// device/WiredSpeakerAdapter.h — ADAPTER: IAudioOutputDevice ko external
// WiredSpeakerAPI me translate karta hai (same role as other adapters).
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
#ifndef WIRED_SPEAKER_ADAPTER_HPP
#define WIRED_SPEAKER_ADAPTER_HPP

#include "../models/Song.hpp"
#include "IAudioOutputDevice.hpp"
#include "../external/WiredSpeakerAPI.hpp"

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
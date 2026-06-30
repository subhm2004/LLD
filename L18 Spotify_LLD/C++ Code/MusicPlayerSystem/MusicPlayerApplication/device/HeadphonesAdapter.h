// device/HeadphonesAdapter.h — ADAPTER: IAudioOutputDevice ko external
// HeadphonesAPI me translate karta hai (same role as Bluetooth adapter).
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

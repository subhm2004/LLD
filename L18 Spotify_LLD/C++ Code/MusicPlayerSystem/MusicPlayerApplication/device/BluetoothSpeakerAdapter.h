// device/BluetoothSpeakerAdapter.h — ADAPTER: IAudioOutputDevice ko external
// BluetoothSpeakerAPI me convert karta hai. Engine standard playAudio() bolta hai,
// adapter use bluetooth API ke method me translate kar deta hai.
# ifndef BLUETOOTH_SPEAKER_ADAPTER_HPP
# define BLUETOOTH_SPEAKER_ADAPTER_HPP
#include "../models/Song.h"
#include "IAudioOutputDevice.h"
#include "../external/BluetoothSpeakerAPI.h"

using namespace std;

class BluetoothSpeakerAdapter : public IAudioOutputDevice {
private:
    BluetoothSpeakerAPI* bluetoothApi;
public:
    BluetoothSpeakerAdapter(BluetoothSpeakerAPI* api) {
        bluetoothApi = api;
    }
    
    void playAudio(Song* song) override {
        string payload = song->getTitle() + " by " + song->getArtist();
        bluetoothApi->playSoundViaBluetooth(payload);
    }
};

#endif // BLUETOOTH_SPEAKER_ADAPTER_HPP
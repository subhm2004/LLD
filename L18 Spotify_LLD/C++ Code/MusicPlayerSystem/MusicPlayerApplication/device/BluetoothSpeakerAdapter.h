// ============================================================================
//  device/BluetoothSpeakerAdapter.h — ADAPTER 🔌 (translator between 2 interfaces)
// ----------------------------------------------------------------------------
//  Ye Adapter pattern ka DIL hai. Do interfaces jodta hai:
//    - Target (jo engine bolta): IAudioOutputDevice::playAudio(song)
//    - Adaptee (external API):   BluetoothSpeakerAPI::playSoundViaBluetooth(str)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  TRANSLATION kaise hoti hai:                                            │
//  │                                                                          │
//  │   Engine bolta hai:  adapter->playAudio(song)  [target interface]       │
//  │                            │                                            │
//  │   Adapter ANDAR:  1. Song object ko string me convert                   │
//  │                      ("title by artist" payload banao)                  │
//  │                   2. External API ke method ko call:                    │
//  │                      bluetoothApi->playSoundViaBluetooth(payload)       │
//  │                                                                          │
//  │  Engine ko external API ka naam tak nahi pata — adapter beech me        │
//  │  "translator" ki tarah baitha hai. Naya speaker brand aaye? Naya        │
//  │  adapter, engine untouched! ✅                                          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  Structure: adapter Target interface IMPLEMENT karta hai (is-a) aur
//  Adaptee ko HOLD karta hai (has-a) — inheritance + composition dono.
//  (WiredSpeakerAdapter aur HeadphonesAdapter bilkul same, sirf API alag.)
// ============================================================================
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
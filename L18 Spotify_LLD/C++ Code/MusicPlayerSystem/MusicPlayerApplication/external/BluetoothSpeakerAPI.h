#ifndef BLUETOOTH_SPEAKER_API_HPP
#define BLUETOOTH_SPEAKER_API_HPP
#include<string>
#include<iostream>

using namespace std;

class BluetoothSpeakerAPI {
public:
    void playSoundViaBluetooth(const string& data) {
        cout << "[BluetoothSpeaker] Playing: " << data << "\n";
        // mimics playing music
    }
};
#endif // BLUETOOTH_SPEAKER_API_HPP


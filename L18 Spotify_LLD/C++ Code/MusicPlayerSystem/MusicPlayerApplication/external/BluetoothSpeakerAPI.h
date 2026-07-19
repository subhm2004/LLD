// ============================================================================
//  external/BluetoothSpeakerAPI.h — "ADAPTEE" #1: third-party bluetooth API
// ----------------------------------------------------------------------------
//  Ye ek EXTERNAL / third-party library ki class hai jise hum CHANGE NAHI
//  kar sakte (imagine ye kisi vendor ki SDK hai). Iska method-naam
//  playSoundViaBluetooth() hai — humare IAudioOutputDevice ke playAudio()
//  se ALAG. Isi "mismatch" ki wajah se Adapter ki zaroorat padti hai.
//  Adapter pattern me isko "ADAPTEE" kehte hain (jise adapt karna hai).
//  (external/ folder ki teeno APIs — Bluetooth/Wired/Headphones — same role.)
// ============================================================================
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


// ============================================================================
//  external/HeadphonesAPI.h — "ADAPTEE" #3: third-party headphones API
// ----------------------------------------------------------------------------
//  Teesra external API — method playSoundViaJack(). Teeno APIs (Bluetooth/
//  Wired/Headphones) ke method-naam ALAG-ALAG hain — yahi problem Adapter
//  solve karta hai (sabko ek common playAudio() me laata hai).
//  HeadphonesAdapter isse IAudioOutputDevice me wrap karta hai.
// ============================================================================
#ifndef HEADPHONES_API_HPP
#define HEADPHONES_API_HPP
#include<string>
#include<iostream>

using namespace std;

class HeadphonesAPI {
public:
    void playSoundViaJack(const string& data) {
        cout << "[Headphones] Playing: " << data << "\n";
        // mimics playing music
    }
};
#endif // HEADPHONES_API_HPP
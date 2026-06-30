// external/HeadphonesAPI.h — Third-party headphones API (incompatible interface);
// HeadphonesAdapter isse IAudioOutputDevice me wrap karta hai.
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
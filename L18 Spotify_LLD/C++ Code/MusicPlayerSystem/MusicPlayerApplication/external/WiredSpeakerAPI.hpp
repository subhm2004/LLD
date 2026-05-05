#ifndef WIRED_SPEAKER_API_HPP
#define WIRED_SPEAKER_API_HPP
#include<string>
#include<iostream>

using namespace std;

class WiredSpeakerAPI {
public:
    void playSoundViaCable(const string& data) {
        cout << "[WiredSpeaker] Playing: " << data << "\n";
        // mimics playing music
    }
};
#endif // WIRED_SPEAKER_API_HPP
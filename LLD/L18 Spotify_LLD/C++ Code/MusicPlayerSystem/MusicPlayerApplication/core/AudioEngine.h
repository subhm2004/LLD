// ============================================================================
//  core/AudioEngine.h — PLAYBACK engine (system ka core jahan "sound" nikalta)
// ----------------------------------------------------------------------------
//  Ye actual play/pause karta hai — current song + paused state track karta.
//  play() ko output DEVICE (adapter) diya jaata hai, aur wo device pe
//  playAudio() call karta hai (yahi jagah adapter ka translation trigger hota).
//
//  ⭐ SMART RESUME logic: agar same song jo paused tha, wapas play karo to
//  "Resuming..." bolta hai (naya playback nahi). Ye state machine chhota hai:
//    playing -> pause() -> paused -> play(same song) -> resuming -> playing
//
//  DHYAN DO — Engine ko device ke CONCRETE type ka pata nahi (Bluetooth/
//  Wired?) — bas IAudioOutputDevice* jaanta hai. Yahi Adapter + polymorphism
//  ka fayda: engine ek hi bhasha bolta hai, device kuch bhi ho.
// ============================================================================
#ifndef AUDIO_ENGINE_HPP
#define AUDIO_ENGINE_HPP

#include "../models/Song.h"
#include "../device/IAudioOutputDevice.h"
#include<string>
#include<iostream>

using namespace std;

class AudioEngine {
private:
    Song* currentSong;
    bool songIsPaused;
public:
    AudioEngine() {
        currentSong = nullptr;
        songIsPaused = false;
    }
    string getCurrentSongTitle() const {
        if (currentSong) {
            return currentSong->getTitle();
        }
        return "";
    }
    bool isPaused() const {
        return songIsPaused;
    }
    void play(IAudioOutputDevice* aod, Song* song) {
        if (song == nullptr) {
            throw runtime_error("Cannot play a null song.");
        }
        // Resume if same song was paused
        if (songIsPaused && song == currentSong) {
            songIsPaused = false;
            cout << "Resuming song: " << song->getTitle() << "\n";
            aod->playAudio(song);
            return;
        }

        currentSong = song;
        songIsPaused = false;
        cout << "Playing song: " << song->getTitle() << "\n";
        aod->playAudio(song);
    }

    void pause() {
        if (currentSong == nullptr) {
            throw runtime_error("No song is currently playing to pause.");
        }
        if (songIsPaused) {
            throw runtime_error("Song is already paused.");
        }
        songIsPaused = true;
        cout << "Pausing song: " << currentSong->getTitle() << "\n";
    }
};

#endif // AUDIO_ENGINE_HPP

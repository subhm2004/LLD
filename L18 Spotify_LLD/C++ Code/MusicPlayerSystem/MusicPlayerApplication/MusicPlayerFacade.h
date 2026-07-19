// ============================================================================
//  MusicPlayerFacade.h — FACADE (L17 pattern): playback subsystem ka coordinator
// ----------------------------------------------------------------------------
//  Ye class subsystem ke saare parts (DeviceManager, StrategyManager,
//  PlaylistManager, AudioEngine) ko COORDINATE karti hai — client ko in sab
//  ke aapasi steps ka gyaan nahi chahiye.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  EK method ke PEECHE kitne parts (playAllTracks example):               │
//  │                                                                          │
//  │   while (playStrategy->hasNext()) {        // STRATEGY se order         │
//  │       Song* s = playStrategy->next();                                   │
//  │       device = DeviceManager::getOutputDevice();  // konsa device       │
//  │       audioEngine->play(device, s);        // ENGINE actual play        │
//  │   }                                                                     │
//  │                                                                          │
//  │  3 subsystems (strategy + device + engine) ek loop me — client ko sirf  │
//  │  playAllTracks() dikhta hai. Yahi Facade ka asli fayda!                 │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ITSELF SINGLETON: ek hi facade instance. Andar AudioEngine bhi rakhta hai
//  (playback state — current song/paused). Managers ko getInstance() se
//  access karta (wo bhi singletons).
//
//  ⚠️ NOTE: playSong() pehle check karta hai device connected hai — nahi to
//  exception. loadPlaylist() strategy set hone ka check karta. Ye guard
//  clauses achhe hain (fail-fast), par ownership/cleanup (delete) missing
//  hai (chhota demo — production me smart pointers chahiye).
// ============================================================================
# ifndef MUSIC_PLAYER_FACADE_HPP
# define MUSIC_PLAYER_FACADE_HPP
#include "core/AudioEngine.h"
#include "models/Playlist.h"
#include "models/Song.h"
#include "strategies/PlayStrategy.h"
#include "enums/DeviceType.h"
#include "enums/PlayStrategyType.h"
#include "managers/DeviceManager.h"
#include "managers/PlaylistManager.h"
#include "managers/StrategyManager.h"

using namespace std;

class MusicPlayerFacade {
private:
    static MusicPlayerFacade* instance;
    AudioEngine* audioEngine;
    Playlist* loadedPlaylist;
    PlayStrategy* playStrategy;

    MusicPlayerFacade() {
        loadedPlaylist = nullptr;
        playStrategy   = nullptr;
        audioEngine = new AudioEngine();
    }

public:
    static MusicPlayerFacade* getInstance() {
        if (!instance) {
            instance = new MusicPlayerFacade();
        }
        return instance;
    }

    void connectDevice(DeviceType deviceType) {
        DeviceManager::getInstance()->connect(deviceType);
    }

    void setPlayStrategy(PlayStrategyType strategyType) {
        playStrategy = StrategyManager::getInstance()->getStrategy(strategyType);
    }

    void loadPlaylist(const string& name) {
        loadedPlaylist = PlaylistManager::getInstance()->getPlaylist(name);
        if (!playStrategy) {
            throw runtime_error("Play strategy not set before loading.");
        }
        playStrategy->setPlaylist(loadedPlaylist);
    }
    
    void playSong(Song* song) {
        if (!DeviceManager::getInstance()->hasOutputDevice()) {
            throw runtime_error("No audio device connected.");
        }
        IAudioOutputDevice* device = DeviceManager::getInstance()->getOutputDevice();
        audioEngine->play(device, song);
    }

    void pauseSong(Song* song) {
        if (audioEngine->getCurrentSongTitle() != song->getTitle()) {
            throw runtime_error("Cannot pause \"" + song->getTitle() + "\"; not currently playing.");
        }
        audioEngine->pause();
    }

    void playAllTracks() {
        if (!loadedPlaylist) {
            throw runtime_error("No playlist loaded.");
        }
        while (playStrategy->hasNext()) {
            Song* nextSong = playStrategy->next();
            IAudioOutputDevice* device = DeviceManager::getInstance()->getOutputDevice();
            audioEngine->play(device, nextSong);
        }
        cout << "Completed playlist: " << loadedPlaylist->getPlaylistName() << "\n";
    }

    void playNextTrack() {
        if (!loadedPlaylist) {
            throw runtime_error("No playlist loaded.");
        }
        if(playStrategy->hasNext()) {
            Song* nextSong = playStrategy->next();
            IAudioOutputDevice* device = DeviceManager::getInstance()->getOutputDevice();
            audioEngine->play(device, nextSong);
        }
        else {
            cout << "Completed playlist: " << loadedPlaylist->getPlaylistName() << "\n";
        }
    }

    void playPreviousTrack() {
        if (!loadedPlaylist) {
            throw runtime_error("No playlist loaded.");
        }
        if(playStrategy->hasPrevious()) {
            Song* prevSong = playStrategy->previous();
            IAudioOutputDevice* device = DeviceManager::getInstance()->getOutputDevice();
            audioEngine->play(device, prevSong);
        }
        else {
            cout << "Completed playlist: " << loadedPlaylist->getPlaylistName() << "\n";
        }
    }

    void enqueueNext(Song* song) {
        playStrategy->addToNext(song);
    }
};

MusicPlayerFacade* MusicPlayerFacade::instance = nullptr;
#endif // MUSIC_PLAYER_FACADE_HPP
// MusicPlayerFacade.h — FACADE: client ke liye simple API (play/pause/next,
// playlist load, device connect). Andar managers/engine ke saare steps chhupata
// hai taaki caller ko subsystem ki complexity na dekhni pade.
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
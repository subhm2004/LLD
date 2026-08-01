// ============================================================================
//  MusicPlayerApplication.h — TOP-LEVEL app (SINGLETON) + song library
// ----------------------------------------------------------------------------
//  System ka sabse upar wala layer — client (main.cpp) sirf isse baat karta.
//  Do kaam:
//    1. SONG LIBRARY manage karta hai (saare songs ka master store) +
//       findSongByTitle() se lookup
//    2. Baaki har kaam MusicPlayerFacade ko DELEGATE karta hai (playlist,
//       device, strategy, playback) — khud complexity nahi rakhta
//
//  SINGLETON kyun: pura app EK hi hona chahiye (ek music player instance).
//  Ye SIMPLE lazy singleton hai (bas null-check) — thread-safe nahi.
//  (`inline static` C++17 feature se static member yahin define ho jaata,
//   alag .cpp line ki zaroorat nahi.)
//
//  DHYAN DO — do-layer facade design: Application KHUD ek thin wrapper hai
//  jo Facade ke upar hai. Application = "app-level API + song library",
//  Facade = "playback subsystem coordination". Dono milke client ko ekdum
//  saaf interface dete hain.
// ============================================================================
#ifndef MUSIC_PLAYER_APPLICATION_HPP
#define MUSIC_PLAYER_APPLICATION_HPP
#include "MusicPlayerFacade.h"
#include "managers/PlaylistManager.h"
#include <bits/stdc++.h>

using namespace std;

class MusicPlayerApplication {
private:
  inline static MusicPlayerApplication *instance = nullptr;
  vector<Song *> songLibrary;
  MusicPlayerApplication() {}

public:
  static MusicPlayerApplication *getInstance() {
    if (!instance) {
      instance = new MusicPlayerApplication();
    }
    return instance;
  }

  void createSongInLibrary(const string &title, const string &artist,
                           const string &path) {
    Song *newSong = new Song(title, artist, path);
    songLibrary.push_back(newSong);
  }

  Song *findSongByTitle(const string &title) {
    for (Song *s : songLibrary) {
      if (s->getTitle() == title) {
        return s;
      }
    }
    return nullptr;
  }
  void createPlaylist(const string &playlistName) {
    PlaylistManager::getInstance()->createPlaylist(playlistName);
  }

  void addSongToPlaylist(const string &playlistName, const string &songTitle) {
    Song *song = findSongByTitle(songTitle);
    if (!song) {
      throw runtime_error("Song \"" + songTitle + "\" not found in library.");
    }
    PlaylistManager::getInstance()->addSongToPlaylist(playlistName, song);
  }

  void connectAudioDevice(DeviceType deviceType) {
    MusicPlayerFacade::getInstance()->connectDevice(deviceType);
  }

  void selectPlayStrategy(PlayStrategyType strategyType) {
    MusicPlayerFacade::getInstance()->setPlayStrategy(strategyType);
  }

  void loadPlaylist(const string &playlistName) {
    MusicPlayerFacade::getInstance()->loadPlaylist(playlistName);
  }

  void playSingleSong(const string &songTitle) {
    Song *song = findSongByTitle(songTitle);
    if (!song) {
      throw runtime_error("Song \"" + songTitle + "\" not found.");
    }
    MusicPlayerFacade::getInstance()->playSong(song);
  }

  void pauseCurrentSong(const string &songTitle) {
    Song *song = findSongByTitle(songTitle);
    if (!song) {
      throw runtime_error("Song \"" + songTitle + "\" not found.");
    }
    MusicPlayerFacade::getInstance()->pauseSong(song);
  }

  void playAllTracksInPlaylist() {
    MusicPlayerFacade::getInstance()->playAllTracks();
  }

  void playPreviousTrackInPlaylist() {
    MusicPlayerFacade::getInstance()->playPreviousTrack();
  }

  void queueSongNext(const string &songTitle) {
    Song *song = findSongByTitle(songTitle);
    if (!song) {
      throw runtime_error("Song \"" + songTitle + "\" not found.");
    }
    MusicPlayerFacade::getInstance()->enqueueNext(song);
  }
};

#endif // MUSIC_PLAYER_APPLICATION_HPP
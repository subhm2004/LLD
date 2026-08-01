// ============================================================================
//  models/Playlist.h — Songs ka ORDERED collection (naam + Song list)
// ----------------------------------------------------------------------------
//  Ek playlist = naam + songs ka vector (order maayne rakhta — sequential
//  strategy isi order pe chalti hai). Songs ko POINTER se rakhta (copies
//  nahi) — ek hi Song multiple playlists me ho sakta hai (library shared).
//  PlaylistManager inhe manage karta; PlayStrategy inpe next/previous karti.
//  addSong me null-check hai (fail-fast). getSongs() const list deta.
// ============================================================================
#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP
#include <vector>
#include <string>
#include <iostream>
#include "Song.h"

using namespace std;

class Playlist
{
private:
    string playlistName;
    vector<Song *> songList;

public:
    Playlist(string name)
    {
        playlistName = name;
    }
    string getPlaylistName()
    {
        return playlistName;
    }
    const vector<Song *> getSongs()
    {
        return songList;
    }
    int getSize()
    {
        return (int)songList.size();
    }
    void addSongToPlaylist(Song *song)
    {
        if (song == nullptr)
        {
            throw runtime_error("Cannot add null song to playlist.");
        }
        songList.push_back(song);
    }
};
#endif // PLAYLIST_HPP
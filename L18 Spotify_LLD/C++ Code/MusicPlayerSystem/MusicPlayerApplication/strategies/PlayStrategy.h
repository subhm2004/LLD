#ifndef PLAY_STRATEGY_HPP
#define PLAY_STRATEGY_HPP
#include<iostream>
#include "../models/Song.h"
#include "../models/Playlist.h"

using namespace std;

class PlayStrategy {
public:
    virtual ~PlayStrategy() {}
    virtual void setPlaylist(Playlist* playlist) = 0;
    virtual Song* next() = 0;
    virtual bool hasNext() = 0;
    virtual Song* previous() = 0;
    virtual bool hasPrevious() = 0;
    virtual void addToNext(Song* song) {}
};

#endif // PLAY_STRATEGY_HPP
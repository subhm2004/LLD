// ============================================================================
//  strategies/PlayStrategy.h — STRATEGY interface (L8 pattern): play ORDER
// ----------------------------------------------------------------------------
//  "Agla/pichla gaana kaunsa?" — ye decision alag-alag ho sakta hai
//  (sequential, random, ya user-queue). Har tareeka ek concrete strategy.
//  Facade sirf PlayStrategy* jaanta hai; user strategy switch kar sakta
//  hai (SEQUENTIAL <-> RANDOM <-> CUSTOM_QUEUE) bina engine/facade ko chhede.
//
//  Contract: setPlaylist, next/hasNext, previous/hasPrevious, aur
//  addToNext (default empty — sirf CustomQueue isse override karta hai,
//  baaki strategies ke liye "queue" ka concept hai hi nahi).
//
//  ⭐ Ye Spotify/YouTube ke "shuffle" aur "repeat" buttons ka asli mechanism
//  hai — ek strategy object swap karo, playback order badal jaata hai!
// ============================================================================
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
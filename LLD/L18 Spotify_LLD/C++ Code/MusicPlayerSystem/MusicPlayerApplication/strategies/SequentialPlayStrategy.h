// ============================================================================
//  strategies/SequentialPlayStrategy.h — Concrete Strategy #1: ek-ke-baad-ek
// ----------------------------------------------------------------------------
//  Sabse simple play order — songs ko playlist ke ORDER me chalao (0,1,2...).
//  State bas ek currentIndex hai; next() index++, previous() index--.
//  Yahi normal "play" mode hai (shuffle off).
// ============================================================================
#ifndef SEQUENTIAL_PLAY_STRATEGY_HPP
#define SEQUENTIAL_PLAY_STRATEGY_HPP

#include<iostream>
#include "../models/Playlist.h"
#include "PlayStrategy.h"

using namespace std;

class SequentialPlayStrategy : public PlayStrategy {
private:
    Playlist* currentPlaylist;
    int currentIndex;
public:
    SequentialPlayStrategy() {
        currentPlaylist = nullptr;
        currentIndex = -1;
    }

    void setPlaylist(Playlist* playlist) override {
        currentPlaylist = playlist;
        currentIndex = -1;
    }

    // Aage koi song bacha hai? (agla index playlist ke size ke andar ho)
    bool hasNext() override {
        return ((currentIndex + 1) < currentPlaylist->getSize());
    }

    // Agla song do aur cursor aage badhao
    Song* next() override {
        if (!currentPlaylist || currentPlaylist->getSize() == 0) {
            throw runtime_error("No playlist loaded or playlist is empty.");
        }
        currentIndex = currentIndex + 1;
        return currentPlaylist->getSongs()[currentIndex];
    }

    // BUG FIX: pehle ye `(currentIndex - 1 > 0)` tha — off-by-one!
    // Iska matlab currentIndex > 1, yaani index 0 (PEHLA song) pe kabhi
    // wapas nahi ja sakte the. Sahi condition: currentIndex - 1 >= 0
    // (yaani currentIndex >= 1) — tab index 0 tak previous ja sakta hai.
    bool hasPrevious() override {
        return (currentIndex - 1 >= 0);
    }

    // Pichla song do aur cursor peeche karo
    Song* previous() override {
        if (!currentPlaylist || currentPlaylist->getSize() == 0) {
            throw runtime_error("No playlist loaded or playlist is empty.");
        }
        currentIndex = currentIndex - 1;
        return currentPlaylist->getSongs()[currentIndex];
    }
};

#endif // SEQUENTIAL_PLAY_STRATEGY_HPP
// ============================================================================
//  strategies/CustomQueueStrategy.h — Concrete Strategy #3: user QUEUE ➕
// ----------------------------------------------------------------------------
//  "Play next" wala feature — user manually songs queue karta hai, wo pehle
//  chalte hain, phir normal sequential. Spotify ka "Add to Queue" jaisa!
//
//  HYBRID logic (2 modes ek saath):
//    - nextQueue (queue) me kuch hai? -> wahi pehle bajao (FIFO)
//    - queue khaali? -> nextSequential() (normal order me chalo)
//  addToNext() se user queue me daalta hai. prevStack se previous track.
//
//  ⭐ SMART: jab queued song bajta hai, currentIndex ko us song ki playlist
//  position se SYNC karta hai (loop se dhundhta hai) — taaki queue khatam
//  hone ke baad sequential wahin se continue ho, na ki purane index se.
// ============================================================================
# ifndef CUSTOM_QUEUE_STRATEGY_HPP
# define CUSTOM_QUEUE_STRATEGY_HPP
#include<iostream>
#include "../models/Playlist.h"
#include "PlayStrategy.h"

class CustomQueueStrategy : public PlayStrategy {
private:
    Playlist* currentPlaylist;
    int currentIndex;
    queue<Song*> nextQueue;
    stack<Song*> prevStack;

    Song* nextSequential() {
        if (currentPlaylist->getSize() == 0) {
            throw runtime_error("Playlist is empty.");
        }
        currentIndex = currentIndex + 1;
        return currentPlaylist->getSongs()[currentIndex];
    }

    Song* previousSequential() {
        if (currentPlaylist->getSize() == 0) {
            throw runtime_error("Playlist is empty.");
        }
        currentIndex = currentIndex - 1;
        return currentPlaylist->getSongs()[currentIndex];
    }

public:
    CustomQueueStrategy() {
        currentPlaylist = nullptr;
        currentIndex = -1;
    }

    void setPlaylist(Playlist* playlist) override {
        currentPlaylist = playlist;
        currentIndex = -1;
        while (!nextQueue.empty()) {
            nextQueue.pop();
        }
        while(!prevStack.empty()) {
            prevStack.pop();
        }
    }

    bool hasNext() override {
        return ((currentIndex + 1) < currentPlaylist->getSize());
    }

    Song* next() override {
        if (!currentPlaylist || currentPlaylist->getSize() == 0) {
            throw runtime_error("No playlist loaded or playlist is empty.");
        }

        if (!nextQueue.empty()) {
            Song* s = nextQueue.front();
            nextQueue.pop();
            prevStack.push(s);

            // update index to match queued song
            auto& list = currentPlaylist->getSongs();
            for (int i = 0; i < (int)list.size(); ++i) {
                if (list[i] == s) {
                    currentIndex = i;
                    break;
                }
            }
            return s;
        }

        // Otherwise sequential
        return nextSequential();
    }

    // BUG FIX: pehle `(currentIndex - 1 > 0)` tha (off-by-one) — index 0
    // wala pehla song previous se reachable nahi tha. Sahi: >= 0.
    // (SequentialPlayStrategy me bhi yahi bug tha, wahan bhi fix kiya.)
    bool hasPrevious() override {
        return (currentIndex - 1 >= 0);
    }

    Song* previous() override {
        if (!currentPlaylist || currentPlaylist->getSize() == 0) {
            throw runtime_error("No playlist loaded or playlist is empty.");
        }

        if (!prevStack.empty()) {
            Song* s = prevStack.top();
            prevStack.pop();

            // update index to match stacked song
            auto& list = currentPlaylist->getSongs();
            for (int i = 0; i < (int)list.size(); ++i) {
                if (list[i] == s) {
                    currentIndex = i;
                    break;
                }
            }
            return s;
        }

        // Otherwise sequential
        return previousSequential();
    }

    void addToNext(Song* song) override {
        if (!song) {
            throw runtime_error("Cannot enqueue null song.");
        }
        nextQueue.push(song);
    }
};
# endif // CUSTOM_QUEUE_STRATEGY_HPP
// ============================================================================
//  strategies/RandomPlayStrategy.h — Concrete Strategy #2: SHUFFLE 🔀
// ----------------------------------------------------------------------------
//  Random order me play — par SMART way se, koi song REPEAT nahi hota jab
//  tak sab na chal jaayein (Spotify shuffle jaisa). Do data structures:
//    remainingSongs (vector) -> jo abhi tak nahi chale (inme se random pick)
//    history (stack)         -> jo chal chuke (previous ke liye — LIFO)
//
//  ⭐ SWAP-AND-POP trick (next() me): random song pick karke usse O(1) me
//  remove karne ke liye — usse last element ke saath swap karo, phir
//  pop_back(). Vector se beech ka element hatana normally O(n) hota (sab
//  shift), par order matter nahi karta yahan (random hai!) to swap-pop O(1).
//
//  previous() history stack se aata hai (jo abhi bajaya wahi wapas) — isliye
//  ye "true previous" hai, phir se random nahi.
// ============================================================================
#ifndef RANDOM_PLAY_STRATEGY_HPP
#define RANDOM_PLAY_STRATEGY_HPP
#include<iostream>
#include "../models/Playlist.h"
#include "PlayStrategy.h"

using namespace std;

class RandomPlayStrategy : public PlayStrategy {
private:
    Playlist* currentPlaylist;
    vector<Song*> remainingSongs; 
    stack<Song*> history; 

public:
    RandomPlayStrategy() {
        currentPlaylist = nullptr;
        srand((unsigned)time(nullptr));
    }

    void setPlaylist(Playlist* playlist) override {
        currentPlaylist = playlist;
        if (!currentPlaylist || currentPlaylist->getSize() == 0) return;

        remainingSongs = currentPlaylist->getSongs();
        history = stack<Song*>(); 
    }

    bool hasNext() override {
        return currentPlaylist && !remainingSongs.empty();
    }

    // Next in Loop
    Song* next() override {
        if (!currentPlaylist || currentPlaylist->getSize() == 0) {
            throw runtime_error("No playlist loaded or playlist is empty.");
        }
        if (remainingSongs.empty()) {
            throw runtime_error("No songs left to play");
        }

        // Random index pick karo remaining songs me se
        int idx = rand() % remainingSongs.size();
        Song* selectedSong = remainingSongs[idx];

        // SWAP-AND-POP: selected ko last ke saath swap, phir pop_back —
        // O(1) me remove (order matter nahi karta kyunki random hai)
        swap(remainingSongs[idx], remainingSongs.back());
        remainingSongs.pop_back();

        history.push(selectedSong); // previous() ke liye yaad rakho
        return selectedSong;
    }

    bool hasPrevious() override {
        return history.size() > 0;
    }

    Song* previous() override {
        if (history.empty()) {
            throw std::runtime_error("No previous song available.");
        }

        Song* song = history.top();
        history.pop();
        return song;
    }
};
    
#endif // RANDOM_PLAY_STRATEGY_HPP
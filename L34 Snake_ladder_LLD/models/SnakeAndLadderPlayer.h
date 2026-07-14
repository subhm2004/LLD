// ============================================================================
//  models/SnakeAndLadderPlayer.h  —  Ek khiladi: naam + board pe uski jagah
// ----------------------------------------------------------------------------
//  Simple data class. Player khud koi chaal nahi chalta, koi faisla nahi leta —
//  wo bas apni HAALAT (state) rakhta hai. Chaal chalwana engine ka kaam hai,
//  aur wo chaal valid hai ya nahi ye Rules ka. Har class ka ek hi kaam. ✅
//
//  ⭐ `position_ = 0` se shuru hota hai — 1 se nahi!
//     Kyun? Kyunki 0 ka matlab hai "abhi board pe utra hi nahi". Board ke ghar
//     1 se 100 tak hain. Pehla pasa 3 aaya? 0 + 3 = 3 pe pahunch gaye.
//
//     Agar 1 se shuru karte, to pehli chaal me 1 + 3 = 4 pe pahunchte — yaani
//     ek ghar extra mil jaata, aur poora khel ek ghar aage khisak jaata. 🤔
//     Chhota sa detail, par asli khel se match karna zaroori hai.
// ============================================================================
#ifndef SNAKE_LADDER_LLD_MODELS_PLAYER_H
#define SNAKE_LADDER_LLD_MODELS_PLAYER_H

#include <bits/stdc++.h>
using namespace std;

namespace snake_ladder_lld {

class SnakeAndLadderPlayer {
private:
    int playerId_;
    string name_;
    int position_;   // 0 = board pe utra nahi. 1..boardSize = ghar ka number.
    int score_;      // kitne game jeeta

public:
    // `std::move(name)` — string ki COPY nahi banti, uska andar ka data seedha
    // "cheen" (move) liya jaata hai. Param by-value liya hai isi liye safe hai.
    SnakeAndLadderPlayer(int playerId, string name)
        : playerId_(playerId), name_(std::move(name)), position_(0), score_(0) {}

    // ✅ `getPlayerId()` add kiya — pehle `playerId_` field bana to tha, par use
    // kabhi kisi ne padha hi nahi! Compiler bhi shikayat kar raha tha:
    //     "warning: private field 'playerId_' is not used"
    //
    // Bug to nahi tha, par ek adhoora field tha. Ab kam se kam padha ja sakta
    // hai. (Doosra option: field hi hata dete. Par id rakhna aage kaam aayega —
    // multi-game scoreboard, save/load, network play... isi liye rakha.)
    //
    // 📌 Compiler warnings ko kabhi ignore mat karo. Wo aksar adhoore kaam ka
    //    ishara hoti hain — aur kabhi-kabhi asli bug ka.
    int getPlayerId() const { return playerId_; }

    string getName() const { return name_; }
    int getPosition() const { return position_; }
    void setPosition(int position) { position_ = position; }

    int getScore() const { return score_; }
    void incrementScore() { score_++; }
    // ⚠ `score_` badhta to hai (jeetne pe), par kabhi print nahi hota — main.cpp
    // ek hi game khelta hai. Ye aage ke liye rakha hai (jab "best of 3" jaisa
    // multi-round support aayega). Abhi ke liye dead code hai — bug nahi, bas
    // notice karne layak.
};

}

#endif

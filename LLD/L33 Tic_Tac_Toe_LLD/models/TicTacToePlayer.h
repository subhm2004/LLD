// ============================================================================
//  models/TicTacToePlayer.h  —  Ek khiladi: id, naam, apna Symbol, aur score
// ----------------------------------------------------------------------------
//  Simple data class hai — koi game logic nahi. Player khud koi move nahi karta,
//  koi faisla nahi leta. Wo bas "kaun khel raha hai" ka record hai.
//
//  Move karwana TicTacToeGame ka kaam hai, aur valid hai ya nahi ye Rules ka.
//  Har class ka ek hi kaam — SINGLE RESPONSIBILITY PRINCIPLE.
//
//  ⭐ OWNERSHIP — is file ki sabse important baat:
//    Player apne Symbol ka MAALIK hai. Constructor me `Symbol*` leta hai, aur
//    destructor me use `delete` kar deta hai.
//
//    Ab dhyaan se dekho — wahi Symbol pointer Board ki grid me BHI pada hota
//    hai (jahan-jahan player ne move kiya). To kya Board bhi use delete karega?
//    NAHI. ✅ Board sirf pointers "rakhta" hai, unka maalik nahi hai. Board ka
//    destructor sirf apne `emptyCell_` ko delete karta hai — jo usne khud banaya
//    tha. Baaki symbols ko haath nahi lagata.
//
//    📌 GOLDEN RULE: **jo BANATA hai, wahi DELETE karta hai.** Baaki sab sirf
//    "dekhte" hain. Isi ek rule se 90% memory bugs bach jaate hain.
//
//    ⚠ Isi ka ulta pehlu: agar Player pehle delete ho gaya aur Board baad me
//    grid padhne ki koshish kare, to wo DANGLING pointers padhega -> crash.
//    main.cpp me order sahi hai (game pehle, players baad me), par ye nazuk hai.
//    Behtar: `shared_ptr<Symbol>` use karo — tab lifetime apne aap sambhal jaati.
// ============================================================================
#ifndef TICTACTOE_LLD_MODELS_TICTACTOEPLAYER_H
#define TICTACTOE_LLD_MODELS_TICTACTOEPLAYER_H

#include <bits/stdc++.h>

#include "Symbol.h"

using namespace std;

namespace tictactoe_lld {

class TicTacToePlayer {
public:
    // `std::move(name)` — string ki COPY nahi banti, uska andar ka data seedha
    // "cheen" (move) liya jaata hai. Param by-value liya hai isi liye ye safe
    // hai. Chhoti optimization hai, par bade strings pe fark padta hai.
    TicTacToePlayer(int playerId, string name, Symbol *symbol)
        : playerId_(playerId), name_(std::move(name)), symbol_(symbol), score_(0) {}

    // Player apne Symbol ka maalik hai -> wahi use delete karega. (Upar wala
    // ownership note padho — Board ise delete NAHI karta.)
    ~TicTacToePlayer() { delete symbol_; }

    int getPlayerId() const { return playerId_; }
    string getName() const { return name_; }

    // ⚠ Ye function apna andar ka pointer bahar de deta hai — yaani bahar wala
    // code us Symbol ko chhu sakta hai. Yahan safe hai kyunki Symbol IMMUTABLE
    // hai (koi setter nahi), to koi use bigaad hi nahi sakta. Agar Symbol me
    // setter hota, to ye getter ek khula darwaza ban jaata.
    Symbol *getSymbol() const { return symbol_; }

    int getScore() const { return score_; }

    // ⚠ Score badhta to hai, par kabhi print nahi hota! main.cpp ek hi game
    // khelta hai aur `getScore()` kabhi bulata hi nahi. Ye field aage ke liye
    // rakha gaya hai — jab multi-round support aayega ("best of 3"), tab kaam
    // aayega. Abhi ke liye ye dead code hai — koi bug nahi, bas notice karne
    // layak hai.
    void incrementScore() { score_++; }

private:
    int playerId_;
    string name_;
    Symbol *symbol_;   // is player ka nishaan (X ya O) — iska maalik yahi hai
    int score_;        // kitne game jeeta (abhi use nahi hota)
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_MODELS_TICTACTOEPLAYER_H

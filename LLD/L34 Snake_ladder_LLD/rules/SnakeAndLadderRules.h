// ============================================================================
//  rules/SnakeAndLadderRules.h  —  STRATEGY: game ke NIYAM, engine se alag
// ----------------------------------------------------------------------------
//  Teen sawaal, teen function:
//     1. isValidMove()         -> "kya ye chaal chali ja sakti hai?"
//     2. calculateNewPosition() -> "chalne ke BAAD banda kahan pahuncha?"
//     3. checkWinCondition()   -> "kya jeet gaya?"
//
//  Engine (`SnakeAndLadderGame`) me ye niyam kahin likhe NAHI hain. Wo bas
//  `rules_->...` puchta hai aur jawab maan leta hai. Naye niyam chahiye? Ek nayi
//  class banao — engine ki ek line bhi nahi badlegi:
//
//     FastSnakeAndLadderRules -> overshoot pe bhi jeet (exact roll ki zaroorat nahi)
//     BounceBackRules         -> 100 se aage gaye? Bounce hoke peeche aao
//                                 (98 pe ho, 5 aaya -> 100 pe jaake 3 wapas -> 97)
//     DoubleSixRules          -> 6 aaya to ek aur chaal milegi 🎲
//
//  ⚠ PAR — L33 Tic-Tac-Toe wali hi kami yahan bhi hai:
//    `SnakeAndLadderGame` ka constructor rules ko HARDCODE karta hai:
//        rules_(new StandardSnakeAndLadderRules())
//    Yaani upar likhe saare variants theoretical hain — unhe plug karne ka raasta
//    hi nahi. Strategy pattern aadha laga hai.
//    FIX hota: rules ko constructor me INJECT karo (dependency injection), aur
//    Factory decide kare kaunse rules bhejne hain. (Bilkul waise hi jaise L33 me
//    fix kiya tha.) Ise abhi chhod raha hu kyunki L34 ka focus Bridge pe hai —
//    par ye ek accha exercise hai, khud try karo.
//
//  ============================================================================
//   ⭐ SNAKE & LADDER KA SABSE MAZEDAAR NIYAM — "EXACT ROLL CHAHIYE"
//  ----------------------------------------------------------------------------
//   Tum 97 pe ho. Board 100 tak hai. Jeetne ke liye THEEK 3 chahiye.
//      3 aaya -> 100 -> JEET! 🎉
//      4 aaya -> 101? Board pe hai hi nahi -> chaal REJECT, wahi ke wahi raho
//      5 aaya -> wahi kahani
//
//   Yaani aakhri ke kuch ghar sabse tadpaane wale hote hain — 97 pe atke ho aur
//   6-6 aata ja raha hai. 😤 Yahi is khel ka maza hai.
//
//   Code me ye ek line hai: `(currentPos + diceValue) <= boardSize`
//   Bas. Ek chhoti si shart, aur poora khel ka mizaaj badal jaata hai.
//  ============================================================================
#ifndef SNAKE_LADDER_LLD_RULES_RULES_H
#define SNAKE_LADDER_LLD_RULES_RULES_H

#include <bits/stdc++.h>

#include "../core/Board.h"

using namespace std;

namespace snake_ladder_lld {

// ---- STRATEGY INTERFACE ----------------------------------------------------
class SnakeAndLadderRules {
public:
    virtual bool isValidMove(int currentPos, int diceValue, int boardSize) = 0;
    virtual int calculateNewPosition(int currentPos, int diceValue, Board *board) = 0;
    virtual bool checkWinCondition(int position, int boardSize) = 0;

    // Virtual destructor — engine `SnakeAndLadderRules*` me rakhta hai aur
    // destructor me `delete rules_` karta hai (base pointer se!). Iske bina
    // derived ka destructor chalta hi nahi.
    virtual ~SnakeAndLadderRules() {}
};

// ---- CONCRETE: aam Snake & Ladder ke niyam ---------------------------------
class StandardSnakeAndLadderRules : public SnakeAndLadderRules {
public:
    // ---- 1. Chaal valid hai? -----------------------------------------------
    // ⭐ "EXACT ROLL" wala niyam yahi ek line hai (upar detail me padho).
    // 97 pe ho aur 5 aaya? 102 board se bahar -> chaal reject.
    bool isValidMove(int currentPos, int diceValue, int boardSize) override {
        return (currentPos + diceValue) <= boardSize;
    }

    // ---- 2. Chalne ke baad kahan pahuncha? ---------------------------------
    int calculateNewPosition(int currentPos, int diceValue, Board *board) override {
        // Pehle seedha pasa ke hisaab se aage badho
        int newPos = currentPos + diceValue;

        // Ab dekho — us ghar pe kuch hai kya? (saanp ya seedhi)
        BoardEntity *entity = board->getEntity(newPos);

        // ⭐ YE LINE POLYMORPHISM KA KHOOBSURAT UDAHARAN HAI:
        //    Kuch nahi mila (nullptr) -> wahi ruko
        //    Kuch mila                -> uske `end` pe chale jao
        //
        //    Aur dhyaan do — yahan `if (saanp) ... else if (seedhi) ...` NAHI hai!
        //    Kyunki dono ka kaam BILKUL EK HI hai: "start pe aao, end pe jao."
        //    Saanp ka end neeche hai, seedhi ka upar — bas itna hi farak, aur wo
        //    farak DATA me hai, CODE me nahi.
        //
        //    Isi liye BoardEntity base class banayi thi. Ek line, dono cases. 🎯
        return entity == nullptr ? newPos : entity->getEnd();
    }

    // ---- 3. Jeet gaya? -----------------------------------------------------
    // `==` hai, `>=` nahi — aur yahi sahi hai! `isValidMove` pehle hi 100 se
    // aage jaana rok chuka hai, to position kabhi 100 se zyada ho hi nahi sakti.
    // Dono checks milke "exact roll" wala niyam poora karte hain.
    bool checkWinCondition(int position, int boardSize) override { return position == boardSize; }

    // 💡 Ek chhupa hua khatra jo ab FIX hai:
    //    Agar aakhri ghar (100) pe koi SAANP hota, to banda 100 pe pahunchta,
    //    saanp use neeche patak deta, aur `checkWinCondition` kabhi true hota
    //    hi nahi -> game HAMESHA chalta rehta (infinite loop)! 💀
    //    (Test kiya tha: 4 second me 16 lakh lines, koi vijeta nahi.)
    //
    //    ✅ Ab `Board::addBoardEntity()` aisi entity ko board pe chadhne hi nahi
    //    deta (`start < size_` wali shart). Aakhri ghar hamesha khaali rehta hai.
    //    📌 Notice: bug RULES me dikha, par fix BOARD me hua — kyunki asli galti
    //       wahan thi ki galat board ban hi kaise gaya. Bug ko uski JAD pe theek
    //       karo, jahan wo dikha wahan nahi.
};

}

#endif

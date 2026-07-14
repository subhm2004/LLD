// ============================================================================
//  factories/SnakeAndLadderGameFactory.h  —  FACTORY PATTERN (Creational)
// ----------------------------------------------------------------------------
//  Factory = "object banane ki poori jhanjhat ek jagah rakh do."
//
//  Ek game banane me kitne kadam hain? Dekho:
//     1. Board banao (sahi dimension ka)
//     2. Sahi setup strategy chuno
//     3. Board ko us strategy se bharo (Bridge ke through)
//     4. Strategy delete karo (ab uska kaam khatam)
//     5. Dice banao
//     6. Sab kuch Game me wire karo
//
//  Chhe kadam! Kya main.cpp ko ye sab pata hona chahiye? Bilkul nahi.
//  Client bas itna likhta hai:
//        game = SnakeAndLadderGameFactory::createStandardGame();
//  Bas. Ek line. Baaki sab factory ne sambhal liya. ✅
//
//  ⭐ TEEN alag-alag "recipe" — teen alag functions:
//     createStandardGame()  -> classic 10×10, fixed positions (koi param nahi!)
//     createRandomGame()    -> size + difficulty se random board
//     createCustomGame()    -> caller apni strategy khud bana ke bhejta hai
//
//  ⚠ OWNERSHIP KA SABSE ZAROORI SAWAL — `strategy` kaun delete karega?
//
//     `createStandardGame` / `createRandomGame` -> factory KHUD strategy banati
//        hai, to WAHI delete karti hai. Saaf.
//
//     `createCustomGame` -> strategy BAHAR se aati hai (main.cpp banata hai),
//        to factory use delete NAHI karti. main.cpp khud karta hai.
//
//     ⭐ Aur ye SAFE kyun hai? Kyunki `Board::setupBoard(strategy)` strategy ka
//     pointer STORE nahi karta — bas use karke chhod deta hai (BoardSetupBridge.h
//     dekho). Agar Board use sambhal ke rakhta, to main.cpp ka `delete strategy`
//     ek DANGLING pointer bana deta aur baad me crash hota. 💀
//
//     📌 Raw pointers me ownership hamesha aisi hi ulajhi hui hoti hai — "kaun
//     banata hai, kaun delete karta hai" har baar sochna padta hai. `unique_ptr`
//     use karte to ye poora paragraph likhne ki zaroorat hi na padti: signature
//     khud bata deta ki ownership kiske paas ja rahi hai. Yahi uska asli fayda hai.
// ============================================================================
#ifndef SNAKE_LADDER_LLD_FACTORIES_GAMEFACTORY_H
#define SNAKE_LADDER_LLD_FACTORIES_GAMEFACTORY_H

#include <bits/stdc++.h>

#include "../core/BoardSetupBridge.h"
#include "../core/SnakeAndLadderGame.h"
#include "../strategies/BoardSetupStrategyImpl.h"

using namespace std;

namespace snake_ladder_lld {

class SnakeAndLadderGameFactory {
private:
    static const int DICE_FACES = 6;

    // ========================================================================
    //  ✅ FINAL CHECK — "kya ye board jeeta bhi ja sakta hai?"
    // ========================================================================
    //  Board bhar gaya. Har entity alag-alag valid hai (addBoardEntity ne check
    //  kiya). Par kya POORA board sahi hai? Ye alag sawaal hai!
    //
    //  🐛 Ek asli case jo test me pakda:
    //     4×4 board (16 cells), 8 saanp maange. Saanp lage 10..15 pe —
    //     har ek bilkul valid! Par 16 pe pahunchne ke liye 10..15 me se kisi
    //     ghar pe khada hona zaroori tha, aur wo saare saanp the. 💀
    //     Koi kabhi jeet hi nahi sakta -> game me INFINITE LOOP. 🔁
    //
    //  Isi liye ye check zaroori hai — aur ye ek CENTRAL jagah pe hona chahiye
    //  jahan se HAR game guzarta ho. Factory se behtar jagah kya hogi? ✅
    //
    //  📌 Ye layered validation ka accha udaharan hai:
    //       Layer 1: `Board::addBoardEntity()` -> har ENTITY alag-alag check
    //       Layer 2: `ensureWinnable()` (yahan) -> POORA BOARD ek saath check
    //     Dono zaroori hain. Layer 1 layer 2 ki jagah nahi le sakta — kyunki
    //     kuch galtiyan sirf poore system me hi dikhti hain, tukdon me nahi.
    static void ensureWinnable(Board *board) {
        if (!board->isGoalReachable(DICE_FACES)) {
            delete board;   // apna banaya hua board saaf karo, warna leak 💧
            throw runtime_error(
                "Ye board jeeta hi nahi ja sakta! Saanp/seedhi ki jamavat aisi hai ki "
                "aakhri ghar tak pahunchne ka koi raasta hi nahi bachta. "
                "Kam saanp daalo, ya bada board lo.");
        }
    }

public:
    // ---- 1. STANDARD — classic 10×10 board -------------------------------
    // Koi parameter nahi! Kyunki standard board me kuch chunne ko hai hi nahi —
    // 10×10, 10 saanp, 11 seedhi, sab fixed. Bas maango aur mil jaayega.
    static SnakeAndLadderGame *createStandardGame() {
        Board *board = new Board(10);                                  // 100 cells
        BoardSetupStrategy *strategy = new StandardBoardSetupStrategy();

        board->setupBoard(strategy);   // ⭐ BRIDGE — board ko strategy se bharo

        delete strategy;   // kaam khatam. Board ne ise sambhal ke nahi rakha,
                           // isi liye ye delete bilkul safe hai.

        ensureWinnable(board);   // ✅ classic board hamesha pass hoga, par check
                                 //    to sab pe lagna chahiye — bina bhed-bhaav.

        return new SnakeAndLadderGame(board, new Dice(DICE_FACES));
        // Ab `board` aur `dice` ka maalik Game ban gaya — wahi unhe delete karega.
    }

    // ---- 2. RANDOM — difficulty ke hisaab se ------------------------------
    static SnakeAndLadderGame *createRandomGame(int boardSize, Difficulty difficulty) {
        // ⭐ Board ka constructor khud `dimension < 4` pe throw karta hai — to
        // yahan alag se check karne ki zaroorat nahi. Galat size andar jaayegi
        // hi nahi. (Aur main.cpp usko try/catch me pakad leta hai.)
        //
        // 💡 Pehle chhota board ek asli bug tha: 3×3 = 9 cells pe strategies ka
        // `rand() % (9 - 10)` NEGATIVE ho jaata tha -> board ke BAHAR saanp lag
        // jaate the, ya infinite loop ho jaata tha. Ab jad se band. ✅
        Board *board = new Board(boardSize);
        BoardSetupStrategy *strategy = new RandomBoardSetupStrategy(difficulty);

        board->setupBoard(strategy);
        delete strategy;

        ensureWinnable(board);   // ✅ HARD difficulty me saanp bahut hote hain —
                                 //    chhote board pe wo goal ko block kar sakte
                                 //    hain. Ye check use pakad lega.

        return new SnakeAndLadderGame(board, new Dice(DICE_FACES));
    }

    // ---- 3. CUSTOM — caller apni strategy bhejta hai ----------------------
    // ⭐ Ye sabse "khula" (flexible) hai — caller KOI BHI strategy bhej sakta
    // hai, chahe wo aaj bani ho ya kal. Factory ko usse koi matlab nahi, wo bas
    // `BoardSetupStrategy*` interface se baat karti hai.
    //
    // ⚠ `strategy` yahan DELETE NAHI hota — kyunki wo factory ne banaya hi nahi!
    // Jo banata hai wahi delete karta hai. main.cpp ne banaya, wahi karega.
    static SnakeAndLadderGame *createCustomGame(int boardSize, BoardSetupStrategy *strategy) {
        if (strategy == nullptr) {
            throw invalid_argument("createCustomGame: strategy nullptr nahi ho sakti");
        }

        Board *board = new Board(boardSize);   // galat size pe khud throw karega
        board->setupBoard(strategy);

        ensureWinnable(board);   // ✅ SABSE ZAROORI YAHAN — custom mode me user
                                 //    kuch bhi daal sakta hai. Yahi wo raasta tha
                                 //    jisse 4×4 board pe 8 saanp daal ke game
                                 //    hang kar diya ja sakta tha. Ab band. 🛡

        return new SnakeAndLadderGame(board, new Dice(DICE_FACES));
    }
};

}

#endif

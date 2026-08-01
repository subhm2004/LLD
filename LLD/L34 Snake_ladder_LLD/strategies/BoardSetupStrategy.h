// ============================================================================
//  strategies/BoardSetupStrategy.h  —  STRATEGY PATTERN (Behavioral)
// ----------------------------------------------------------------------------
//  "Board pe saanp-seedhi KAISE lagayein" — ye ek alag zimmedari hai, aur uske
//  kai tareeke ho sakte hain. Har tareeka apni class me.
//
//     StandardBoardSetupStrategy    -> classic 10×10 board, fixed positions
//                                      (wahi jo asli board pe chhapa hota hai)
//     RandomBoardSetupStrategy      -> difficulty ke hisaab se random placement
//     CustomCountBoardSetupStrategy -> user batata hai kitne/kahan
//
//  ⭐ Board ko ye pata hi NAHI ki uspe entities kaise aayi. Wo bas bolta hai:
//        board->setupBoard(strategy);   // "tu jaane, bhar de mujhe"
//     Aur strategy `board->addBoardEntity(...)` bulati rehti hai.
//
//  Naya setup chahiye? (jaise "MirrorSetup" jahan saanp-seedhi symmetric hon,
//  ya "NoSnakesSetup" bachchon ke liye) — ek nayi class banao, bas. Board,
//  Game, Rules — kuch bhi chhune ki zaroorat nahi. OPEN/CLOSED PRINCIPLE. ✅
//
//  📌 STRATEGY vs BRIDGE — dono yahan hain, confuse mat hona:
//     - STRATEGY: "ek kaam ke kai tareeke, runtime pe chuno" — yahi ye file hai.
//     - BRIDGE:   "abstraction (Board) aur implementation (Setup) ko alag rakho,
//                  taaki dono ALAG-ALAG badh sakein" — BoardSetupBridge.h dekho.
//     Sach ye hai ki code dono me lagbhag EK JAISA dikhta hai. Farak NIYAT ka
//     hai (kyun kiya), structure ka nahi. Interview me ye poocha jaata hai —
//     BoardSetupBridge.h me maine detail me likha hai.
// ============================================================================
#ifndef SNAKE_LADDER_LLD_STRATEGIES_BOARDSETUPSTRATEGY_H
#define SNAKE_LADDER_LLD_STRATEGIES_BOARDSETUPSTRATEGY_H

#include <bits/stdc++.h>

#include "../enums/Difficulty.h"
#include "../models/BoardEntity.h"

using namespace std;

namespace snake_ladder_lld {

// Forward declaration — hum sirf `Board*` (pointer) use kar rahe hain, uske
// members nahi. Isi liye poori definition ki zaroorat nahi.
// ⚠ Isi wajah se in classes ke `setupBoard()` ki BODY yahan nahi likhi ja sakti
// (`board->getBoardSize()` chahiye hoga!) — wo BoardSetupStrategyImpl.h me hai,
// jahan Board poora include ho chuka hota hai.
class Board;

// ---- STRATEGY INTERFACE ----------------------------------------------------
class BoardSetupStrategy {
public:
    // Ek hi kaam: "ye board le, aur ispe apne hisaab se entities laga de."
    virtual void setupBoard(Board *board) = 0;

    // Virtual destructor — caller `BoardSetupStrategy*` (base pointer) me rakh
    // ke `delete` karta hai (factory aur main.cpp me dekho). Iske bina derived
    // ka destructor chalta hi nahi -> undefined behavior.
    virtual ~BoardSetupStrategy() {}
};

// ---- 1. RANDOM: difficulty ke hisaab se saanp/seedhi ka anupaat -------------
class RandomBoardSetupStrategy : public BoardSetupStrategy {
private:
    Difficulty difficulty_;

    // Asli kaam yahi karta hai. `snakeProbability` = har entity ke saanp banne
    // ka chance (0.3 = 30%). Private hai — bahar wale ko iski zaroorat nahi.
    void setupWithProbability(Board *board, double snakeProbability);

public:
    explicit RandomBoardSetupStrategy(Difficulty difficulty) : difficulty_(difficulty) {}
    void setupBoard(Board *board) override;
};

// ---- 2. CUSTOM: user batata hai kitne (ya kahan-kahan) ----------------------
//  ⭐ Ye class DO modes me kaam karti hai — `randomPositions_` flag se decide:
//
//     randomPositions_ = true   -> "5 saanp aur 3 seedhi chahiye, jagah tu chun"
//                                  (numSnakes_ / numLadders_ use hote hain)
//     randomPositions_ = false  -> "main khud batata hu kahan-kahan"
//                                  (snakePositions_ / ladderPositions_ use hote hain)
//
//  ⚠ Ek class do kaam kar rahi hai — ye thoda "code smell" hai. Saaf design me
//  ye DO alag strategies hoti:
//       RandomCountBoardSetupStrategy   (ginti do, jagah main chunta hu)
//       ExactPositionBoardSetupStrategy (jagah tum batao)
//  Tab `randomPositions_` flag ki zaroorat hi na padti, aur har class me sirf
//  wahi fields hoti jo use hoti hain. Abhi random-mode me `snakePositions_`
//  khaali pada rehta hai, aur exact-mode me `numSnakes_` bekaar hai. 🤷
//  📌 Boolean flag jo behavior badalta hai = aksar do classes ka ishara hota hai.
class CustomCountBoardSetupStrategy : public BoardSetupStrategy {
private:
    int numSnakes_;                            // random-mode me use hota hai
    int numLadders_;                           // random-mode me use hota hai
    bool randomPositions_;                     // kaunsa mode?
    vector<pair<int, int>> snakePositions_;    // exact-mode me use hota hai
    vector<pair<int, int>> ladderPositions_;   // exact-mode me use hota hai

public:
    CustomCountBoardSetupStrategy(int snakes, int ladders, bool randomPositions)
        : numSnakes_(snakes), numLadders_(ladders), randomPositions_(randomPositions) {}

    // Exact-mode me positions ek-ek karke daali jaati hain (main.cpp dekho).
    // ⚠ Yahan koi validation nahi — par zaroorat bhi nahi! Sab kuch
    // `Board::addBoardEntity()` me check hota hai, jo board ka ekmatr darwaza
    // hai. Ek jagah pehredaar, aur poora board mehfooz. ✅
    void addSnakePosition(int start, int end) { snakePositions_.push_back({start, end}); }
    void addLadderPosition(int start, int end) { ladderPositions_.push_back({start, end}); }

    void setupBoard(Board *board) override;
};

// ---- 3. STANDARD: classic 10×10 board, wahi jo bachpan me khela tha ---------
class StandardBoardSetupStrategy : public BoardSetupStrategy {
public:
    // Koi member variable nahi — ye class bilkul STATELESS hai. Bas ek hi kaam
    // karti hai: 21 hardcoded entities laga do. (L30 Flyweight yaad karo —
    // aisi stateless class ka ek hi object poore program me kaafi hota hai.)
    void setupBoard(Board *board) override;
};

}

#endif

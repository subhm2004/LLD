// ============================================================================
//  strategies/BoardSetupStrategyImpl.h  —  CONCRETE STRATEGIES ki asli body
// ----------------------------------------------------------------------------
//  Ye file `BoardSetupStrategy.h` me declare ki gayi classes ke functions ki
//  BODY rakhti hai. Alag file kyun?
//
//  Kyunki in functions ko `board->getBoardSize()`, `board->addBoardEntity()`
//  chahiye — yaani `Board` ki POORI definition. Par `BoardSetupStrategy.h` me
//  Board sirf forward-declared hai (`class Board;`), kyunki Board ko bhi
//  strategy chahiye! Classic murgi-anda (circular dependency).
//
//  Hal: declaration alag, definition alag.
//     BoardSetupStrategy.h     -> classes ka DHAANCHA (Board forward-declared)
//     BoardSetupStrategyImpl.h -> functions ki BODY  (Board poora include)
//
//  ⭐ Har function pe `inline` kyun laga hai?
//     Ye header file hai, aur agar ye 2 alag .cpp me include ho gayi, to har
//     function ki DO copy ban jaati -> linker "duplicate symbol" ka error deta.
//     `inline` kehta hai: "duplicate copies theek hain, ek chun lo." Header me
//     function body likhne ke liye ye zaroori hai.
//
//  ============================================================================
//   IS FILE ME 3 ASLI BUG THE — teeno test karke confirm kiye, teeno fix hue
//  ----------------------------------------------------------------------------
//   🐛 1. NEGATIVE MODULO: `rand() % (boardSize - 10)` — agar board 10 cells se
//         chhota hua to divisor NEGATIVE ho jaata. Natija: 9-cell board pe
//         "Snake: 10 -> 8" — board ke BAHAR ka saanp! 😵
//         ✅ FIX: `randomInRange(lo, hi)` helper banaya — hamesha safe.
//                Aur Board ab dimension >= 4 maangta hai.
//
//   🐛 2. INFINITE LOOP: `while (snakesAdded < numSnakes_)` — koi try limit hi
//         nahi thi! Board me jitni jagah hai usse ZYADA entities maang lo, aur
//         loop hamesha ke liye ghoomta rahega. 🔁
//         (Test kiya: 100-cell board pe 200 saanp maange -> game HANG.)
//         ✅ FIX: bounded retries. Nahi laga paye to warning de ke aage badho.
//
//   🐛 3. ADD KA NATIJA IGNORE: `addBoardEntity` ka return value dekha hi nahi
//         jaata tha (wo `void` tha!). Entity lagi ya reject hui — pata hi nahi
//         chalta tha.
//         ✅ FIX: ab wo `bool` lautata hai, aur hum uspe bharosa karte hain.
//  ============================================================================
#ifndef SNAKE_LADDER_LLD_STRATEGIES_BOARDSETUPSTRATEGYIMPL_H
#define SNAKE_LADDER_LLD_STRATEGIES_BOARDSETUPSTRATEGYIMPL_H

#include <bits/stdc++.h>

#include "../core/Board.h"
#include "BoardSetupStrategy.h"

using namespace std;

namespace snake_ladder_lld {

// ============================================================================
//  ✅ HELPER — [lo, hi] ke beech ek random number (dono inclusive)
// ----------------------------------------------------------------------------
//  Ye chhota sa function poori file ka sabse important fix hai. Kyun?
//
//  Purana code har jagah aisa likhta tha:
//      int start = rand() % (boardSize - 10) + 10;
//
//  Agar `boardSize` 10 se chhota hua, to `boardSize - 10` NEGATIVE ho jaata.
//  Aur C++ me negative se modulo lena... theek nahi chalta. 9-cell board pe
//  `rand() % -1` = 0 nikalta tha, to `start` hamesha 10 ban jaata — jabki board
//  hi 1..9 tak hai! Board ke BAHAR ka saanp. 😵 (Test karke dekha tha.)
//
//  Ab har jagah ye helper use hota hai. Range SAAF dikhti hai (`randomInRange(10, 99)`
//  padhne me turant samajh aata hai), aur `hi < lo` wala kharab case bhi guard
//  kar diya hai. Ek jagah sahi kar do, poori file surakshit.
//
//  📌 Jab ek hi ganda formula 4 jagah copy-paste ho, to wo ek function banne ki
//     bheekh maang raha hota hai. Aur aksar bug bhi wahi chhupa hota hai.
// ============================================================================
inline int randomInRange(int lo, int hi) {
    if (hi < lo) return lo;              // ulti range -> safe fallback
    return lo + rand() % (hi - lo + 1);  // divisor hamesha >= 1 -> kabhi crash nahi
}

// ============================================================================
//  RANDOM STRATEGY — difficulty ke hisaab se saanp/seedhi ka anupaat
// ============================================================================
inline void RandomBoardSetupStrategy::setupWithProbability(Board *board, double snakeProbability) {
    int boardSize = board->getBoardSize();

    // Kul entities = cells / 10. (100 cells -> 10 entities.) Difficulty se ye
    // GINTI nahi badalti — sirf saanp aur seedhi ka BATWARA badalta hai.
    int totalEntities = boardSize / 10;

    for (int i = 0; i < totalEntities; i++) {
        // 0.0 se 1.0 ke beech ek random number. Agar wo `snakeProbability` se
        // kam hai to saanp banao, warna seedhi.
        //   HARD (0.7) -> 70% baar `p < 0.7` sach hoga -> zyadatar saanp 🐍
        //   EASY (0.3) -> sirf 30% baar -> zyadatar seedhi 🪜
        double p = static_cast<double>(rand()) / RAND_MAX;
        bool wantSnake = (p < snakeProbability);

        // ---- ✅ BOUNDED RETRIES — 50 baar try karo, phir haar maan lo -------
        // Position pehle se bhari ho sakti hai, ya entity validation me reject
        // ho sakti hai. Isi liye kai baar try karte hain.
        //
        // ⭐ Par LIMIT ke saath (50). Bina limit ke, agar board bhar chuka ho to
        // ye loop HAMESHA ghoomta rahega. Ye is file ka doosra bada bug tha
        // (CustomCount me) — yahan pehle se limit thi, aur yahi sahi tareeka hai.
        for (int tries = 0; tries < 50; tries++) {
            BoardEntity *entity = nullptr;

            if (wantSnake) {
                // Saanp: munh [10, boardSize-1] pe (shuruaat me saanp nahi —
                // warna khel bahut nirash karne wala ho jaata 😄), aur poonch
                // [1, start-1] pe (yaani hamesha NEECHE le jaata hai).
                int start = randomInRange(10, boardSize - 1);
                int end   = randomInRange(1, start - 1);
                entity = new Snake(start, end);
            } else {
                // Seedhi: neeche se shuru [1, boardSize-10] (upar chadhne ki
                // jagah bache), aur [start+1, boardSize-1] pe khatam.
                // `boardSize-1` isliye — seedhi seedha JEET ke ghar tak nahi le
                // ja sakti, warna "exact roll chahiye" wala niyam bekaar ho jaata.
                int start = randomInRange(1, boardSize - 10);
                int end   = randomInRange(start + 1, boardSize - 1);
                entity = new Ladder(start, end);
            }

            // ⭐ `addBoardEntity` ab `bool` lautata hai (pehle `void` tha!).
            // Laga to break, warna dobara try. Aur agar reject hua, to wo KHUD
            // hi entity ko delete kar deta hai — yahan `delete` likhne ki
            // zaroorat nahi. (Pehle ye leak hota tha!)
            if (board->addBoardEntity(entity)) break;
        }
    }
}

inline void RandomBoardSetupStrategy::setupBoard(Board *board) {
    // Difficulty -> probability. Bas ek chhota sa mapping.
    if (difficulty_ == EASY) setupWithProbability(board, 0.3);        // zyadatar seedhi 🪜
    else if (difficulty_ == MEDIUM) setupWithProbability(board, 0.5); // aadha-aadha
    else setupWithProbability(board, 0.7);                            // zyadatar saanp 🐍
}

// ============================================================================
//  CUSTOM STRATEGY — do mode: ginti se (random jagah) ya exact positions se
// ============================================================================
inline void CustomCountBoardSetupStrategy::setupBoard(Board *board) {
    int boardSize = board->getBoardSize();

    if (randomPositions_) {
        // ====================================================================
        //  MODE 1 — "itne saanp chahiye, jagah tu chun"
        // ====================================================================
        //
        //  🐛 PURANA CODE — YAHAN INFINITE LOOP THA:
        //
        //      int snakesAdded = 0;
        //      while (snakesAdded < numSnakes_) {        // 👈 koi limit NAHI!
        //          int start = rand() % (boardSize - 10) + 10;
        //          ...
        //          if (board->canAddEntity(start)) { ...; snakesAdded++; }
        //      }
        //
        //  Agar `numSnakes_` board me available jagah se ZYADA hai, to
        //  `snakesAdded` kabhi `numSnakes_` tak pahunchega hi nahi — aur loop
        //  HAMESHA ghoomta rahega. 🔁 Program hang. Koi error nahi, koi message
        //  nahi. Bas... chup-chaap atak jaata hai.
        //
        //  Test karke dekha tha: 100-cell board (saanp ki jagah sirf ~90),
        //  200 saanp maango -> game HANG ho gaya. 💀
        //  Aur 9-cell board pe 2 seedhi maango -> bhi HANG (kyunki chhote board
        //  pe ladder start hamesha 1 aata tha, to doosri kabhi lagti hi nahi).
        //
        //  ✅ FIX: har entity ke liye BOUNDED tries (50). Nahi laga paye to
        //  warning de ke aage badho. Program CHALTA rahe — hang na ho.
        //
        //  📌 SABAK: koi bhi `while` loop jo "kismat" pe bharosa karta ho
        //     (random try karo jab tak safal na ho), usko HAMESHA limit do.
        //     "Kabhi to lag hi jaayega" — nahi, kabhi nahi lagega. Aur tab
        //     program hang ho jaayega, aur tumhe pata bhi nahi chalega ki kyun.

        int snakesAdded = 0;
        for (int i = 0; i < numSnakes_; i++) {
            bool placed = false;
            for (int tries = 0; tries < 50 && !placed; tries++) {
                int start = randomInRange(10, boardSize - 1);
                int end   = randomInRange(1, start - 1);
                placed = board->addBoardEntity(new Snake(start, end));
            }
            if (placed) snakesAdded++;
        }
        if (snakesAdded < numSnakes_) {
            cout << "Note: " << numSnakes_ << " me se sirf " << snakesAdded
                 << " saanp laga paye (board me itni jagah nahi hai)." << endl;
        }

        int laddersAdded = 0;
        for (int i = 0; i < numLadders_; i++) {
            bool placed = false;
            for (int tries = 0; tries < 50 && !placed; tries++) {
                int start = randomInRange(1, boardSize - 10);
                int end   = randomInRange(start + 1, boardSize - 1);
                placed = board->addBoardEntity(new Ladder(start, end));
            }
            if (placed) laddersAdded++;
        }
        if (laddersAdded < numLadders_) {
            cout << "Note: " << numLadders_ << " me se sirf " << laddersAdded
                 << " seedhi laga paye (board me itni jagah nahi hai)." << endl;
        }

    } else {
        // ====================================================================
        //  MODE 2 — "main khud batata hu kahan-kahan"
        // ====================================================================
        //
        //  🐛 PURANE CODE ME KOI VALIDATION NAHI THI. Kuch bhi chal jaata tha:
        //     - `Snake(5, 90)`   -> saanp jo UPAR le jaaye! 😵 (test kiya, chalta tha)
        //     - `Snake(100, 5)`  -> JEETNE ke ghar pe saanp! 💀 Ab koi kabhi jeet
        //                           hi nahi sakta — 100 pe pahuncho, saanp neeche.
        //                           Game me INFINITE LOOP. (Test kiya: 4 second
        //                           me 16 LAKH lines output, koi vijeta nahi.)
        //     - Board ke bahar ki positions
        //
        //  ✅ Ab saari validation `Board::addBoardEntity()` me hai — board ka
        //  ekmatr darwaza. Galat entity wahin reject ho jaati hai (aur delete
        //  bhi ho jaati hai, to leak bhi nahi hota).
        //
        //  ⭐ Yahan hum bas `false` pe user ko BATA dete hain ki kaunsi entity
        //  reject hui aur kyun — chup-chaap ignore nahi karte. Galti chupni
        //  nahi chahiye; user ko pata hona chahiye ki uska saanp board pe nahi hai.
        for (auto &p : snakePositions_) {
            if (!board->addBoardEntity(new Snake(p.first, p.second))) {
                cout << "Saanp (" << p.first << " -> " << p.second << ") reject hua — "
                     << "galat position (saanp NEECHE le jaana chahiye, board ke andar, "
                     << "aur aakhri ghar pe nahi)." << endl;
            }
        }
        for (auto &p : ladderPositions_) {
            if (!board->addBoardEntity(new Ladder(p.first, p.second))) {
                cout << "Seedhi (" << p.first << " -> " << p.second << ") reject hui — "
                     << "galat position (seedhi UPAR le jaani chahiye, board ke andar, "
                     << "aur aakhri ghar pe nahi)." << endl;
            }
        }
    }
}

// ============================================================================
//  STANDARD STRATEGY — classic 10×10 board (wahi jo bachpan me khela tha)
// ============================================================================
inline void StandardBoardSetupStrategy::setupBoard(Board *board) {
    // Ye setup SIRF 100-cell board ke liye bana hai — positions hardcoded hain.
    // Chhote board pe Snake(99, 54) ka koi matlab hi nahi.
    if (board->getBoardSize() != 100) {
        cout << "Standard setup only works for 10x10 board!" << endl;
        return;
    }

    // 10 saanp 🐍 — sab NEECHE le jaate hain (end < start)
    // Notice: koi saanp 100 pe nahi hai — warna game jeeta hi nahi ja sakta!
    // (Wahi cheez jo ab `addBoardEntity` bhi enforce karta hai.)
    board->addBoardEntity(new Snake(99, 54)); board->addBoardEntity(new Snake(95, 75));
    board->addBoardEntity(new Snake(92, 88)); board->addBoardEntity(new Snake(89, 68));
    board->addBoardEntity(new Snake(74, 53)); board->addBoardEntity(new Snake(64, 60));
    board->addBoardEntity(new Snake(62, 19)); board->addBoardEntity(new Snake(49, 11));
    board->addBoardEntity(new Snake(46, 25)); board->addBoardEntity(new Snake(16, 6));

    // 11 seedhi 🪜 — sab UPAR le jaati hain (end > start)
    // Sabse mazedaar: 28 -> 84 (ek hi chaal me 56 ghar ki chhalang!) 🚀
    // Aur koi seedhi 100 tak nahi jaati — "exact roll chahiye" niyam bacha rehta.
    board->addBoardEntity(new Ladder(2, 38)); board->addBoardEntity(new Ladder(7, 14));
    board->addBoardEntity(new Ladder(8, 31)); board->addBoardEntity(new Ladder(15, 26));
    board->addBoardEntity(new Ladder(21, 42)); board->addBoardEntity(new Ladder(28, 84));
    board->addBoardEntity(new Ladder(36, 44)); board->addBoardEntity(new Ladder(51, 67));
    board->addBoardEntity(new Ladder(71, 91)); board->addBoardEntity(new Ladder(78, 98));
    board->addBoardEntity(new Ladder(87, 94));

    // 💡 Ye saari positions `addBoardEntity` ki validation pass karti hain —
    // sab board ke andar hain, disha sahi hai, aur koi duplicate nahi. Agar koi
    // galti se `Snake(50, 90)` likh deta, to ab wo chup-chaap reject ho jaata
    // (aur delete bhi). Pehle wo board pe lag jaata aur khel bigaad deta. 😅
}

}

#endif

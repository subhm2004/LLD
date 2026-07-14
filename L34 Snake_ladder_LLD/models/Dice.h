// ============================================================================
//  models/Dice.h  —  Pasa (dice): roll() pe 1..faces ke beech ek random number
// ----------------------------------------------------------------------------
//  Poori class 2 line ki hai, par ismein ek BAHUT chalak bug chhupa tha. Padho.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  🐛 PURANA BUG — "Random" board har baar BILKUL SAME banta tha!          │
//  │                                                                          │
//  │  Purana constructor aisa tha:                                            │
//  │      explicit Dice(int faces) : faces_(faces) {                          │
//  │          srand(time(0));          // 👈 YAHAN seed ho raha tha           │
//  │      }                                                                   │
//  │                                                                          │
//  │  Dekhne me theek lagta hai na? Par Factory ka order dekho:               │
//  │                                                                          │
//  │      Board *board = new Board(boardSize);                                │
//  │      board->setupBoard(strategy);   // 👈 (1) yahan rand() chalta hai    │
//  │      ...                                                                 │
//  │      return new SnakeAndLadderGame(board, new Dice(6));                  │
//  │                                     // 👆 (2) srand() AB chalta hai!     │
//  │                                                                          │
//  │  Yaani `rand()` pehle chal raha tha, aur `srand()` BAAD me! 💀           │
//  │  Bina seed ke `rand()` hamesha default seed (= 1) use karta hai. Natija: │
//  │  board setup ke saare "random" numbers HAR RUN ME BILKUL WAHI aate the.  │
//  │                                                                          │
//  │  Test karke dekha tha — teen alag run, teeno me EXACTLY same board:      │
//  │      Snake: 89->34   Snake: 50->21   Snake: 18->2   Ladder: 1->19  ...   │
//  │  "Random with Difficulty" naam ka feature, jo bilkul bhi random nahi. 😅 │
//  │                                                                          │
//  │  ✅ FIX: `srand()` ko yahan se HATA diya. Ab wo main.cpp me, program ke  │
//  │  sabse shuru me, EK BAAR chalta hai — kisi bhi rand() se pehle.          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 DO SABAK is bug se:
//
//   1. **`srand()` poore PROGRAM me sirf EK BAAR chalna chahiye — shuruaat me.**
//      Wo ek GLOBAL cheez ko seed karta hai. Usko kisi class ke constructor me
//      chhupa dena galat hai: us class ko pata hi nahi ki uske banne se pehle
//      kisne rand() bulaya tha. Global state ko chhupe kone me set karna hamesha
//      dukh deta hai.
//
//   2. **Dice ka kaam pasa phenkna hai, poore program ka RNG seed karna nahi.**
//      Ye SINGLE RESPONSIBILITY ka ullanghan tha — Dice ne ek aisi zimmedari
//      utha li jo uski thi hi nahi. Aur bug bhi wahi se aaya. Aksar aisa hi hota
//      hai: galat jagah rakha hua code, aage chal ke bug banta hai.
// ============================================================================
#ifndef SNAKE_LADDER_LLD_MODELS_DICE_H
#define SNAKE_LADDER_LLD_MODELS_DICE_H

#include <bits/stdc++.h>
using namespace std;

namespace snake_ladder_lld {

class Dice {
private:
    int faces_;   // kitne pehlu (aam pasa = 6)

public:
    explicit Dice(int faces) : faces_(faces) {
        // ✅ Guard — `faces_ = 0` hota to `rand() % 0` = DIVIDE BY ZERO = crash 💥
        // Abhi hamesha `new Dice(6)` hi banta hai, par public constructor ko
        // apni suraksha khud karni chahiye. Kal koi `new Dice(0)` likh dega.
        if (faces_ < 1) {
            throw invalid_argument("Dice ke kam se kam 1 face hone chahiye");
        }

        // ⭐ Yahan pehle `srand(time(0))` tha — HATA diya (upar wala bug padho).
        // Ab seeding main.cpp me, program ke shuru me, ek hi baar hoti hai.
    }

    // 1 se `faces_` tak ek random number.
    // `% faces_` -> [0, faces_-1] deta hai, aur `+ 1` se [1, faces_] ban jaata hai.
    // (Pasa 0 nahi dikhata, isi liye +1.)
    int roll() { return (rand() % faces_) + 1; }

    // 💡 Waise `rand()` khud bhi purana tareeka hai — uska distribution thoda
    // tirchha hota hai (modulo bias) aur wo thread-safe bhi nahi. Aaj kal
    // <random> use karte hain:
    //     mt19937 gen{random_device{}()};
    //     uniform_int_distribution<int> dist{1, faces_};
    //     int roll() { return dist(gen); }
    // Yahan `rand()` isi liye rakha hai kyunki board setup bhi wahi use karta
    // hai — dono ko ek saath badalna hota. Padhne me simple rahe, isi liye chhoda.
};

}

#endif

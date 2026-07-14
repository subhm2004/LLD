// ============================================================================
//  observers/SnakeAndLadderConsoleNotifier.h  —  CONCRETE OBSERVER
// ----------------------------------------------------------------------------
//  Poori class 3 line ki hai. Aur yahi Observer pattern ki KHOOBSURTI hai —
//  ek naya observer banana itna hi sasta hona chahiye.
//
//  Isi tarah aur observers ban sakte hain, bina Game ko chhue:
//
//     class GameReplayRecorder : public IObserver {
//         vector<string> moves_;
//     public:
//         void update(const string &m) override { moves_.push_back(m); }
//         void replay() { for (auto &m : moves_) cout << m << "\n"; }
//     };
//
//     class FileLogger : public IObserver {
//         ofstream file_{"game.log"};
//     public:
//         void update(const string &m) override { file_ << m << "\n"; }
//     };
//
//  Dono ko `game->addObserver(...)` me daal do — bas. Game ko pata bhi nahi
//  chalega ki uske events ab file me ja rahe hain.
//
//  ⚠ Ek gadbad — DO tarah ka output ek saath:
//     `SnakeAndLadderGame::play()` me DONO cheezein hoti hain:
//         cout << "Rolled: " << diceValue << endl;             // seedha cout 👈
//         notify(name + " played. New Position : " + ...);     // observer se 👈
//
//     Isi liye console pe milta-julta output do baar dikhta hai. Thoda ganda hai.
//
//     Saaf design: game KABHI seedha `cout` na kare — SAB kuch observers ke
//     through jaaye. Tab "console pe dikhana" bhi bas ek observer hota, aur game
//     poori tarah UI se aazaad ho jaata. Tab wo headless bhi chal sakta (server
//     pe, ya automated test me — jahan koi console hai hi nahi).
//
//     Abhi game console se BANDHA hua hai. Ye ek accha refactoring exercise hai:
//     saare `cout` uthao aur ek `ConsoleDisplayObserver` me daal do. 🎯
// ============================================================================
#ifndef SNAKE_LADDER_LLD_OBSERVERS_CONSOLENOTIFIER_H
#define SNAKE_LADDER_LLD_OBSERVERS_CONSOLENOTIFIER_H

#include <bits/stdc++.h>

#include "IObserver.h"

using namespace std;

namespace snake_ladder_lld {

class SnakeAndLadderConsoleNotifier : public IObserver {
public:
    // `override` -> compiler check karega ki signature base se EXACTLY match
    // karti hai. Typo hui to compile-time error milega, chupke se ek naya
    // function banne ke bajaye. Chhota keyword, bahut se ghante bachata hai.
    void update(const string &message) override {
        cout << "[NOTIFICATION] " << message << endl;
    }
};

}

#endif

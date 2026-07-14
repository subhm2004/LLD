// ============================================================================
//  observers/ConsoleNotifier.h  —  CONCRETE OBSERVER: events console pe print
// ----------------------------------------------------------------------------
//  Poori class 3 line ki hai. Aur yahi Observer pattern ki KHOOBSURTI hai —
//  ek naya observer banana itna hi sasta hona chahiye.
//
//  Isi tarah aur observers ban sakte the, bina Game ko chhue:
//
//     class FileLogger : public IObserver {
//         ofstream file_{"game.log"};
//     public:
//         void update(const string &message) override { file_ << message << "\n"; }
//     };
//
//     class SilentObserver : public IObserver {
//     public:
//         void update(const string &) override { /* kuch mat karo */ }
//     };
//
//  Dono ko `game->addObserver(...)` me daal do — bas. TicTacToeGame ko pata bhi
//  nahi chalega ki uske events ab file me ja rahe hain ya kahin nahi ja rahe.
//
//  ⚠ Ek chhota confusion — do tarah ka output ek saath:
//     TicTacToeGame::play() me DONO cheezein hoti hain:
//         cout << currentPlayer->getName() << " wins!\n";   // seedha cout 👈
//         notify(currentPlayer->getName() + " wins!");      // observer se 👈
//     Isi liye console pe do line dikhti hain:
//         Aditya wins!
//         [Notification] Aditya wins!
//
//     Yaani game seedha bhi print kar raha hai AUR observer se bhi. Thoda
//     ganda hai. Saaf design: game KABHI seedha `cout` na kare — sab kuch
//     observers ke through jaaye. Tab display bhi ek observer hoti, aur game
//     poori tarah "UI se aazaad" ho jaata (headless bhi chal sakta — server pe,
//     ya test me). Abhi game console se BANDHA hua hai.
// ============================================================================
#ifndef TICTACTOE_LLD_OBSERVERS_CONSOLENOTIFIER_H
#define TICTACTOE_LLD_OBSERVERS_CONSOLENOTIFIER_H

#include <bits/stdc++.h>

#include "IObserver.h"

using namespace std;

namespace tictactoe_lld {

class ConsoleNotifier : public IObserver {
public:
    // `override` -> compiler check karega ki signature base se EXACTLY match
    // karti hai. Typo hui (jaise `Update`, ya `string` bina `const&`), to
    // compile-time error milega — chupke se ek naya function banne ke bajaye.
    // Ye chhota keyword bahut se ghante bachata hai. Hamesha lagao.
    void update(const string &message) override {
        // "[Notification]" prefix isi liye taaki game ke apne output se alag
        // dikhe (upar wala note padho).
        cout << "[Notification] " << message << "\n";
    }
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_OBSERVERS_CONSOLENOTIFIER_H

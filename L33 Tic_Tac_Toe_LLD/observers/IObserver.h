// ============================================================================
//  observers/IObserver.h  —  OBSERVER PATTERN (Behavioral) ka interface
// ----------------------------------------------------------------------------
//  Observer = "publisher–subscriber". Ek SUBJECT ke paas kuch hota rehta hai,
//  aur kai OBSERVERS ko har ghatna ki khabar chahiye. Subject ko ye jaanne ki
//  zaroorat NAHI ki observers kaun hain — bas unki list rakhta hai aur event pe
//  sabko `update()` bol deta hai.
//
//  Is project me mapping:
//     Subject  -> TicTacToeGame   (game start hua, move hua, koi jeeta, draw hua)
//     Observer -> ConsoleNotifier (events ko console pe print karta hai)
//
//  ⭐ FAYDA — Game ko ye pata hi nahi ki notification KAISE deliver hogi:
//     Aaj console print hai. Kal:
//        FileLogger      -> events ko log file me likhe
//        WebSocketPusher -> browser pe live game bheje
//        ReplayRecorder  -> saare moves save kare (baad me replay ke liye)
//        AnalyticsTracker-> stats bheje (kitne game khele gaye)
//     Ek bhi naya observer add karo — TicTacToeGame ka code EK LINE bhi nahi
//     badlega. Bas `game->addObserver(naya)` karo aur ho gaya.
//
//  Aur ULTA bhi sach hai — koi observer add hi mat karo, game phir bhi chalega!
//  `notify()` ka loop khaali list pe chalega, bas. Game observers pe DEPEND nahi
//  karta. Yahi asli LOOSE COUPLING hai — dono taraf se aazadi.
//
//  📌 Naam me `I` kyun? (IObserver, "I" for Interface)
//     Ye C#/Java se aayi convention hai. C++ me utni common nahi (log `Observer`
//     hi likhte hain — L31 Splitwise me wahi tha). Par galat kuch nahi — bas
//     ek nazar me pata chal jaata hai ki ye interface hai, concrete class nahi.
// ============================================================================
#ifndef TICTACTOE_LLD_OBSERVERS_IOBSERVER_H
#define TICTACTOE_LLD_OBSERVERS_IOBSERVER_H

#include <bits/stdc++.h>

using namespace std;

namespace tictactoe_lld {

class IObserver {
public:
    // Subject (Game) event hone par ye call karta hai. Message me event ki
    // detail hoti hai ("Aditya played (0,2)", "Aditya wins!", waghairah).
    //
    // ⚠ Ek design kami: message ek plain STRING hai. Yaani observer ko sirf ek
    // insaan-ke-padhne-layak line milti hai — koi STRUCTURED data nahi (kaunsa
    // player? kaunsa cell? event ka type kya tha?).
    //
    // Ek ReplayRecorder banana ho to? Usko (row, col, player) chahiye — par usko
    // milti hai "Aditya played (0,2)" string, jise wo PARSE kare! 🤮 Bhadda hai.
    //
    // Behtar: ek `GameEvent` struct bhejo —
    //     struct GameEvent { EventType type; string playerName; int row, col; };
    //     virtual void update(const GameEvent &event) = 0;
    // Tab har observer apne kaam ka data seedha nikaal le — string todne ki
    // zaroorat hi nahi. String sirf DISPLAY ke liye hoti hai, DATA ke liye nahi.
    virtual void update(const string &message) = 0;

    // Virtual destructor — main.cpp `IObserver *notifier` rakhta hai par asal
    // object `ConsoleNotifier` hai, aur `delete notifier` BASE pointer se hota
    // hai. Iske bina derived ka destructor chalta hi nahi -> undefined behavior.
    virtual ~IObserver() {}
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_OBSERVERS_IOBSERVER_H

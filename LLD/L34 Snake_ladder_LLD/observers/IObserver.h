// ============================================================================
//  observers/IObserver.h  —  OBSERVER PATTERN (Behavioral) ka interface
// ----------------------------------------------------------------------------
//  Observer = "publisher–subscriber". Ek SUBJECT ke paas ghatnaayein hoti rehti
//  hain, aur kai OBSERVERS ko unki khabar chahiye. Subject ko ye jaanne ki
//  zaroorat NAHI ki observers kaun hain — bas unki list rakhta hai aur event pe
//  sabko `update()` bol deta hai.
//
//  Is project me:
//     Subject  -> SnakeAndLadderGame            (game shuru, chaal chali, jeet)
//     Observer -> SnakeAndLadderConsoleNotifier (console pe print karta hai)
//
//  ⭐ Game ko ye pata hi nahi ki notification KAISE deliver hogi. Aaj console
//  print hai. Kal `GameReplayRecorder` (saari chaalein save karo), ya
//  `WebSocketPusher` (browser pe live dikhao), ya `StatsCollector` (kitni baar
//  saanp ne kaata). Ek bhi naya observer add karo — Game ka code EK LINE bhi
//  nahi badlega. Bas `game->addObserver(naya)` aur ho gaya. ✅
//
//  Aur ULTA bhi sach hai: koi observer add hi mat karo, game phir bhi chalega!
//  `notify()` ka loop khaali list pe chalega, bas. Dono taraf se aazadi = asli
//  LOOSE COUPLING.
//
//  ⚠ Ek design kami (L33 me bhi yahi thi):
//    Message ek plain STRING hai — koi STRUCTURED data nahi. Observer ko sirf
//    "Aditya played. New Position : 42" milta hai. Ab agar koi `StatsCollector`
//    banana ho jo ginna chahe ki kitni baar saanp ne kaata — usko ye string
//    PARSE karni padegi! 🤮
//
//    Behtar hota:
//        struct GameEvent { EventType type; string player; int from, to; };
//        virtual void update(const GameEvent &event) = 0;
//    Tab har observer apne kaam ka data seedha nikaal leta.
//    📌 String DISPLAY ke liye hoti hai, DATA ke liye nahi.
// ============================================================================
#ifndef SNAKE_LADDER_LLD_OBSERVERS_IOBSERVER_H
#define SNAKE_LADDER_LLD_OBSERVERS_IOBSERVER_H

#include <bits/stdc++.h>
using namespace std;

namespace snake_ladder_lld {

class IObserver {
public:
    // Subject (Game) har event pe ye bulata hai.
    virtual void update(const string &message) = 0;

    // Virtual destructor — main.cpp `IObserver *notifier` rakhta hai par asal
    // object `SnakeAndLadderConsoleNotifier` hai, aur `delete notifier` BASE
    // pointer se hota hai. Iske bina derived ka destructor chalta hi nahi.
    virtual ~IObserver() {}
};

}

#endif

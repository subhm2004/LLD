// core/ChessSystemDemo.h — End-to-end demo runner: users queue karta hai,
// matchmaking, sample moves, check/checkmate, aur chat dikhata hai. main() isse call karta.
#ifndef CHESS_LLD_CORE_CHESSSYSTEMDEMO_H
#define CHESS_LLD_CORE_CHESSSYSTEMDEMO_H

#include <bits/stdc++.h>

#include "Match.h"

using namespace std;

namespace chess_lld {

class ChessSystemDemo {
public:
    static void demonstrateScholarsMate() {
        cout << "\n=== Scholar's Mate Demo (4-move checkmate) ===" << endl;
        User *aditya = new User("DEMO_1", "Aditya");
        User *rohit = new User("DEMO_2", "Rohit");
        Match *demoMatch = new Match("DEMO_MATCH", aditya, rohit);
        demoMatch->getBoard()->display();

        demoMatch->makeMove(Position(6, 4), Position(4, 4), aditya);
        demoMatch->makeMove(Position(1, 4), Position(3, 4), rohit);
        demoMatch->makeMove(Position(7, 5), Position(4, 2), aditya);
        demoMatch->makeMove(Position(0, 1), Position(2, 2), rohit);
        demoMatch->makeMove(Position(7, 3), Position(3, 7), aditya);
        demoMatch->makeMove(Position(0, 6), Position(2, 5), rohit);
        demoMatch->makeMove(Position(3, 7), Position(1, 5), aditya);

        aditya->send(new Message(aditya->getId(), "Good game!"));
        rohit->send(new Message(rohit->getId(), "Thanks, that was a quick one!"));

        delete demoMatch;
        delete aditya;
        delete rohit;
    }
};
}

#endif

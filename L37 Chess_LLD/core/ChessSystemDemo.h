// ============================================================================
//  core/ChessSystemDemo.h — SCHOLAR'S MATE demo (4-move checkmate! ♟️)
// ----------------------------------------------------------------------------
//  Scholar's Mate = chess ki sabse famous quick-win: Queen + Bishop milke
//  f7 (black ka sabse kamzor square) pe attack karte hain. Moves ka matlab:
//    1. White e2->e4  (6,4)->(4,4)   : king's pawn do kadam aage
//    2. Black e7->e5  (1,4)->(3,4)   : black bhi wahi jawab
//    3. White Bf1->c4 (7,5)->(4,2)   : bishop f7 pe nishana
//    4. Black Nb8->c6 (0,1)->(2,2)   : knight develop (khatra nahi dekha!)
//    5. White Qd1->h5 (7,3)->(3,7)   : queen bhi f7 pe nishana!
//    6. Black Ng8->f6 (0,6)->(2,5)   : knight se queen ko bhagane ki koshish...
//    7. White Qxf7#   (3,7)->(1,5)   : CHECKMATE! Queen f7 capture,
//                                      bishop c4 se protect kar raha hai.
//  (Yaad rakho: row 0 = rank 8, isliye coordinates ulte lagte hain!)
//  End me MEDIATOR chat demo bhi: dono players GG bolte hain Match ke through.
// ============================================================================
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

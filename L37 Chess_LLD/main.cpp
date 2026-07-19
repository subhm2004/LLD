// ============================================================================
//  main.cpp  —  ONLINE CHESS SYSTEM ka demo driver (L37 LLD project)
// ----------------------------------------------------------------------------
//  System kya karta hai: users matchmaking queue me aate hain -> score-based
//  pairing se match banta hai -> real chess khela jaata hai (har piece ke
//  legal moves, check/checkmate/stalemate detection) -> in-match chat +
//  score updates. Pura chess.com ka mini version!
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  SYSTEM KA FLOW:                                                        │
//  │                                                                          │
//  │  User ──requestMatch()──> GameManager (SINGLETON)                       │
//  │                              │ MatchingStrategy se opponent dhundo      │
//  │                              │ (score ±100 wala best match)             │
//  │                              ▼                                          │
//  │            mila? ──nahi──> waiting queue me daalo                       │
//  │              │haan                                                       │
//  │              ▼                                                          │
//  │            Match banao ──> Board setup (PieceFactory se 32 pieces)      │
//  │              │                                                          │
//  │   makeMove() loop:  turn check -> piece check -> ChessRules se          │
//  │   validate (legal move? king safe rahega?) -> board update ->           │
//  │   checkmate/stalemate/check detect -> turn switch                       │
//  │              │                                                          │
//  │   Chat: player.send() ──> Match (MEDIATOR) ──> opponent.receive()       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ============================================================================
//   IS PROJECT ME 4 DESIGN PATTERNS use hue hain:
//  ----------------------------------------------------------------------------
//   Pattern    | Kahan                        | Kya kaam
//   -----------+------------------------------+--------------------------------
//   Singleton  | core/GameManager.h           | Poore system ka EK coordinator
//              |                              |  (matches + waiting queue)
//   Strategy   | strategies/MatchingStrategy.h| Matchmaking ka algorithm
//              | rules/ChessRules.h           |  swap-able; rules engine bhi
//   Mediator   | models/User.h (interface) +  | Players seedha baat nahi karte —
//              |  core/Match.h (implement)    |  Match ke through chat hoti hai
//   Factory    | factories/PieceFactory.h     | PieceType+Color se sahi piece
//              |                              |  object banana (Board setup)
//
//   Demo 1: Scholar's Mate — 4-move checkmate ka pura khel (ChessSystemDemo)
//   Demo 2: GameManager — 3 users ki matchmaking (2 match, 1 waiting)
// ============================================================================
#include <bits/stdc++.h>

#include "core/ChessSystemDemo.h"
#include "core/GameManager.h"

using namespace std;
using namespace chess_lld;

int main() {
    cout << "=== Chess System with Design Patterns Demo ===" << endl;
    ChessSystemDemo::demonstrateScholarsMate();

    cout << "\n=== Game Manager Demo ===" << endl;
    GameManager *gm = GameManager::getInstance();
    User *saurav = new User("USER_1", "Saurav");
    User *manish = new User("USER_2", "Manish");
    User *abhishek = new User("USER_3", "Abhishek");

    cout << "\nUsers: " << saurav->toString() << ", " << manish->toString() << ", " << abhishek->toString() << endl;
    gm->requestMatch(saurav);
    gm->requestMatch(manish);
    gm->requestMatch(abhishek);
    gm->displayActiveMatches();

    delete saurav;
    delete manish;
    delete abhishek;
    delete GameManager::getInstance();
    return 0;
}

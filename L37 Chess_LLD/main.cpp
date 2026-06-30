// ============================================================================
//  main.cpp  —  Online Chess system ka demo driver
// ----------------------------------------------------------------------------
//  Users queue karo -> score-based matchmaking -> match -> legal moves (per-piece),
//  check/checkmate/stalemate detection, in-match chat, aur score update. Patterns:
//  Singleton (GameManager) + Strategy (rules/matchmaking) + Mediator (chat) + Factory.
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

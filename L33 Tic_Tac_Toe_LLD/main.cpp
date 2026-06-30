// ============================================================================
//  main.cpp  —  Tic Tac Toe game ka demo driver
// ----------------------------------------------------------------------------
//  Factory se game banao (configurable N x N board + players), turn-by-turn
//  moves karo, rules win/draw detect karte hain, observer events print karta hai.
//  Patterns: Strategy (rules) + Observer (events) + Factory.
// ============================================================================
#include <bits/stdc++.h>

#include "core/TicTacToeGameFactory.h"
#include "models/TicTacToePlayer.h"
#include "observers/ConsoleNotifier.h"

using namespace std;
using namespace tictactoe_lld;

int main() {
    cout << "=== TIC TAC TOE GAME ===\n";
    cout << "Enter board size (e.g., 3 for 3x3): ";

    int boardSize;
    cin >> boardSize;

    TicTacToeGame *game = TicTacToeGameFactory::createGame(STANDARD, boardSize);
    IObserver *notifier = new ConsoleNotifier();
    game->addObserver(notifier);

    TicTacToePlayer *player1 = new TicTacToePlayer(1, "Aditya", new Symbol('X'));
    TicTacToePlayer *player2 = new TicTacToePlayer(2, "Harshita", new Symbol('O'));
    game->addPlayer(player1);
    game->addPlayer(player2);

    game->play();

    delete game;
    delete player1;
    delete player2;
    delete notifier;
    return 0;
}

// ============================================================================
//  main.cpp  —  Snake & Ladder game ka demo driver
// ----------------------------------------------------------------------------
//  Factory se game banao (standard/random/custom board setup), multiplayer
//  turn-by-turn dice rolls, snakes/ladders apply, exact-roll win. Patterns:
//  Strategy + Bridge (board setup) + Factory + Observer.
// ============================================================================
#include <bits/stdc++.h>

#include "factories/SnakeAndLadderGameFactory.h"
#include "observers/SnakeAndLadderConsoleNotifier.h"

using namespace std;
using namespace snake_ladder_lld;

int main() {
    cout << "=== SNAKE AND LADDER GAME ===" << endl;
    SnakeAndLadderGame *game = nullptr;

    cout << "Choose game setup:\n1. Standard\n2. Random with Difficulty\n3. Custom" << endl;
    int choice;
    cin >> choice;

    if (choice == 1) {
        game = SnakeAndLadderGameFactory::createStandardGame();
    } else if (choice == 2) {
        int boardSize, diffChoice;
        cout << "Enter board size (e.g., 10): ";
        cin >> boardSize;
        cout << "Choose difficulty: 1.Easy 2.Medium 3.Hard : ";
        cin >> diffChoice;
        Difficulty difficulty = (diffChoice == 1) ? EASY : (diffChoice == 3) ? HARD : MEDIUM;
        game = SnakeAndLadderGameFactory::createRandomGame(boardSize, difficulty);
    } else if (choice == 3) {
        int boardSize;
        cout << "Enter board size (e.g., 10): ";
        cin >> boardSize;
        cout << "Choose custom setup type:\n1. Random placement using counts\n2. Exact positions" << endl;
        int customChoice;
        cin >> customChoice;

        if (customChoice == 1) {
            int snakes, ladders;
            cout << "Enter number of snakes: ";
            cin >> snakes;
            cout << "Enter number of ladders: ";
            cin >> ladders;
            BoardSetupStrategy *strategy = new CustomCountBoardSetupStrategy(snakes, ladders, true);
            game = SnakeAndLadderGameFactory::createCustomGame(boardSize, strategy);
            delete strategy;
        } else {
            int snakes, ladders;
            cout << "Enter number of snakes: ";
            cin >> snakes;
            cout << "Enter number of ladders: ";
            cin >> ladders;
            auto *strategy = new CustomCountBoardSetupStrategy(snakes, ladders, false);
            for (int i = 0; i < snakes; i++) {
                int start, end;
                cout << "Enter snake " << i + 1 << " start and end: ";
                cin >> start >> end;
                strategy->addSnakePosition(start, end);
            }
            for (int i = 0; i < ladders; i++) {
                int start, end;
                cout << "Enter ladder " << i + 1 << " start and end: ";
                cin >> start >> end;
                strategy->addLadderPosition(start, end);
            }
            game = SnakeAndLadderGameFactory::createCustomGame(boardSize, strategy);
            delete strategy;
        }
    }

    if (game == nullptr) {
        cout << "Invalid choice!" << endl;
        return 1;
    }

    IObserver *notifier = new SnakeAndLadderConsoleNotifier();
    game->addObserver(notifier);

    int numPlayers;
    cout << "Enter number of players: ";
    cin >> numPlayers;
    for (int i = 0; i < numPlayers; i++) {
        string name;
        cout << "Enter name for player " << (i + 1) << ": ";
        cin >> name;
        game->addPlayer(new SnakeAndLadderPlayer(i + 1, name));
    }

    game->play();
    delete game;
    delete notifier;
    return 0;
}

#ifndef SNAKE_LADDER_LLD_FACTORIES_GAMEFACTORY_H
#define SNAKE_LADDER_LLD_FACTORIES_GAMEFACTORY_H

#include <bits/stdc++.h>

#include "../core/BoardSetupBridge.h"
#include "../core/SnakeAndLadderGame.h"
#include "../strategies/BoardSetupStrategyImpl.h"

using namespace std;

namespace snake_ladder_lld {
class SnakeAndLadderGameFactory {
public:
    static SnakeAndLadderGame *createStandardGame() {
        Board *board = new Board(10);
        BoardSetupStrategy *strategy = new StandardBoardSetupStrategy();
        board->setupBoard(strategy);
        delete strategy;
        return new SnakeAndLadderGame(board, new Dice(6));
    }

    static SnakeAndLadderGame *createRandomGame(int boardSize, Difficulty difficulty) {
        Board *board = new Board(boardSize);
        BoardSetupStrategy *strategy = new RandomBoardSetupStrategy(difficulty);
        board->setupBoard(strategy);
        delete strategy;
        return new SnakeAndLadderGame(board, new Dice(6));
    }

    static SnakeAndLadderGame *createCustomGame(int boardSize, BoardSetupStrategy *strategy) {
        Board *board = new Board(boardSize);
        board->setupBoard(strategy);
        return new SnakeAndLadderGame(board, new Dice(6));
    }
};
}

#endif

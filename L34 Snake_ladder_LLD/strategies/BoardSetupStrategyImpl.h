// strategies/BoardSetupStrategyImpl.h — Concrete board-setup strategies:
// Standard (predefined), Random (difficulty-based), Custom (counts/positions).
// BoardSetupStrategy ko implement karte hain.
#ifndef SNAKE_LADDER_LLD_STRATEGIES_BOARDSETUPSTRATEGYIMPL_H
#define SNAKE_LADDER_LLD_STRATEGIES_BOARDSETUPSTRATEGYIMPL_H

#include <bits/stdc++.h>

#include "../core/Board.h"
#include "BoardSetupStrategy.h"

using namespace std;

namespace snake_ladder_lld {

inline void RandomBoardSetupStrategy::setupWithProbability(Board *board, double snakeProbability) {
    int boardSize = board->getBoardSize();
    int totalEntities = boardSize / 10;
    for (int i = 0; i < totalEntities; i++) {
        double p = static_cast<double>(rand()) / RAND_MAX;
        if (p < snakeProbability) {
            for (int tries = 0; tries < 50; tries++) {
                int start = rand() % (boardSize - 10) + 10;
                int end = rand() % (start - 1) + 1;
                if (board->canAddEntity(start)) {
                    board->addBoardEntity(new Snake(start, end));
                    break;
                }
            }
        } else {
            for (int tries = 0; tries < 50; tries++) {
                int start = rand() % (boardSize - 10) + 1;
                int end = rand() % (boardSize - start) + start + 1;
                if (board->canAddEntity(start) && end < boardSize) {
                    board->addBoardEntity(new Ladder(start, end));
                    break;
                }
            }
        }
    }
}

inline void RandomBoardSetupStrategy::setupBoard(Board *board) {
    if (difficulty_ == EASY) setupWithProbability(board, 0.3);
    else if (difficulty_ == MEDIUM) setupWithProbability(board, 0.5);
    else setupWithProbability(board, 0.7);
}

inline void CustomCountBoardSetupStrategy::setupBoard(Board *board) {
    int boardSize = board->getBoardSize();
    if (randomPositions_) {
        int snakesAdded = 0;
        while (snakesAdded < numSnakes_) {
            int start = rand() % (boardSize - 10) + 10;
            int end = rand() % (start - 1) + 1;
            if (board->canAddEntity(start)) {
                board->addBoardEntity(new Snake(start, end));
                snakesAdded++;
            }
        }
        int laddersAdded = 0;
        while (laddersAdded < numLadders_) {
            int start = rand() % (boardSize - 10) + 1;
            int end = rand() % (boardSize - start) + start + 1;
            if (board->canAddEntity(start) && end < boardSize) {
                board->addBoardEntity(new Ladder(start, end));
                laddersAdded++;
            }
        }
    } else {
        for (auto &p : snakePositions_) if (board->canAddEntity(p.first)) board->addBoardEntity(new Snake(p.first, p.second));
        for (auto &p : ladderPositions_) if (board->canAddEntity(p.first)) board->addBoardEntity(new Ladder(p.first, p.second));
    }
}

inline void StandardBoardSetupStrategy::setupBoard(Board *board) {
    if (board->getBoardSize() != 100) {
        cout << "Standard setup only works for 10x10 board!" << endl;
        return;
    }
    board->addBoardEntity(new Snake(99, 54)); board->addBoardEntity(new Snake(95, 75));
    board->addBoardEntity(new Snake(92, 88)); board->addBoardEntity(new Snake(89, 68));
    board->addBoardEntity(new Snake(74, 53)); board->addBoardEntity(new Snake(64, 60));
    board->addBoardEntity(new Snake(62, 19)); board->addBoardEntity(new Snake(49, 11));
    board->addBoardEntity(new Snake(46, 25)); board->addBoardEntity(new Snake(16, 6));
    board->addBoardEntity(new Ladder(2, 38)); board->addBoardEntity(new Ladder(7, 14));
    board->addBoardEntity(new Ladder(8, 31)); board->addBoardEntity(new Ladder(15, 26));
    board->addBoardEntity(new Ladder(21, 42)); board->addBoardEntity(new Ladder(28, 84));
    board->addBoardEntity(new Ladder(36, 44)); board->addBoardEntity(new Ladder(51, 67));
    board->addBoardEntity(new Ladder(71, 91)); board->addBoardEntity(new Ladder(78, 98));
    board->addBoardEntity(new Ladder(87, 94));
}

}

#endif

#ifndef SNAKE_LADDER_LLD_STRATEGIES_BOARDSETUPSTRATEGY_H
#define SNAKE_LADDER_LLD_STRATEGIES_BOARDSETUPSTRATEGY_H

#include <bits/stdc++.h>

#include "../enums/Difficulty.h"
#include "../models/BoardEntity.h"

using namespace std;

namespace snake_ladder_lld {
class Board;

class BoardSetupStrategy {
public:
    virtual void setupBoard(Board *board) = 0;
    virtual ~BoardSetupStrategy() {}
};

class RandomBoardSetupStrategy : public BoardSetupStrategy {
private:
    Difficulty difficulty_;
    void setupWithProbability(Board *board, double snakeProbability);

public:
    explicit RandomBoardSetupStrategy(Difficulty difficulty) : difficulty_(difficulty) {}
    void setupBoard(Board *board) override;
};

class CustomCountBoardSetupStrategy : public BoardSetupStrategy {
private:
    int numSnakes_;
    int numLadders_;
    bool randomPositions_;
    vector<pair<int, int>> snakePositions_;
    vector<pair<int, int>> ladderPositions_;

public:
    CustomCountBoardSetupStrategy(int snakes, int ladders, bool randomPositions)
        : numSnakes_(snakes), numLadders_(ladders), randomPositions_(randomPositions) {}

    void addSnakePosition(int start, int end) { snakePositions_.push_back({start, end}); }
    void addLadderPosition(int start, int end) { ladderPositions_.push_back({start, end}); }
    void setupBoard(Board *board) override;
};

class StandardBoardSetupStrategy : public BoardSetupStrategy {
public:
    void setupBoard(Board *board) override;
};
}

#endif

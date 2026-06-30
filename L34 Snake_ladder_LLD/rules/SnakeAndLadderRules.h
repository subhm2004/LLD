// rules/SnakeAndLadderRules.h — Rules engine: move valid hai? (EXACT roll se win
// chahiye — overshoot reject), aur win condition check. Game isse consult karta hai.
#ifndef SNAKE_LADDER_LLD_RULES_RULES_H
#define SNAKE_LADDER_LLD_RULES_RULES_H

#include <bits/stdc++.h>

#include "../core/Board.h"

using namespace std;

namespace snake_ladder_lld {
class SnakeAndLadderRules {
public:
    virtual bool isValidMove(int currentPos, int diceValue, int boardSize) = 0;
    virtual int calculateNewPosition(int currentPos, int diceValue, Board *board) = 0;
    virtual bool checkWinCondition(int position, int boardSize) = 0;
    virtual ~SnakeAndLadderRules() {}
};

class StandardSnakeAndLadderRules : public SnakeAndLadderRules {
public:
    bool isValidMove(int currentPos, int diceValue, int boardSize) override {
        return (currentPos + diceValue) <= boardSize;
    }
    int calculateNewPosition(int currentPos, int diceValue, Board *board) override {
        int newPos = currentPos + diceValue;
        BoardEntity *entity = board->getEntity(newPos);
        return entity == nullptr ? newPos : entity->getEnd();
    }
    bool checkWinCondition(int position, int boardSize) override { return position == boardSize; }
};
}

#endif

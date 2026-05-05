#ifndef TICTACTOE_LLD_CORE_TICTACTOEGAMEFACTORY_H
#define TICTACTOE_LLD_CORE_TICTACTOEGAMEFACTORY_H

#include <bits/stdc++.h>

#include "../enums/GameType.h"
#include "TicTacToeGame.h"

using namespace std;

namespace tictactoe_lld {

class TicTacToeGameFactory {
public:
    static TicTacToeGame *createGame(GameType gameType, int boardSize) {
        if (gameType == STANDARD) {
            return new TicTacToeGame(boardSize);
        }
        return nullptr;
    }
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_CORE_TICTACTOEGAMEFACTORY_H

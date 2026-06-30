// rules/TicTacToeRules.h — STRATEGY interface for game rules: move valid hai?
// koi jeeta? draw hua? Naye variants (timed, wildcard) iske naye implementations.
#ifndef TICTACTOE_LLD_RULES_TICTACTOERULES_H
#define TICTACTOE_LLD_RULES_TICTACTOERULES_H

#include <bits/stdc++.h>

#include "../models/Board.h"

using namespace std;

namespace tictactoe_lld {

class TicTacToeRules {
public:
    virtual bool isValidMove(Board *board, int row, int col) = 0;
    virtual bool checkWinCondition(Board *board, Symbol *symbol) = 0;
    virtual bool checkDrawCondition(Board *board) = 0;
    virtual ~TicTacToeRules() {}
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_RULES_TICTACTOERULES_H

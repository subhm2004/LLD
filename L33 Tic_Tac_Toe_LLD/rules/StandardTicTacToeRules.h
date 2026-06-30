// rules/StandardTicTacToeRules.h — Concrete rules: row/column/diagonal/anti-
// diagonal complete -> win; board full + no win -> draw. TicTacToeRules implement.
#ifndef TICTACTOE_LLD_RULES_STANDARDTICTACTOERULES_H
#define TICTACTOE_LLD_RULES_STANDARDTICTACTOERULES_H

#include <bits/stdc++.h>

#include "TicTacToeRules.h"

using namespace std;

namespace tictactoe_lld {

class StandardTicTacToeRules : public TicTacToeRules {
public:
    bool isValidMove(Board *board, int row, int col) override {
        return board->isCellEmpty(row, col);
    }

    bool checkWinCondition(Board *board, Symbol *symbol) override {
        int size = board->getSize();
        for (int i = 0; i < size; i++) {
            bool rowWin = true;
            for (int j = 0; j < size; j++) {
                if (board->getCell(i, j) != symbol) {
                    rowWin = false;
                    break;
                }
            }
            if (rowWin) {
                return true;
            }
        }

        for (int j = 0; j < size; j++) {
            bool colWin = true;
            for (int i = 0; i < size; i++) {
                if (board->getCell(i, j) != symbol) {
                    colWin = false;
                    break;
                }
            }
            if (colWin) {
                return true;
            }
        }

        bool mainDiagonalWin = true;
        for (int i = 0; i < size; i++) {
            if (board->getCell(i, i) != symbol) {
                mainDiagonalWin = false;
                break;
            }
        }
        if (mainDiagonalWin) {
            return true;
        }

        bool antiDiagonalWin = true;
        for (int i = 0; i < size; i++) {
            if (board->getCell(i, size - 1 - i) != symbol) {
                antiDiagonalWin = false;
                break;
            }
        }
        return antiDiagonalWin;
    }

    bool checkDrawCondition(Board *board) override {
        int size = board->getSize();
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (board->getCell(i, j) == board->getEmptyCell()) {
                    return false;
                }
            }
        }
        return true;
    }
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_RULES_STANDARDTICTACTOERULES_H

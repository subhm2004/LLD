#ifndef TICTACTOE_LLD_MODELS_BOARD_H
#define TICTACTOE_LLD_MODELS_BOARD_H

#include <bits/stdc++.h>

#include "Symbol.h"

using namespace std;

namespace tictactoe_lld {

class Board {
public:
    explicit Board(int size) : size_(size), emptyCell_(new Symbol('-')) {
        grid_ = vector<vector<Symbol *>>(size_, vector<Symbol *>(size_, emptyCell_));
    }

    ~Board() { delete emptyCell_; }

    bool isCellEmpty(int row, int col) const {
        if (row < 0 || row >= size_ || col < 0 || col >= size_) {
            return false;
        }
        return grid_[row][col] == emptyCell_;
    }

    bool placeMark(int row, int col, Symbol *symbol) {
        if (row < 0 || row >= size_ || col < 0 || col >= size_) {
            return false;
        }
        if (!isCellEmpty(row, col)) {
            return false;
        }
        grid_[row][col] = symbol;
        return true;
    }

    Symbol *getCell(int row, int col) const {
        if (row < 0 || row >= size_ || col < 0 || col >= size_) {
            return emptyCell_;
        }
        return grid_[row][col];
    }

    int getSize() const { return size_; }
    Symbol *getEmptyCell() const { return emptyCell_; }

    void display() const {
        cout << "\n  ";
        for (int i = 0; i < size_; i++) {
            cout << i << " ";
        }
        cout << "\n";
        for (int i = 0; i < size_; i++) {
            cout << i << " ";
            for (int j = 0; j < size_; j++) {
                cout << grid_[i][j]->getMark() << " ";
            }
            cout << "\n";
        }
        cout << "\n";
    }

private:
    vector<vector<Symbol *>> grid_;
    int size_;
    Symbol *emptyCell_;
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_MODELS_BOARD_H

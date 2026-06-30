// models/Position.h — Board pe ek cell ka coordinate (row, col). Moves aur board
// lookups isi se hote hain.
#ifndef CHESS_LLD_MODELS_POSITION_H
#define CHESS_LLD_MODELS_POSITION_H

#include <bits/stdc++.h>
using namespace std;

namespace chess_lld {

class Position {
private:
    int row_;
    int col_;

public:
    Position() : row_(0), col_(0) {}
    Position(int row, int col) : row_(row), col_(col) {}

    int getRow() const { return row_; }
    int getCol() const { return col_; }
    bool isValid() const { return row_ >= 0 && row_ < 8 && col_ >= 0 && col_ < 8; }

    bool operator==(const Position &other) const { return row_ == other.row_ && col_ == other.col_; }
    bool operator<(const Position &other) const {
        if (row_ != other.row_) return row_ < other.row_;
        return col_ < other.col_;
    }

    string toChessNotation() const {
        char file = static_cast<char>('a' + col_);
        char rank = static_cast<char>('8' - row_);
        return string(1, file) + string(1, rank);
    }
};

}

#endif

// ============================================================================
//  models/Position.h — Board ka ek coordinate (row, col) + chess notation
// ----------------------------------------------------------------------------
//  Chhota par har jagah use hone wala model. Teen kaam ki cheezein:
//    isValid()          : 0-7 range check — move generation me boundary guard
//    operator== aur <   : == comparison ke liye, < isliye ZAROORI hai kyunki
//                         Board me map<Position, Piece*> hai — std::map ko
//                         keys sort karne ke liye < chahiye hota hai!
//    toChessNotation()  : (row,col) -> "e4" jaisi asli chess bhasha:
//                         col 0-7 -> file 'a'-'h';  row 0-7 -> rank '8'-'1'
//                         (ULTA! row 0 = rank 8 — kyunki board array upar
//                          se neeche print hota hai, black upar hota hai)
// ============================================================================
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

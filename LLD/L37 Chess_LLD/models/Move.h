// ============================================================================
//  models/Move.h — Ek move ka RECORD: from -> to (+ kaun chala, kya kata)
// ----------------------------------------------------------------------------
//  Pure data holder — 4 cheezein pack karta hai:
//    from_/to_       : kahan se kahan
//    piece_          : kaunsa piece chala
//    capturedPiece_  : kya capture hua (nullptr = khali square pe move)
//  Do jagah use hota hai:
//    1. ChessRules ko validation ke liye poora move-context ek object me
//    2. Match ki moveHistory_ me — game ka record (isi se aage UNDO bhi
//       ban sakta hai — capturedPiece_ isliye store hota hai! L39 Memento
//       ya Command pattern se undo add karna practice idea hai)
//  Forward declaration `class Piece;` — sirf pointer chahiye, poori
//  definition nahi (compile speed + circular include se bachav).
// ============================================================================
#ifndef CHESS_LLD_MODELS_MOVE_H
#define CHESS_LLD_MODELS_MOVE_H

#include <bits/stdc++.h>

#include "Position.h"

using namespace std;

namespace chess_lld {
class Piece;

class Move {
private:
    Position from_;
    Position to_;
    Piece *piece_;
    Piece *capturedPiece_;

public:
    Move() : piece_(nullptr), capturedPiece_(nullptr) {}
    Move(Position from, Position to, Piece *piece, Piece *capturedPiece)
        : from_(from), to_(to), piece_(piece), capturedPiece_(capturedPiece) {}

    Position getFrom() const { return from_; }
    Position getTo() const { return to_; }
    Piece *getPiece() const { return piece_; }
    Piece *getCapturedPiece() const { return capturedPiece_; }
};
}

#endif

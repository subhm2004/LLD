// models/Move.h — Ek move: from Position -> to Position (+ moved/captured piece).
// Match move-history me inhe store karta hai.
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

// pieces/Piece.h — Ek chess piece: type, color, position, aur uska move-generation
// behavior (PieceMoves). Board inhi pieces se bharta hai.
#ifndef CHESS_LLD_PIECES_PIECE_H
#define CHESS_LLD_PIECES_PIECE_H

#include <bits/stdc++.h>

#include "../enums/Color.h"
#include "../enums/PieceType.h"
#include "../models/Position.h"

using namespace std;

namespace chess_lld {
class Board;

class Piece {
protected:
    Color color_;
    PieceType type_;
    bool hasMoved_;

public:
    Piece(Color color, PieceType type) : color_(color), type_(type), hasMoved_(false) {}
    virtual ~Piece() {}

    Color getColor() const { return color_; }
    PieceType getType() const { return type_; }
    bool getHasMoved() const { return hasMoved_; }
    void setMoved(bool moved) { hasMoved_ = moved; }

    virtual vector<Position> getPossibleMoves(Position currentPos, Board *board) = 0;
    virtual string getSymbol() = 0;

    string toString() {
        return string(color_ == WHITE ? "W" : "B") + getSymbol();
    }
};

class King : public Piece {
public:
    explicit King(Color color) : Piece(color, KING) {}
    vector<Position> getPossibleMoves(Position currentPos, Board *board) override;
    string getSymbol() override { return "K"; }
};

class Queen : public Piece {
public:
    explicit Queen(Color color) : Piece(color, QUEEN) {}
    vector<Position> getPossibleMoves(Position currentPos, Board *board) override;
    string getSymbol() override { return "Q"; }
};

class Rook : public Piece {
public:
    explicit Rook(Color color) : Piece(color, ROOK) {}
    vector<Position> getPossibleMoves(Position currentPos, Board *board) override;
    string getSymbol() override { return "R"; }
};

class Bishop : public Piece {
public:
    explicit Bishop(Color color) : Piece(color, BISHOP) {}
    vector<Position> getPossibleMoves(Position currentPos, Board *board) override;
    string getSymbol() override { return "B"; }
};

class Knight : public Piece {
public:
    explicit Knight(Color color) : Piece(color, KNIGHT) {}
    vector<Position> getPossibleMoves(Position currentPos, Board *board) override;
    string getSymbol() override { return "N"; }
};

class Pawn : public Piece {
public:
    explicit Pawn(Color color) : Piece(color, PAWN) {}
    vector<Position> getPossibleMoves(Position currentPos, Board *board) override;
    string getSymbol() override { return "P"; }
};
}

#endif

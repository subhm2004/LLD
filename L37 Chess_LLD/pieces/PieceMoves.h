// ============================================================================
//  pieces/PieceMoves.h — Har piece ka MOVE-GENERATION algorithm (asli chess yahan!)
// ----------------------------------------------------------------------------
//  Pieces ke move-patterns ki 3 categories hain — ye samajh lo to sab clear:
//
//   Category         | Pieces         | Algorithm
//   -----------------+----------------+---------------------------------------
//   STEPPER (1 kadam)| King, Knight   | direction array me har offset try karo
//                    |                |  — valid + apna piece nahi? -> add
//   SLIDER (phisalne | Queen, Rook,   | har direction me 1,2,3...7 tak SLIDE
//    wale)           | Bishop         |  karo jab tak: board khatam / apna
//                    |                |  piece (ruk jao) / dushman (capture
//                    |                |  karke ruk jao — AAR-PAAR nahi!)
//   SPECIAL (ajeeb)  | Pawn           | seedha chalta hai par TIRCHHA maarta
//                    |                |  hai; pehli move pe 2 kadam ka bonus
//
//  Direction arrays ka pattern dekho:
//    King/Queen : 8 directions (chaaro taraf + diagonals)
//    Rook       : 4 straight   {up, down, left, right}
//    Bishop     : 4 diagonal
//    Knight     : 8 "L" jumps  {±2,±1} combos — beech ke pieces ko JUMP
//                 kar jaata hai (isliye slider loop nahi, direct check!)
//
//  ⚠️ NOTE: Ye moves "pseudo-legal" hain — "king expose to nahi hoga?" wala
//  final check ChessRules::isValidMove() karta hai (wouldMoveCauseCheck).
//  Simplification: castling, en-passant, pawn-promotion implement nahi hain.
//
//  `inline` kyun har function pe? Ye header-only project hai — same function
//  multiple .cpp me include hone par linker "duplicate symbol" na bole,
//  isliye inline zaroori hai!
// ============================================================================
#ifndef CHESS_LLD_PIECES_PIECEMOVES_H
#define CHESS_LLD_PIECES_PIECEMOVES_H

#include <bits/stdc++.h>

#include "../core/Board.h"
#include "Piece.h"

using namespace std;

namespace chess_lld {

inline vector<Position> King::getPossibleMoves(Position p, Board *board) {
    vector<Position> moves;
    int d[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
    for (auto &x : d) {
        Position np(p.getRow() + x[0], p.getCol() + x[1]);
        if (np.isValid() && !board->isOccupiedBySameColor(np, color_)) moves.push_back(np);
    }
    return moves;
}

inline vector<Position> Queen::getPossibleMoves(Position p, Board *board) {
    vector<Position> moves;
    int d[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
    for (auto &x : d) for (int i = 1; i < 8; i++) {
        Position np(p.getRow() + x[0] * i, p.getCol() + x[1] * i);
        if (!np.isValid() || board->isOccupiedBySameColor(np, color_)) break;
        moves.push_back(np);
        if (board->isOccupied(np)) break;
    }
    return moves;
}

inline vector<Position> Rook::getPossibleMoves(Position p, Board *board) {
    vector<Position> moves;
    int d[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (auto &x : d) for (int i = 1; i < 8; i++) {
        Position np(p.getRow() + x[0] * i, p.getCol() + x[1] * i);
        if (!np.isValid() || board->isOccupiedBySameColor(np, color_)) break;
        moves.push_back(np);
        if (board->isOccupied(np)) break;
    }
    return moves;
}

inline vector<Position> Bishop::getPossibleMoves(Position p, Board *board) {
    vector<Position> moves;
    int d[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    for (auto &x : d) for (int i = 1; i < 8; i++) {
        Position np(p.getRow() + x[0] * i, p.getCol() + x[1] * i);
        if (!np.isValid() || board->isOccupiedBySameColor(np, color_)) break;
        moves.push_back(np);
        if (board->isOccupied(np)) break;
    }
    return moves;
}

inline vector<Position> Knight::getPossibleMoves(Position p, Board *board) {
    vector<Position> moves;
    int d[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
    for (auto &x : d) {
        Position np(p.getRow() + x[0], p.getCol() + x[1]);
        if (np.isValid() && !board->isOccupiedBySameColor(np, color_)) moves.push_back(np);
    }
    return moves;
}

inline vector<Position> Pawn::getPossibleMoves(Position p, Board *board) {
    vector<Position> moves;
    int direction = (color_ == WHITE) ? -1 : 1;
    Position one(p.getRow() + direction, p.getCol());
    if (one.isValid() && !board->isOccupied(one)) {
        moves.push_back(one);
        if (!hasMoved_) {
            Position two(p.getRow() + 2 * direction, p.getCol());
            if (two.isValid() && !board->isOccupied(two)) moves.push_back(two);
        }
    }
    Position left(p.getRow() + direction, p.getCol() - 1);
    Position right(p.getRow() + direction, p.getCol() + 1);
    if (left.isValid() && board->isOccupied(left) && !board->isOccupiedBySameColor(left, color_)) moves.push_back(left);
    if (right.isValid() && board->isOccupied(right) && !board->isOccupiedBySameColor(right, color_)) moves.push_back(right);
    return moves;
}

}

#endif

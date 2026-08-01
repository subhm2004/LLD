// ============================================================================
//  enums/PieceType.h — 6 piece types (factory ki "menu card")
// ----------------------------------------------------------------------------
//  PieceFactory isse switch karke sahi concrete class banata hai; Board
//  findKing() me KING type dhundta hai. Har type ka move-pattern
//  PieceMoves.h me hai: King/Knight = stepper, Queen/Rook/Bishop = slider,
//  Pawn = special (seedha chale, tirchha maare).
// ============================================================================
#ifndef CHESS_LLD_ENUMS_PIECETYPE_H
#define CHESS_LLD_ENUMS_PIECETYPE_H

#include <bits/stdc++.h>
using namespace std;

namespace chess_lld {
enum PieceType { KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };
}

#endif

// ============================================================================
//  factories/PieceFactory.h — SIMPLE FACTORY: pieces ki "minting machine" 🏭
// ----------------------------------------------------------------------------
//  L9 wala Simple Factory — enum (KING/QUEEN/...) + Color do, sahi concrete
//  piece object lo. Board::initializeBoard() isi se 32 pieces banwata hai.
//  Fayda: Board ko King/Queen/Rook... concrete classes ka pata hi nahi —
//  wo sirf Piece* aur PieceType enum jaanta hai (loose coupling).
//  Static method hai — factory ka koi state nahi, to object banane ki
//  zaroorat hi nahi: PieceFactory::createPiece(KING, WHITE) — done!
//  (L24 ke DiscountStrategyManager se compare karo — wahan Singleton +
//  instance method tha; yahan aur bhi simple: static. Dono valid styles.)
// ============================================================================
#ifndef CHESS_LLD_FACTORIES_PIECEFACTORY_H
#define CHESS_LLD_FACTORIES_PIECEFACTORY_H

#include <bits/stdc++.h>

#include "../pieces/Piece.h"

using namespace std;

namespace chess_lld {
class PieceFactory {
public:
    static Piece *createPiece(PieceType type, Color color) {
        switch (type) {
            case KING: return new King(color);
            case QUEEN: return new Queen(color);
            case ROOK: return new Rook(color);
            case BISHOP: return new Bishop(color);
            case KNIGHT: return new Knight(color);
            case PAWN: return new Pawn(color);
            default: return nullptr;
        }
    }
};
}

#endif

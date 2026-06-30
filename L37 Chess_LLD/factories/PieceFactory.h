// factories/PieceFactory.h — FACTORY: PieceType + Color se sahi piece (apne
// move-behavior ke saath) bana ke deta hai. Board setup isi se pieces banwata.
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

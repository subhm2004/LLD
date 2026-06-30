// rules/ChessRules.h — STRATEGY for rule engine: move legal hai? (piece moves +
// king safety), aur check/checkmate/stalemate detection. Match isse consult karta.
#ifndef CHESS_LLD_RULES_CHESSRULES_H
#define CHESS_LLD_RULES_CHESSRULES_H

#include <bits/stdc++.h>

#include "../core/Board.h"
#include "../models/Move.h"

using namespace std;

namespace chess_lld {

class ChessRules {
public:
    virtual ~ChessRules() {}
    virtual bool isValidMove(Move move, Board *board) = 0;
    virtual bool isInCheck(Color color, Board *board) = 0;
    virtual bool isCheckmate(Color color, Board *board) = 0;
    virtual bool isStalemate(Color color, Board *board) = 0;
    virtual bool wouldMoveCauseCheck(Move move, Board *board, Color kingColor) = 0;
};

class StandardChessRules : public ChessRules {
public:
    bool isValidMove(Move move, Board *board) override {
        Piece *piece = move.getPiece();
        vector<Position> possibleMoves = piece->getPossibleMoves(move.getFrom(), board);
        bool validDestination = false;
        for (const Position &pos : possibleMoves) if (pos == move.getTo()) { validDestination = true; break; }
        if (!validDestination) return false;
        return !wouldMoveCauseCheck(move, board, piece->getColor());
    }

    bool wouldMoveCauseCheck(Move move, Board *board, Color kingColor) override {
        Piece *movingPiece = board->getPiece(move.getFrom());
        Piece *capturedPiece = board->getPiece(move.getTo());
        if (movingPiece == nullptr) return true;

        board->removePiece(move.getFrom());
        if (capturedPiece != nullptr) board->removePiece(move.getTo());
        board->placePiece(move.getTo(), movingPiece);

        bool inCheck = isInCheck(kingColor, board);

        board->removePiece(move.getTo());
        board->placePiece(move.getFrom(), movingPiece);
        if (capturedPiece != nullptr) board->placePiece(move.getTo(), capturedPiece);
        return inCheck;
    }

    bool isInCheck(Color color, Board *board) override {
        Position kingPos = board->findKing(color);
        if (kingPos.getRow() == -1) return false;
        Color opponent = (color == WHITE) ? BLACK : WHITE;
        vector<Position> pieces = board->getAllPiecesOfColor(opponent);
        for (const Position &pos : pieces) {
            Piece *piece = board->getPiece(pos);
            vector<Position> moves = piece->getPossibleMoves(pos, board);
            for (const Position &target : moves) if (target == kingPos) return true;
        }
        return false;
    }

    bool isCheckmate(Color color, Board *board) override {
        if (!isInCheck(color, board)) return false;
        vector<Position> pieces = board->getAllPiecesOfColor(color);
        for (const Position &pos : pieces) {
            Piece *piece = board->getPiece(pos);
            vector<Position> moves = piece->getPossibleMoves(pos, board);
            for (const Position &target : moves) {
                Move move(pos, target, piece, board->getPiece(target));
                if (isValidMove(move, board)) return false;
            }
        }
        return true;
    }

    bool isStalemate(Color color, Board *board) override {
        if (isInCheck(color, board)) return false;
        vector<Position> pieces = board->getAllPiecesOfColor(color);
        for (const Position &pos : pieces) {
            Piece *piece = board->getPiece(pos);
            vector<Position> moves = piece->getPossibleMoves(pos, board);
            for (const Position &target : moves) {
                Move move(pos, target, piece, board->getPiece(target));
                if (isValidMove(move, board)) return false;
            }
        }
        return true;
    }
};

}

#endif

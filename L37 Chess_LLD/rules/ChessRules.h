// ============================================================================
//  rules/ChessRules.h — RULES ENGINE (Strategy pattern): chess ka "dimaag" 🧠
// ----------------------------------------------------------------------------
//  ChessRules abstract interface hai, StandardChessRules implementation —
//  kal "Chess960Rules" ya "BlitzRules" aaye to Match me swap ho sakta hai
//  (Match sirf ChessRules* jaanta hai). Yahi STRATEGY pattern hai.
//
//  5 sawalon ke jawab deta hai — sab ek-dusre pe bane hain (pyramid):
//    isValidMove()        = piece ja sakta hai? AND king expose nahi hoga?
//    wouldMoveCauseCheck()= move SIMULATE karo, check dekho, UNDO karo ⭐
//    isInCheck()          = koi dushman piece mere king tak pahunch sakta hai?
//    isCheckmate()        = check ME hoon AND koi bhi move bacha nahi sakti
//    isStalemate()        = check me NAHI hoon PAR koi legal move bhi nahi
//                           (haar nahi — DRAW! chess ka mashhoor twist)
//
//  ⭐ SIMULATE-AND-RESTORE trick (wouldMoveCauseCheck — sabse smart code):
//    1. Move ko board pe TEMPORARILY khel do (remove + place)
//    2. Dekho apna king check me aaya kya?
//    3. Sab kuch WAPAS rakh do (jaise kuch hua hi nahi — capture bhi restore!)
//  Isi se "pinned piece" wala rule free me mil jaata hai — jo piece hilne
//  se apna king khul jaye, uski move automatically invalid!
//
//  CHECKMATE/STALEMATE ka brute-force: apne HAR piece ki HAR possible move
//  try karo — koi EK bhi legal nikli to mate/stalemate nahi. Chhote board
//  pe ye kaafi fast hai (engines isi pe pruning laga ke tez karte hain).
// ============================================================================
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

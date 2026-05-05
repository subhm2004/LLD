#ifndef CHESS_LLD_CORE_BOARD_H
#define CHESS_LLD_CORE_BOARD_H

#include <bits/stdc++.h>

#include "../enums/Color.h"
#include "../factories/PieceFactory.h"
#include "../models/Position.h"

using namespace std;

namespace chess_lld {

class Board {
private:
    Piece *board_[8][8];
    map<Position, Piece *> piecePositions_;

public:
    Board() {
        for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++) board_[i][j] = nullptr;
        initializeBoard();
    }

    ~Board() {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (board_[i][j] != nullptr) delete board_[i][j];
                board_[i][j] = nullptr;
            }
        }
        piecePositions_.clear();
    }

    void initializeBoard() {
        placePiece(Position(7, 0), PieceFactory::createPiece(ROOK, WHITE));
        placePiece(Position(7, 1), PieceFactory::createPiece(KNIGHT, WHITE));
        placePiece(Position(7, 2), PieceFactory::createPiece(BISHOP, WHITE));
        placePiece(Position(7, 3), PieceFactory::createPiece(QUEEN, WHITE));
        placePiece(Position(7, 4), PieceFactory::createPiece(KING, WHITE));
        placePiece(Position(7, 5), PieceFactory::createPiece(BISHOP, WHITE));
        placePiece(Position(7, 6), PieceFactory::createPiece(KNIGHT, WHITE));
        placePiece(Position(7, 7), PieceFactory::createPiece(ROOK, WHITE));
        for (int i = 0; i < 8; i++) placePiece(Position(6, i), PieceFactory::createPiece(PAWN, WHITE));

        placePiece(Position(0, 0), PieceFactory::createPiece(ROOK, BLACK));
        placePiece(Position(0, 1), PieceFactory::createPiece(KNIGHT, BLACK));
        placePiece(Position(0, 2), PieceFactory::createPiece(BISHOP, BLACK));
        placePiece(Position(0, 3), PieceFactory::createPiece(QUEEN, BLACK));
        placePiece(Position(0, 4), PieceFactory::createPiece(KING, BLACK));
        placePiece(Position(0, 5), PieceFactory::createPiece(BISHOP, BLACK));
        placePiece(Position(0, 6), PieceFactory::createPiece(KNIGHT, BLACK));
        placePiece(Position(0, 7), PieceFactory::createPiece(ROOK, BLACK));
        for (int i = 0; i < 8; i++) placePiece(Position(1, i), PieceFactory::createPiece(PAWN, BLACK));
    }

    void placePiece(Position pos, Piece *piece) {
        board_[pos.getRow()][pos.getCol()] = piece;
        piecePositions_[pos] = piece;
    }
    void removePiece(Position pos) {
        board_[pos.getRow()][pos.getCol()] = nullptr;
        piecePositions_.erase(pos);
    }
    Piece *getPiece(Position pos) { return board_[pos.getRow()][pos.getCol()]; }
    bool isOccupied(Position pos) { return getPiece(pos) != nullptr; }
    bool isOccupiedBySameColor(Position pos, Color color) {
        Piece *piece = getPiece(pos);
        return piece != nullptr && piece->getColor() == color;
    }

    void movePiece(Position from, Position to) {
        Piece *piece = getPiece(from);
        if (piece == nullptr) return;
        Piece *captured = getPiece(to);
        if (captured != nullptr) {
            delete captured;
            piecePositions_.erase(to);
        }
        board_[from.getRow()][from.getCol()] = nullptr;
        board_[to.getRow()][to.getCol()] = piece;
        piecePositions_.erase(from);
        piecePositions_[to] = piece;
        piece->setMoved(true);
    }

    Position findKing(Color color) {
        for (auto &entry : piecePositions_) {
            if (entry.second->getType() == KING && entry.second->getColor() == color) return entry.first;
        }
        return Position(-1, -1);
    }
    vector<Position> getAllPiecesOfColor(Color color) {
        vector<Position> positions;
        for (auto &entry : piecePositions_) if (entry.second->getColor() == color) positions.push_back(entry.first);
        return positions;
    }

    void display() {
        constexpr int cellW = 3;
        auto printBorder = [&]() {
            cout << "  +";
            for (int i = 0; i < 8; ++i) cout << string(cellW, '-') << "+";
            cout << "\n";
        };
        printBorder();
        cout << "  |";
        for (char f = 'a'; f <= 'h'; ++f) {
            int pad = (cellW - 1) / 2;
            cout << string(pad, ' ') << f << string(cellW - 1 - pad, ' ') << "|";
        }
        cout << "\n";
        printBorder();
        for (int rank = 8; rank >= 1; --rank) {
            int row = 8 - rank;
            cout << rank << " |";
            for (int file = 0; file < 8; ++file) {
                Piece *p = board_[row][file];
                string s = p ? p->toString() : "  ";
                int pad = (cellW - 2) / 2;
                cout << string(pad, ' ') << s << string(cellW - 2 - pad, ' ') << "|";
            }
            cout << " " << rank << "\n";
            printBorder();
        }
    }
};

}

#endif

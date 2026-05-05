#ifndef CHESS_LLD_CORE_MATCH_H
#define CHESS_LLD_CORE_MATCH_H

#include <bits/stdc++.h>

#include "../enums/GameStatus.h"
#include "../models/Move.h"
#include "../models/User.h"
#include "../pieces/PieceMoves.h"
#include "../rules/ChessRules.h"

using namespace std;

namespace chess_lld {

class Match : public ChatMediator {
private:
    string matchId_;
    User *whitePlayer_;
    User *blackPlayer_;
    Board *board_;
    ChessRules *rules_;
    Color currentTurn_;
    GameStatus status_;
    vector<Move> moveHistory_;
    vector<Message *> chatHistory_;

public:
    Match(string matchId, User *white, User *black)
        : matchId_(std::move(matchId)), whitePlayer_(white), blackPlayer_(black), board_(new Board()),
          rules_(new StandardChessRules()), currentTurn_(WHITE), status_(IN_PROGRESS) {
        whitePlayer_->setMediator(this);
        blackPlayer_->setMediator(this);
        cout << "Match started between " << whitePlayer_->getName() << " (White) and "
             << blackPlayer_->getName() << " (Black)" << endl;
    }

    ~Match() override {
        for (auto *message : chatHistory_) delete message;
        delete board_;
        delete rules_;
    }

    bool makeMove(Position from, Position to, User *player) {
        if (status_ != IN_PROGRESS) { cout << "Game is not in progress!" << endl; return false; }
        Color playerColor = getPlayerColor(player);
        if (playerColor != currentTurn_) { cout << "It's not your turn!" << endl; return false; }
        Piece *piece = board_->getPiece(from);
        if (piece == nullptr || piece->getColor() != playerColor) { cout << "Invalid piece selection!" << endl; return false; }

        Move move(from, to, piece, board_->getPiece(to));
        if (!rules_->isValidMove(move, board_)) { cout << "Invalid move!" << endl; return false; }

        board_->movePiece(from, to);
        moveHistory_.push_back(move);
        cout << player->getName() << " moved " << piece->getSymbol()
             << " from " << from.toChessNotation() << " to " << to.toChessNotation() << endl;
        board_->display();

        Color opponentColor = (currentTurn_ == WHITE) ? BLACK : WHITE;
        if (rules_->isCheckmate(opponentColor, board_)) {
            endGame(player, "checkmate");
        } else if (rules_->isStalemate(opponentColor, board_)) {
            endGame(nullptr, "stalemate");
        } else {
            currentTurn_ = opponentColor;
            if (rules_->isInCheck(opponentColor, board_)) {
                cout << getPlayerByColor(opponentColor)->getName() << " is in check!" << endl;
            }
        }
        return true;
    }

    void quitGame(User *player) {
        User *opponent = (player == whitePlayer_) ? blackPlayer_ : whitePlayer_;
        endGame(opponent, "quit");
        player->decrementScore(50);
        cout << player->getName() << " quit the game. Score decreased by 50." << endl;
    }

    void endGame(User *winner, string reason) {
        status_ = COMPLETED;
        if (winner == nullptr) {
            cout << "Game ended in " << reason << "! No score change." << endl;
            return;
        }
        User *loser = (winner == whitePlayer_) ? blackPlayer_ : whitePlayer_;
        winner->incrementScore(30);
        loser->decrementScore(20);
        cout << "Game ended - " << winner->getName() << " wins by " << reason << "!" << endl;
        cout << "Score update: " << winner->getName() << " +30, " << loser->getName() << " -20" << endl;
    }

    Color getPlayerColor(User *player) { return (player == whitePlayer_) ? WHITE : BLACK; }
    User *getPlayerByColor(Color color) { return (color == WHITE) ? whitePlayer_ : blackPlayer_; }

    void sendMessage(Message *message, User *user) override {
        chatHistory_.push_back(message);
        User *recipient = (user == whitePlayer_) ? blackPlayer_ : whitePlayer_;
        recipient->receive(message);
        cout << "Chat in match " << matchId_ << " - " << message->getContent() << endl;
    }
    void addUser(User *user) override {}
    void removeUser(User *user) override { quitGame(user); }

    string getMatchId() const { return matchId_; }
    GameStatus getStatus() const { return status_; }
    User *getWhitePlayer() const { return whitePlayer_; }
    User *getBlackPlayer() const { return blackPlayer_; }
    Board *getBoard() const { return board_; }
};
}

#endif

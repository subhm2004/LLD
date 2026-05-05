#ifndef TICTACTOE_LLD_CORE_TICTACTOEGAME_H
#define TICTACTOE_LLD_CORE_TICTACTOEGAME_H

#include <bits/stdc++.h>

#include "../models/Board.h"
#include "../models/TicTacToePlayer.h"
#include "../observers/IObserver.h"
#include "../rules/StandardTicTacToeRules.h"

using namespace std;

namespace tictactoe_lld {

class TicTacToeGame {
public:
    explicit TicTacToeGame(int boardSize)
        : board_(new Board(boardSize)), rules_(new StandardTicTacToeRules()), gameOver_(false) {}

    ~TicTacToeGame() {
        delete board_;
        delete rules_;
    }

    void addPlayer(TicTacToePlayer *player) { players_.push_back(player); }
    void addObserver(IObserver *observer) { observers_.push_back(observer); }

    void notify(const string &message) {
        for (auto *observer : observers_) {
            observer->update(message);
        }
    }

    void play() {
        if (players_.size() < 2) {
            cout << "Need at least 2 players!\n";
            return;
        }

        notify("Tic Tac Toe Game Started!");
        while (!gameOver_) {
            board_->display();
            TicTacToePlayer *currentPlayer = players_.front();
            cout << currentPlayer->getName() << " (" << currentPlayer->getSymbol()->getMark()
                 << ") - Enter row and column: ";

            int row, col;
            cin >> row >> col;

            if (rules_->isValidMove(board_, row, col)) {
                board_->placeMark(row, col, currentPlayer->getSymbol());
                notify(currentPlayer->getName() + " played (" + to_string(row) + "," + to_string(col) + ")");

                if (rules_->checkWinCondition(board_, currentPlayer->getSymbol())) {
                    board_->display();
                    cout << currentPlayer->getName() << " wins!\n";
                    currentPlayer->incrementScore();
                    notify(currentPlayer->getName() + " wins!");
                    gameOver_ = true;
                } else if (rules_->checkDrawCondition(board_)) {
                    board_->display();
                    cout << "It's a draw!\n";
                    notify("Game is Draw!");
                    gameOver_ = true;
                } else {
                    players_.pop_front();
                    players_.push_back(currentPlayer);
                }
            } else {
                cout << "Invalid move! Try again.\n";
            }
        }
    }

private:
    Board *board_;
    deque<TicTacToePlayer *> players_;
    TicTacToeRules *rules_;
    vector<IObserver *> observers_;
    bool gameOver_;
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_CORE_TICTACTOEGAME_H

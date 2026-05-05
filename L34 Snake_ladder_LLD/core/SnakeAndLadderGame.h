#ifndef SNAKE_LADDER_LLD_CORE_GAME_H
#define SNAKE_LADDER_LLD_CORE_GAME_H

#include <bits/stdc++.h>

#include "../models/Dice.h"
#include "../models/SnakeAndLadderPlayer.h"
#include "../observers/IObserver.h"
#include "../rules/SnakeAndLadderRules.h"

using namespace std;

namespace snake_ladder_lld {
class SnakeAndLadderGame {
private:
    Board *board_;
    Dice *dice_;
    deque<SnakeAndLadderPlayer *> players_;
    SnakeAndLadderRules *rules_;
    vector<IObserver *> observers_;
    bool gameOver_;

public:
    SnakeAndLadderGame(Board *board, Dice *dice)
        : board_(board), dice_(dice), rules_(new StandardSnakeAndLadderRules()), gameOver_(false) {}

    ~SnakeAndLadderGame() {
        delete rules_;
        delete board_;
        delete dice_;
        for (auto *p : players_) delete p;
    }

    void addPlayer(SnakeAndLadderPlayer *player) { players_.push_back(player); }
    void addObserver(IObserver *observer) { observers_.push_back(observer); }
    void notify(const string &message) { for (auto *o : observers_) o->update(message); }

    void displayPlayerPositions() {
        cout << "\n=== Current Positions ===" << endl;
        for (auto *player : players_) cout << player->getName() << ": " << player->getPosition() << endl;
        cout << "======================" << endl;
    }

    void play() {
        if (players_.size() < 2) { cout << "Need at least 2 players!" << endl; return; }
        notify("Game started");
        board_->display();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        while (!gameOver_) {
            SnakeAndLadderPlayer *currentPlayer = players_.front();
            cout << "\n" << currentPlayer->getName() << "'s turn. Press Enter to roll dice...";
            cin.get();

            int diceValue = dice_->roll();
            cout << "Rolled: " << diceValue << endl;
            int currentPos = currentPlayer->getPosition();

            if (rules_->isValidMove(currentPos, diceValue, board_->getBoardSize())) {
                int intermediate = currentPos + diceValue;
                int newPos = rules_->calculateNewPosition(currentPos, diceValue, board_);
                currentPlayer->setPosition(newPos);

                BoardEntity *entity = board_->getEntity(intermediate);
                if (entity != nullptr) {
                    if (entity->name() == "SNAKE") {
                        cout << "Oh no! Snake at " << intermediate << "! Going down to " << newPos << endl;
                    } else {
                        cout << "Great! Ladder at " << intermediate << "! Going up to " << newPos << endl;
                    }
                }
                notify(currentPlayer->getName() + " played. New Position : " + to_string(newPos));
                displayPlayerPositions();

                if (rules_->checkWinCondition(newPos, board_->getBoardSize())) {
                    cout << "\n" << currentPlayer->getName() << " wins!" << endl;
                    currentPlayer->incrementScore();
                    notify("Game Ended. Winner is : " + currentPlayer->getName());
                    gameOver_ = true;
                } else {
                    players_.pop_front();
                    players_.push_back(currentPlayer);
                }
            } else {
                cout << "Need exact roll to reach " << board_->getBoardSize() << "!" << endl;
                players_.pop_front();
                players_.push_back(currentPlayer);
            }
        }
    }
};
}

#endif

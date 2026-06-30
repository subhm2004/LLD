// models/SnakeAndLadderPlayer.h — Player: naam + current board position.
// Game inhe turn-by-turn aage badhata hai.
#ifndef SNAKE_LADDER_LLD_MODELS_PLAYER_H
#define SNAKE_LADDER_LLD_MODELS_PLAYER_H

#include <bits/stdc++.h>
using namespace std;

namespace snake_ladder_lld {
class SnakeAndLadderPlayer {
private:
    int playerId_;
    string name_;
    int position_;
    int score_;

public:
    SnakeAndLadderPlayer(int playerId, string name)
        : playerId_(playerId), name_(std::move(name)), position_(0), score_(0) {}

    string getName() const { return name_; }
    int getPosition() const { return position_; }
    void setPosition(int position) { position_ = position; }
    int getScore() const { return score_; }
    void incrementScore() { score_++; }
};
}

#endif

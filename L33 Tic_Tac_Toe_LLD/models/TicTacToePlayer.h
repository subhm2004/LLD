// models/TicTacToePlayer.h — Player: naam + apna unique Symbol. Game turn-by-turn
// inhi players ko move karne deta hai.
#ifndef TICTACTOE_LLD_MODELS_TICTACTOEPLAYER_H
#define TICTACTOE_LLD_MODELS_TICTACTOEPLAYER_H

#include <bits/stdc++.h>

#include "Symbol.h"

using namespace std;

namespace tictactoe_lld {

class TicTacToePlayer {
public:
    TicTacToePlayer(int playerId, string name, Symbol *symbol)
        : playerId_(playerId), name_(std::move(name)), symbol_(symbol), score_(0) {}

    ~TicTacToePlayer() { delete symbol_; }

    int getPlayerId() const { return playerId_; }
    string getName() const { return name_; }
    Symbol *getSymbol() const { return symbol_; }
    int getScore() const { return score_; }
    void incrementScore() { score_++; }

private:
    int playerId_;
    string name_;
    Symbol *symbol_;
    int score_;
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_MODELS_TICTACTOEPLAYER_H

#ifndef SNAKE_LADDER_LLD_MODELS_DICE_H
#define SNAKE_LADDER_LLD_MODELS_DICE_H

#include <bits/stdc++.h>
using namespace std;

namespace snake_ladder_lld {
class Dice {
private:
    int faces_;

public:
    explicit Dice(int faces) : faces_(faces) { srand(static_cast<unsigned int>(time(0))); }
    int roll() { return (rand() % faces_) + 1; }
};
}

#endif

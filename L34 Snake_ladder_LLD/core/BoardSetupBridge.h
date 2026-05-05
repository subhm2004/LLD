#ifndef SNAKE_LADDER_LLD_CORE_BOARDSETUPBRIDGE_H
#define SNAKE_LADDER_LLD_CORE_BOARDSETUPBRIDGE_H

#include <bits/stdc++.h>

#include "Board.h"
#include "../strategies/BoardSetupStrategy.h"

using namespace std;

namespace snake_ladder_lld {
inline void Board::setupBoard(BoardSetupStrategy *strategy) {
    strategy->setupBoard(this);
}
}

#endif

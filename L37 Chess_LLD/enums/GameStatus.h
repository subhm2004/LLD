// enums/GameStatus.h — Match ka status: ACTIVE / CHECK / CHECKMATE / STALEMATE
// etc. Rules detect karke yahi status set karte hain.
#ifndef CHESS_LLD_ENUMS_GAMESTATUS_H
#define CHESS_LLD_ENUMS_GAMESTATUS_H

#include <bits/stdc++.h>
using namespace std;

namespace chess_lld {
enum GameStatus { WAITING, IN_PROGRESS, COMPLETED, ABORTED };
}

#endif

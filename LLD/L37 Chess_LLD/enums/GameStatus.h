// ============================================================================
//  enums/GameStatus.h — Match ka lifecycle status
// ----------------------------------------------------------------------------
//  WAITING     -> opponent ka intezaar (matchmaking queue me)
//  IN_PROGRESS -> khel chalu — sirf isi state me makeMove() allowed
//                 (Match ka Gate #1 yahi check karta hai!)
//  COMPLETED   -> checkmate/stalemate/quit se khatam (endGame() set karta hai)
//  ABORTED     -> beech me radd (network drop jaise cases ke liye reserved)
// ============================================================================
#ifndef CHESS_LLD_ENUMS_GAMESTATUS_H
#define CHESS_LLD_ENUMS_GAMESTATUS_H

#include <bits/stdc++.h>
using namespace std;

namespace chess_lld {
enum GameStatus { WAITING, IN_PROGRESS, COMPLETED, ABORTED };
}

#endif

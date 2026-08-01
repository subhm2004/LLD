// ============================================================================
//  enums/Color.h — WHITE / BLACK (piece ka malik + kiski turn hai)
// ----------------------------------------------------------------------------
//  Har jagah use hota hai: piece ownership, turn tracking (Match me
//  currentTurn_), rules me "apna vs dushman" checks, pawn ki direction
//  (WHITE upar jaata hai row-- , BLACK neeche row++).
//  Opponent nikalne ka common idiom: (color == WHITE) ? BLACK : WHITE
// ============================================================================
#ifndef CHESS_LLD_ENUMS_COLOR_H
#define CHESS_LLD_ENUMS_COLOR_H

#include <bits/stdc++.h>
using namespace std;

namespace chess_lld {
enum Color { WHITE, BLACK };
}

#endif

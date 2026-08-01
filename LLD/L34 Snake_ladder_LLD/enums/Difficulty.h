// ============================================================================
//  enums/Difficulty.h  —  Random board kitna mushkil banega
// ----------------------------------------------------------------------------
//  Ye sirf ek "tag" hai jo `RandomBoardSetupStrategy` ko batata hai ki board pe
//  saanp zyada daalne hain ya seedhiyan.
//
//  Andar ye ek PROBABILITY ban jaata hai (BoardSetupStrategyImpl.h me dekho):
//
//     EASY   -> 0.3  ->  har entity ke 30% chance saanp banne ke, 70% seedhi ke
//                        (yaani zyadatar SEEDHIYAN -> khel aasan) 🪜
//     MEDIUM -> 0.5  ->  aadha-aadha (barabar ka khel)
//     HARD   -> 0.7  ->  70% chance saanp ka -> zyadatar SAANP -> khel mushkil 🐍
//
//  ⭐ Notice: difficulty se entity ki GINTI nahi badalti — sirf saanp aur seedhi
//  ka ANUPAAT badalta hai. Kul entities hamesha `boardSize / 10` hi rehti hain
//  (100 cells = 10 entities). Bas unme se kitne saanp honge, wo difficulty tay
//  karti hai. Chhoti si baat, par design me saaf hai.
//
//  ⚠ `enum` hai, `enum class` NAHI:
//     - `EASY` ka naam poore namespace me leak ho jaata hai (isi liye main.cpp
//       me seedha `EASY` likha ja sakta hai, `Difficulty::EASY` nahi)
//     - Aur ye chupke se `int` me convert ho jaata hai — yaani koi
//       `createRandomGame(10, 0)` bhi likh sakta hai (0 == EASY). Compiler
//       rokega hi nahi. 😬
//
//     `enum class Difficulty { EASY, MEDIUM, HARD };` zyada safe hota —
//     `Difficulty::EASY` likhna padta, aur int se galti se compare nahi hota.
//     (L31 Splitwise me `enum class SplitType` isi liye use kiya tha.)
// ============================================================================
#ifndef SNAKE_LADDER_LLD_ENUMS_DIFFICULTY_H
#define SNAKE_LADDER_LLD_ENUMS_DIFFICULTY_H

#include <bits/stdc++.h>
using namespace std;

namespace snake_ladder_lld {

enum Difficulty { EASY, MEDIUM, HARD };

}

#endif

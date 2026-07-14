// ============================================================================
//  enums/GameType.h  —  Game ka variant ("kaunsa tic-tac-toe khelna hai")
// ----------------------------------------------------------------------------
//  Client (main.cpp) factory se bolta hai "mujhe STANDARD game chahiye", aur
//  factory us hisaab se game bana deti hai. Client ko concrete classes ke naam
//  jaanne ki zaroorat hi nahi.
//
//  Abhi sirf EK value hai — STANDARD. To ye enum kyun banaya?
//    Kyunki ye AAGE KE LIYE ek "khaali jagah" (extension point) hai. Kal ko:
//        enum class GameType { STANDARD, FOUR_IN_A_ROW, MISERE, TIMED };
//    Aur factory har type ke liye alag rules plug kar de. (TicTacToeRules.h me
//    ye variants detail me likhe hain.)
//
//  ⚠ Do problem hain is enum ke saath:
//
//  1️⃣ `enum` hai, `enum class` NAHI.
//     Farak kya hai?
//       - `enum GameType { STANDARD };`
//              -> `STANDARD` naam poore namespace me LEAK ho jaata hai. Seedha
//                 `STANDARD` likha ja sakta hai (main.cpp me wahi hua hai).
//              -> Aur ye chupke se `int` me convert ho jaata hai! Yaani
//                 `createGame(0, 3)` bhi chal jaayega — 0 == STANDARD. 😬
//                 Compiler rokega hi nahi.
//       - `enum class GameType { STANDARD };`
//              -> `GameType::STANDARD` likhna PADEGA (scoped hai)
//              -> int me apne aap convert NAHI hoga -> type safety ✅
//
//     L31 (Splitwise) me `enum class SplitType` use hua tha — wo zyada sahi tha.
//     Yahan plain enum hai. Chhoti si inconsistency, par theek karne layak.
//
//  2️⃣ Ye enum ABHI KUCH KARTA HI NAHI — bas dikhawa hai. 🎭
//     Factory `if (gameType == STANDARD)` check karti hai, par game ke andar
//     rules WAISE BHI hardcoded hain (`new StandardTicTacToeRules()`). Yaani
//     GameType kuch bhi ho, milega wahi ek game. Enum ka koi asar hi nahi.
//     Poori kahani TicTacToeGameFactory.h me likhi hai — wahan padho.
// ============================================================================
#ifndef TICTACTOE_LLD_ENUMS_GAMETYPE_H
#define TICTACTOE_LLD_ENUMS_GAMETYPE_H

#include <bits/stdc++.h>

using namespace std;

namespace tictactoe_lld {

enum GameType { STANDARD };

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_ENUMS_GAMETYPE_H

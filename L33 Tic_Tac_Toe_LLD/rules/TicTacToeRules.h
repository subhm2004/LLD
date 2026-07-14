// ============================================================================
//  rules/TicTacToeRules.h  —  STRATEGY PATTERN (Behavioral) ka interface
// ----------------------------------------------------------------------------
//  "Game ke NIYAM" ko game ke ENGINE se alag kar do.
//
//    TicTacToeGame (engine) jaanta hai : "turn kaise ghumana hai, board kab
//                                         dikhana hai, kab notify karna hai"
//    TicTacToeRules (niyam) jaanta hai : "move valid hai? koi jeeta? draw hua?"
//
//  Engine ko ye pata hi NAHI ki jeetne ke liye 3 in a row chahiye. Wo bas
//  `rules_->checkWinCondition(...)` puchta hai aur jo jawab mile, maan leta hai.
//
//  ⭐ Iska FAYDA — naye game variants bina engine chhue:
//     StandardTicTacToeRules  -> poori row/col/diagonal chahiye (abhi hai)
//     FourInARowRules         -> 5×5 board pe sirf 4 lagatar chahiye
//     MisereRules             -> ULTA game: jo 3 banaye wo HAAR jaaye 😄
//     TimedRules              -> 10 second me move na kiya to invalid
//     WildcardRules           -> koi ek cell dono ke liye count ho
//
//  Har variant = ek nayi class. TicTacToeGame ki EK LINE bhi nahi badlegi.
//  Yahi OPEN/CLOSED PRINCIPLE hai — nayi cheez ke liye code JODO, purana CHHEDO mat.
//
//  ⚠ PAR — ek badi kami hai is code me:
//    Strategy ka poora point hai ki rules BAHAR SE PLUG kiye ja sakein.
//    Yahan TicTacToeGame ka constructor rules ko HARDCODE kar deta hai:
//
//        TicTacToeGame(int boardSize)
//            : board_(new Board(boardSize)),
//              rules_(new StandardTicTacToeRules()),   // 👈 hardcoded!
//              ...
//
//    Yaani upar likhe saare variants theoretical hain — abhi unhe use karne ka
//    koi RAASTA hi nahi hai! Interface bana diya, par usko plug karne ka darwaza
//    nahi chhoda. Strategy pattern aadha hi laga hai.
//
//    FIX (seedha sa): rules ko INJECT karo, andar mat banao —
//        TicTacToeGame(int boardSize, TicTacToeRules *rules)
//            : board_(new Board(boardSize)), rules_(rules), gameOver_(false) {}
//    Aur factory decide kare ki kaunse rules bhejne hain (GameType dekh ke).
//    Ise DEPENDENCY INJECTION kehte hain — "apni zaroorat khud mat banao,
//    bahar se maango". Tabhi ye asli Strategy banega.
//    (Poori detail TicTacToeGameFactory.h me likhi hai.)
// ============================================================================
#ifndef TICTACTOE_LLD_RULES_TICTACTOERULES_H
#define TICTACTOE_LLD_RULES_TICTACTOERULES_H

#include <bits/stdc++.h>

#include "../models/Board.h"

using namespace std;

namespace tictactoe_lld {

class TicTacToeRules {
public:
    // Teeno `= 0` -> pure virtual. Har rule-set ko ye teen sawaalon ka jawab
    // dena HI padega. Compiler majboor karega — koi rule-set kisi sawaal ko
    // "bhool" nahi sakta.

    // 1. "Kya is jagah move kiya ja sakta hai?"
    //    Standard: cell khaali honi chahiye, bas.
    //    Ek TimedRules isme time-limit bhi check kar sakta tha.
    virtual bool isValidMove(Board *board, int row, int col) = 0;

    // 2. "Kya IS symbol wala player jeet gaya?"
    //    Note: poore board ka analysis nahi — sirf EK symbol ke liye puchha ja
    //    raha hai. Kyunki engine ye tabhi puchhta hai jab kisi ne abhi move
    //    kiya ho, aur sirf WAHI jeet sakta hai (uske move se hi to line bani).
    //    Chhoti si baat, par kaafi kaam bach jaata hai.
    virtual bool checkWinCondition(Board *board, Symbol *symbol) = 0;

    // 3. "Kya game draw ho gaya?"
    //    Standard: board bhar gaya aur koi jeeta nahi.
    //    (Engine hamesha pehle win check karta hai, phir draw — order zaroori hai!)
    virtual bool checkDrawCondition(Board *board) = 0;

    // Virtual destructor — MUST. Engine rules ko `TicTacToeRules*` (base pointer)
    // me rakhta hai, par asal object `StandardTicTacToeRules` hai. Engine ka
    // destructor `delete rules_` karta hai — BASE pointer se. Agar ye virtual na
    // hota, to derived ka destructor chalta hi nahi -> undefined behavior.
    // 📌 RULE: ek bhi virtual function hai? To destructor bhi virtual. Har baar.
    virtual ~TicTacToeRules() {}
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_RULES_TICTACTOERULES_H

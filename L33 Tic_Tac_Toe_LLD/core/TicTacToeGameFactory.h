// ============================================================================
//  core/TicTacToeGameFactory.h  —  FACTORY PATTERN (Creational)
// ----------------------------------------------------------------------------
//  Factory = "object banane ka kaam ek jagah rakh do. Client bas TYPE bataye,
//             banane ki jhanjhat factory sambhale."
//
//  Client (main.cpp) ko bas itna likhna hai:
//      TicTacToeGame *game = TicTacToeGameFactory::createGame(STANDARD, 3);
//  Usko `TicTacToeGame` ka constructor, board banane ka tareeka, rules kaunse
//  lagne hain — kuch nahi pata hona chahiye. Bas "STANDARD chahiye" bola, mil gaya.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FACTORY KA ASLI KAAM — "kaun sa game" ka faisla lena                 │
//  │                                                                          │
//  │  Pehle ye factory ek DIKHAWA thi 🎭 — `gameType` leti thi par uska koi   │
//  │  asar hi nahi tha, kyunki `TicTacToeGame` apne rules KHUD hardcode karta │
//  │  tha (`new StandardTicTacToeRules()`). Kuch bhi bhejo, wahi ek game.     │
//  │                                                                          │
//  │  Ab factory ASAL ME kuch decide karti hai:                               │
//  │     1. GameType dekh ke sahi RULES chunti hai                            │
//  │     2. Un rules ko game me INJECT karti hai (dependency injection)       │
//  │                                                                          │
//  │  Isse naye variants ka darwaza khul gaya — `FourInARowRules`,            │
//  │  `MisereRules`, `TimedRules`. Bas yahan ek `case` add karo, aur poora    │
//  │  baaki system (Game, Board, main.cpp) waisa ka waisa rehta hai. ✅       │
//  │  Yahi OPEN/CLOSED PRINCIPLE hai — extension ke liye khula, modification  │
//  │  ke liye band.                                                           │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Ye kaunsa Factory hai?
//     "Simple Factory" / "Static Factory" — ek static method jo switch chalata
//     hai. Poora GoF "Factory Method" tab hota jab factory KHUD abstract hoti
//     aur subclasses decide karti ki kya banana hai. LLD interviews me 90% cases
//     me yahi simple wala kaafi hota hai. (L31 SplitFactory bhi yahi tha.)
// ============================================================================
#ifndef TICTACTOE_LLD_CORE_TICTACTOEGAMEFACTORY_H
#define TICTACTOE_LLD_CORE_TICTACTOEGAMEFACTORY_H

#include <bits/stdc++.h>

#include "../enums/GameType.h"
#include "TicTacToeGame.h"

using namespace std;

namespace tictactoe_lld {

class TicTacToeGameFactory {
public:
    // `static` -> object banane ki zaroorat nahi, seedha
    // `TicTacToeGameFactory::createGame(...)` bulao. Factory ka apna koi STATE
    // (member variable) nahi hai, to uska object banane ka koi matlab hi nahi.
    static TicTacToeGame *createGame(GameType gameType, int boardSize) {

        // ---- ✅ FIX 1: board size yahin validate karo ------------------------
        // Pehle ye check kahin tha hi nahi. `boardSize = 0` bhejte to game me
        // INFINITE LOOP ho jaata, aur `-1` bhejte to Board ke andar
        // `std::length_error` throw hoke program CRASH ho jaata. 💥
        //
        // Ab galat size yahin ruk jaayegi — Board tak pahunchegi hi nahi.
        // ⭐ Aur ye check rules banane se PEHLE hai — jaan-boojh ke. Agar baad me
        // hota, to `rules` bana ke throw kar dete aur wo LEAK ho jaata. Cleanup
        // ki jhanjhat se bachne ka sabse aasan tareeka: fail karo to JALDI karo,
        // kuch allocate karne se pehle. ("Fail fast.")
        if (boardSize < 3) {
            throw invalid_argument("Board size kam se kam 3 hona chahiye (mila: " +
                                   to_string(boardSize) + ")");
        }

        // ---- ✅ FIX 2: ab factory ASAL ME kuch decide karti hai ---------------
        // Pehle ye seedha `new TicTacToeGame(boardSize)` kar deti thi, aur game
        // apne rules KHUD hardcode karta tha. Yaani `gameType` ka koi ASAR hi
        // nahi tha — factory bas ek dikhawa thi. 🎭
        //
        // Ab yahan rules CHUNE jaate hain aur game me INJECT hote hain. Naya
        // variant add karna ho? Bas `GameType` me ek entry aur yahan ek `case`:
        //
        //     case FOUR_IN_A_ROW: rules = new FourInARowRules(); break;
        //     case MISERE:        rules = new MisereRules();     break;
        //
        // Baaki poora system (Game, Board, main) chhune ki zaroorat hi nahi.
        // YEHI Factory + Strategy ka asli fayda hai — ab jaake ye zinda hua.
        TicTacToeRules *rules = nullptr;
        switch (gameType) {
            case STANDARD:
                rules = new StandardTicTacToeRules();
                break;
        }

        // ---- ✅ FIX 3: `nullptr` ki jagah THROW ------------------------------
        // Pehle yahan `return nullptr;` tha, aur main.cpp use CHECK HI NAHI karta
        // tha -> seedha null-pointer crash. Ek bichha hua barud tha 💣: jis din
        // koi `GameType` me nayi entry daalta aur upar `case` add karna bhool
        // jaata, us din crash milta — aur wo crash main.cpp me hota, yahan nahi.
        // Ghante lag jaate dhoondhne me.
        //
        // 📌 `nullptr` lautana ek CHUPI HUI galti hai — use ignore karna aasan
        // hai (aur caller aksar ignore kar hi deta hai, jaisa yahan hua tha).
        // Exception ko ignore karna MUSHKIL hai — wo khud awaaz karta hai.
        // Isi liye exception behtar hai. "Galti chupni nahi chahiye."
        if (rules == nullptr) {
            throw invalid_argument("Ye GameType support nahi karta");
        }

        // Ownership `rules` ka ab game ko mil jaata hai — wahi ise delete karega.
        return new TicTacToeGame(boardSize, rules);
    }
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_CORE_TICTACTOEGAMEFACTORY_H

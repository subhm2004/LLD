// ============================================================================
//  core/BoardSetupBridge.h  —  BRIDGE PATTERN (Structural)
// ----------------------------------------------------------------------------
//  Poori file me sirf EK line ka function hai. Par wo ek line hi "bridge" hai —
//  wo pul jo Board (abstraction) ko uske Setup (implementation) se JODTA hai,
//  bina dono ko ek-doosre me GHOL diye.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  BRIDGE = "Abstraction aur Implementation ko alag rakho, taaki DONO      │
//  │            apne-apne hisaab se badh sakein — ek doosre ko rok na sakein."│
//  │                                                                          │
//  │  Yahan:                                                                  │
//  │     Abstraction    -> Board            ("main ek board hu")              │
//  │     Implementation -> BoardSetupStrategy ("main board bharne ka tareeka") │
//  │                                                                          │
//  │  Dono ALAG-ALAG badh sakte hain:                                         │
//  │                                                                          │
//  │     Board ke naye roop:        Setup ke naye tareeke:                    │
//  │       - SquareBoard              - StandardSetup                         │
//  │       - CircularBoard            - RandomSetup                           │
//  │       - HexBoard                 - CustomSetup                           │
//  │       - 3DBoard                  - MirrorSetup                           │
//  │                                                                          │
//  │  Bina Bridge ke? Har JODI ke liye alag class banani padti:               │
//  │     SquareBoardWithStandardSetup, SquareBoardWithRandomSetup,            │
//  │     CircularBoardWithStandardSetup, CircularBoardWithRandomSetup, ...     │
//  │                                                                          │
//  │     4 boards × 4 setups = 16 CLASSES! 😱 Aur ek naya board add karo to   │
//  │     4 aur classes. Ise "class explosion" kehte hain.                     │
//  │                                                                          │
//  │  Bridge ke saath? 4 + 4 = 8 classes. Bas. Aur koi bhi board kisi bhi     │
//  │  setup ke saath chal jaata — runtime pe jodo. ✅                          │
//  │                                                                          │
//  │  📌 Yaad rakhne ka formula:  M × N  ->  M + N                            │
//  │     Bridge GUNA ko JOD me badal deta hai. Yahi uska poora jaadu hai.     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ============================================================================
//   ⭐ BRIDGE vs STRATEGY — interview ka favourite sawaal. Dhyan se padho.
//  ----------------------------------------------------------------------------
//   Sach ye hai: CODE dono ka LAGBHAG EK JAISA dikhta hai. Dono me ek interface
//   hota hai, aur usko koi class pointer se use karti hai. To farak kya hai?
//
//   Farak NIYAT ka hai (intent), structure ka nahi:
//
//     STRATEGY  -> "Ek KAAM ko karne ke kai TAREEKE hain. Runtime pe chuno."
//                  Focus: BEHAVIOR badalna. (jaise: kaise sort karein — quick/merge)
//                  Yahan: "board bharne ke kai tareeke" — Standard/Random/Custom
//
//     BRIDGE    -> "Do cheezein DONO taraf se badh sakti hain. Unhe bandho mat."
//                  Focus: DO HIERARCHIES ko alag rakhna, class explosion rokna.
//                  Yahan: Board ki hierarchy + Setup ki hierarchy, dono aazaad
//
//   Is project me DONO hi sach hain 😄 — isi liye ise "Bridge" kaha gaya hai
//   par `BoardSetupStrategy` naam bhi diya gaya hai. Confusing lagta hai, par
//   galat kuch nahi.
//
//   Interview me aisa bolo: "Structure dono ka same hai. Agar mera maqsad sirf
//   ALGORITHM swap karna hai, to main ise Strategy kahunga. Agar mere paas DO
//   swatantra hierarchies hain jo alag-alag badh rahi hain aur main M×N class
//   explosion se bachna chahta hu, to ye Bridge hai."
//   Ye jawab tumhe 90% candidates se alag kar dega. 🎯
//  ============================================================================
#ifndef SNAKE_LADDER_LLD_CORE_BOARDSETUPBRIDGE_H
#define SNAKE_LADDER_LLD_CORE_BOARDSETUPBRIDGE_H

#include <bits/stdc++.h>

#include "Board.h"
#include "../strategies/BoardSetupStrategy.h"

using namespace std;

namespace snake_ladder_lld {

// ============================================================================
//  YE FILE ALAG KYUN HAI? — circular dependency todne ke liye
// ----------------------------------------------------------------------------
//  Murgi-anda wali problem:
//      Board.h    ko `BoardSetupStrategy*` chahiye  (setupBoard ke param me)
//      Strategy.h ko `Board*` chahiye               (setupBoard ke param me)
//  Dono ek-doosre ko chahte hain! Koi bhi "pehle" nahi likha ja sakta.
//
//  Hal — teen hisso me baanto:
//      1. Board.h              -> `class BoardSetupStrategy;` (forward decl)
//                                 aur `void setupBoard(BoardSetupStrategy*);`
//                                 SIRF DECLARE karo, body mat likho
//      2. BoardSetupStrategy.h -> `class Board;` (forward decl)
//      3. YE FILE              -> dono ko POORA include karo, aur AB body likho ✅
//
//  Ab tak dono classes poori define ho chuki hain, to yahan `strategy->setupBoard()`
//  bulana bilkul safe hai.
//
//  📌 Ye C++ ka aam muhavara hai — jab do classes ek doosre ko chahiye, to
//     declaration aur definition ko alag files me todo. (L32 me bhi yahi kiya
//     tha — wahan sab ek hi file me tha, bas order badal ke.)
// ============================================================================
inline void Board::setupBoard(BoardSetupStrategy *strategy) {
    // ⭐ YE HAI WO PUL (bridge). Sirf ek line.
    //
    // Board KHUD nahi jaanta ki uspe entities kaise lagengi. Wo bas strategy ko
    // apna pointer (`this`) de deta hai aur kehta hai: "le, tu bhar de mujhe."
    //
    // Strategy phir `board->addBoardEntity(...)` bula-bula ke board bhar deti hai.
    // Board ko bilkul pata nahi chalta ki wo StandardSetup tha, RandomSetup, ya
    // koi aur — aur na hi usko jaanna chahiye. Yahi DECOUPLING hai. ✅
    //
    // 💡 Notice: Board strategy ka pointer STORE nahi karta — bas use karke chhod
    // deta hai. Isi liye caller (factory / main.cpp) strategy ko turant `delete`
    // kar sakta hai. Agar Board ise sambhal ke rakhta, to wo delete DANGLING
    // POINTER bana deta. Ye chhota sa detail bahut zaroori hai — main.cpp me
    // `delete strategy;` isi bharose likha gaya hai.
    strategy->setupBoard(this);
}

}

#endif

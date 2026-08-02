// ============================================================================
//  #1 — 01_fixed_window.cpp
//  FIXED WINDOW COUNTER — sabse sasta, aur sabse "leaky"
// ----------------------------------------------------------------------------
//  Idea sabse seedha hai: waqt ko fixed tukdon (windows) me kaat do, aur har
//  tukde me ek counter rakho.
//
//        window = 10 second, limit = 5
//        [0-10s) me 5 requests OK, 6th DENY
//        [10-20s) me counter WAPAS 0 se shuru
//
//  Code (aapka `rate_limiter/fixed_window.h`) me poora state bas itna hai:
//        struct Window { double start; int count; bool init; };
//  Yaani per key sirf ~16 bytes. Isse sasta kuch nahi ho sakta.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ WINDOW "GRID" PE SNAP HOTI HAI — ye detail zaroori hai                │
//  │                                                                          │
//  │      w.start = floor(now / window) * window;                              │
//  │                                                                          │
//  │  Yaani window kabhi bhi "jab pehli request aayi" se shuru nahi hoti —     │
//  │  wo hamesha ek fixed grid pe baithti hai (0-10, 10-20, 20-30...).         │
//  │                                                                          │
//  │  Faayda: DETERMINISTIC hai. Aapke 10 servers alag-alag waqt pe start hue  │
//  │  hon, phir bhi sab EK HI window boundary maanenge. Agar har server apni   │
//  │  pehli request se window shuru karta, to sabki windows alag hoti aur      │
//  │  distributed rate limiting ka koi matlab hi na rehta.                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠⚠ ISKI ASLI KAMZORI — BOUNDARY BURST (2x limit)                        │
//  │                                                                          │
//  │  Counter har window pe ACHANAK 0 ho jaata hai. To agar koi client         │
//  │  window ke BILKUL ANT me apni saari requests bheje, aur phir agli window  │
//  │  ke BILKUL SHURU me phir se — to usne bahut kam samay me 2x limit         │
//  │  nikaal liya, aur algorithm ko pata bhi nahi chala.                       │
//  │                                                                          │
//  │        limit=5 per 10s                                                    │
//  │        t=9.99s  -> 5 requests ✅ (window 1 ka counter: 5)                 │
//  │        t=10.01s -> 5 requests ✅ (window 2 ka counter: 0 se shuru!)       │
//  │        => 0.02 second me 10 requests. Limit 5 thi. 😱                     │
//  │                                                                          │
//  │  Ye koi theoretical baat nahi — ye demo ise naap ke dikhayega.            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ✅ Faayde  : sabse kam memory (O(1) per key), sabse tez, samajhne me aasan
//  ❌ Nuksan  : boundary pe 2x tak burst nikal jaata hai
//  📌 Kab use : jab thoda overshoot chalta ho aur paimana/memory sabse zaroori
//               ho (jaise crore users ka mota-mota quota)
//
//  Compile: g++ -std=c++17 -Wall -Wextra 01_fixed_window.cpp -o fixed_window_demo
// ============================================================================
#include <bits/stdc++.h>

#include "rate_limiter/fixed_window.h"
#include "rl_harness.h"

using namespace std;
using namespace rlx;

int main()
{
    cout << "=========================================================\n";
    cout << " #1 : FIXED WINDOW COUNTER\n";
    cout << " limit = 5 requests per 10 second window\n";
    cout << "=========================================================\n\n";

    // ---- Part A: normal behaviour ----------------------------------------
    cout << "[A] Seedha-sada case — ek window me 7 requests:\n\n";
    {
        Probe<rl::FixedWindowCounter> limiter(5, 10.0);
        for (int i = 1; i <= 7; ++i) {
            double t = i * 0.5; // t = 0.5, 1.0, ... 3.5 (sab window [0,10) me)
            rl::Decision d = limiter.at("user1", t);
            printDecision(t, d, i == 6 ? "<-- limit lag gayi" : "");
        }
        cout << "\n    Pehli 5 allow, uske baad deny. Bilkul jaisa hona chahiye. ✅\n\n";
    }

    // ---- Part B: window reset --------------------------------------------
    cout << "[B] Agli window me counter RESET ho jaata hai:\n\n";
    {
        Probe<rl::FixedWindowCounter> limiter(5, 10.0);
        sendBurst(limiter, "user1", 1.0, 5);          // window [0,10): 5/5 bhar gayi
        printDecision(9.0, limiter.at("user1", 9.0), "<-- window 1 abhi bhi full");
        printDecision(11.0, limiter.at("user1", 11.0), "<-- window 2, counter 0 se shuru ✅");
        cout << "\n";
    }

    // ---- Part C: ⭐ BOUNDARY BURST — asli kamzori -----------------------
    cout << "[C] ⭐ BOUNDARY BURST — is algorithm ki sabse badi kamzori:\n\n";
    {
        Probe<rl::FixedWindowCounter> limiter(5, 10.0);

        // Window 1 ke bilkul ANT me (t=9.99) saari 5 requests
        int allowedEnd = sendBurst(limiter, "attacker", 9.99, 5);

        // Window 2 ke bilkul SHURU me (t=10.01) phir se 5
        int allowedStart = sendBurst(limiter, "attacker", 10.01, 5);

        cout << "    t=9.99s  (window 1 ka ant)   : " << allowedEnd << " requests ALLOW\n";
        cout << "    t=10.01s (window 2 ki shuruaat): " << allowedStart << " requests ALLOW\n";
        cout << "    ----------------------------------------------------\n";
        int total = allowedEnd + allowedStart;
        cout << "    Kul " << total << " requests sirf 0.02 second me!\n";
        cout << "    Configured limit thi : 5 per 10 second\n";
        cout << "    Asal me nikal gaya   : " << total << " per 0.02 second = " << (total / 5)
             << "x limit 😱\n\n";

        cout << "    ⭐ Aur ye koi 'hack' nahi hai — algorithm ne apne hisaab se BILKUL\n";
        cout << "       SAHI kaam kiya. Dono windows me alag-alag 5-5 requests thi.\n";
        cout << "       Galti algorithm ki soch me hai: wo 'pichhli window' ko\n";
        cout << "       poori tarah BHOOL jaata hai.\n\n";
    }

    // ---- Part D: kitna bura ho sakta hai ---------------------------------
    cout << "[D] Alag-alag limits pe ye burst kitna bada hota hai:\n\n";
    cout << "    limit | window | boundary pe kitni requests nikal gayi\n";
    cout << "    ------+--------+---------------------------------------\n";
    for (int limit : {5, 10, 100, 1000}) {
        Probe<rl::FixedWindowCounter> limiter(limit, 60.0);
        int a = sendBurst(limiter, "u", 59.99, limit);
        int b = sendBurst(limiter, "u", 60.01, limit);
        cout << "    " << setw(5) << limit << " |   60s  | " << setw(5) << (a + b)
             << "  (= 2x limit, 0.02s me)\n";
    }
    cout << "\n    ⭐ Ye hamesha THEEK 2x hota hai — chahe limit kitni bhi ho.\n";
    cout << "       Isliye capacity plan karte waqt maan ke chalo ki peak 2x aayega.\n";

    // ---- Part E: memory ---------------------------------------------------
    cout << "\n[E] Iski asli taakat — MEMORY:\n\n";
    cout << "    Per key state: struct Window { double start; int count; bool init; }\n";
    cout << "    = ~" << sizeof(double) + sizeof(int) + sizeof(bool) << " bytes (padding ke saath ~"
         << 16 << ")\n";
    cout << "\n    1 crore (10 million) users ke liye: ~160 MB\n";
    cout << "    Aur limit kitni bhi ho (5 ho ya 10,000) — memory WAHI rehti hai,\n";
    cout << "    kyunki hum sirf ek COUNTER rakhte hain, timestamps nahi. ⭐\n";
    cout << "    (Sliding Window Log me yahi cheez blast ho jaati hai — file 02.)\n";

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Sabse sasta: O(1) memory per key, limit se bilkul independent.\n";
    cout << "  ✅ Sabse tez aur samajhne me sabse aasan.\n";
    cout << "  ✅ Grid-snapped windows = distributed setup me sab servers ek raay.\n";
    cout << "  ❌ BOUNDARY BURST: hamesha 2x limit nikal sakta hai.\n";
    cout << "  ❌ Window ke andar traffic ka pattern dikhta hi nahi (sab ek saath\n";
    cout << "     aayi ya barabar bat ke — counter ke liye ek jaisa hai).\n";
    cout << "\n  2x burst ka PERFECT ilaaj -> SLIDING WINDOW LOG (file 02).\n";
    cout << "  Sasta-sa ilaaj            -> SLIDING WINDOW COUNTER (file 03).\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}

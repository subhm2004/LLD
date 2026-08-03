// ============================================================================
//  #2 — 02_sliding_window_log.cpp
//  SLIDING WINDOW LOG — bilkul sahi jawab, par keemat de kar
// ----------------------------------------------------------------------------
//  Fixed Window ki galti kya thi? Wo "pichhli window" ko poori tarah bhool
//  jaata tha, isliye boundary pe 2x nikal jaata tha.
//
//  ⭐ Sliding Window Log ka jawab bahut seedha hai: bhoolo mat. HAR accepted
//     request ka TIMESTAMP yaad rakho.
//
//     Har check pe:
//        1. Jo timestamps `window` second se purane ho gaye, unhe nikaal do
//        2. Ab jitne bache — agar `limit` se kam hain to allow, warna deny
//
//  Ab "window" fixed grid pe nahi baithti — wo har request ke saath KHISAKTI
//  (slide) hai. Isliye "sliding". Aur isme koi boundary hai hi nahi, to
//  boundary burst ka sawaal hi nahi uthta.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `std::deque` KYUN? (interview me poocha jaata hai)                    │
//  │                                                                          │
//  │  Timestamps hamesha BADHTE hue order me aate hain. To purane hamesha      │
//  │  aage (front) se nikalte hain aur naye peeche (back) jama hote hain.      │
//  │                                                                          │
//  │      deque : push_back O(1), pop_front O(1)   ✅                          │
//  │      vector: pop_front O(n) — saare elements shift karne padte  ❌        │
//  │                                                                          │
//  │  Isi liye aapke code me `std::deque<double>` hai. Sahi choice.            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠⚠ ISKI KEEMAT — MEMORY, aur wo LIMIT ke saath badhti hai               │
//  │                                                                          │
//  │  Fixed Window : per key ek counter    -> limit 5 ho ya 10,000, memory same│
//  │  Sliding Log  : per key `limit` tak TIMESTAMPS -> limit badhi, memory badhi│
//  │                                                                          │
//  │  Har timestamp ek `double` = 8 bytes. To:                                 │
//  │      limit=100,  1 crore users -> ~8 GB  😰                               │
//  │      limit=1000, 1 crore users -> ~80 GB 💀                               │
//  │                                                                          │
//  │  Yahi wajah hai ki bade paimane pe log ise use nahi karte — chahe ye      │
//  │  sabse SAHI algorithm ho. Sahi hona kaafi nahi, samaana bhi chahiye.      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ✅ Faayde  : 100% accurate, koi burst nahi, koi approximation nahi
//  ❌ Nuksan  : O(limit) memory PER KEY — bade limit pe bilkul nahi chalega
//  📌 Kab use : limit chhoti ho aur galti bardasht na ho (payment API,
//               OTP bhejna, login attempts — jahan 6th request sach me rukni chahiye)
//
//  Compile: g++ -std=c++17 -Wall -Wextra 02_sliding_window_log.cpp -o sliding_log_demo
// ============================================================================
#include <bits/stdc++.h>

#include "rate_limiter/fixed_window.h"
#include "rate_limiter/sliding_window_log.h"
#include "rl_harness.h"

using namespace std;
using namespace rlx;

int main()
{
    cout << "=========================================================\n";
    cout << " #2 : SLIDING WINDOW LOG\n";
    cout << " limit = 5 requests per 10 second (rolling) window\n";
    cout << "=========================================================\n\n";

    // ---- Part A: ⭐ wahi boundary test jo Fixed Window haar gaya tha -----
    cout << "[A] ⭐ Wahi test jisme Fixed Window fail hua tha:\n\n";
    {
        Probe<rl::FixedWindowCounter> fixedWindow(5, 10.0);
        Probe<rl::SlidingWindowLog> slidingLog(5, 10.0);

        int fixedEnd = sendBurst(fixedWindow, "attacker", 9.99, 5);
        int fixedStart = sendBurst(fixedWindow, "attacker", 10.01, 5);

        int logEnd = sendBurst(slidingLog, "attacker", 9.99, 5);
        int logStart = sendBurst(slidingLog, "attacker", 10.01, 5);

        cout << "                        | t=9.99s | t=10.01s | KUL (0.02s me)\n";
        cout << "    --------------------+---------+----------+---------------\n";
        cout << "    Fixed Window        | " << setw(7) << fixedEnd << " | " << setw(8)
             << fixedStart << " | " << setw(6) << (fixedEnd + fixedStart) << "  ❌ 2x limit\n";
        cout << "    Sliding Window Log  | " << setw(7) << logEnd << " | " << setw(8) << logStart
             << " | " << setw(6) << (logEnd + logStart) << "  ✅ theek limit\n";

        cout << "\n    ⭐ Sliding Log ne t=10.01 pe SAB deny kar di — kyunki uske liye\n";
        cout << "       'pichhle 10 second' ka matlab hai [0.01s, 10.01s], aur us\n";
        cout << "       range me pehle se hi 5 requests (t=9.99 wali) maujood hain.\n";
        cout << "       Uske liye koi 'window boundary' hai hi nahi. ✅\n\n";
    }

    // ---- Part B: window sach me KHISAKTI hai -----------------------------
    cout << "[B] Window kaise khisakti hai — ek-ek karke slot khaali hote hain:\n\n";
    {
        Probe<rl::SlidingWindowLog> limiter(5, 10.0);

        // t=0,1,2,3,4 pe 5 requests
        for (int i = 0; i < 5; ++i) {
            limiter.at("user1", i * 1.0);
        }
        cout << "    t=0,1,2,3,4 pe 5 requests bheji -> log full (5/5)\n\n";

        printDecision(5.0, limiter.at("user1", 5.0), "<-- full, deny");
        printDecision(9.5, limiter.at("user1", 9.5), "<-- abhi bhi full");
        printDecision(10.5, limiter.at("user1", 10.5), "<-- t=0 wali slid out -> 1 slot khaali ✅");
        printDecision(10.6, limiter.at("user1", 10.6), "<-- phir full");
        printDecision(11.5, limiter.at("user1", 11.5), "<-- t=1 wali slid out -> phir jagah ✅");

        cout << "\n    ⭐ Slot ek saath 5 nahi khulte (jaise Fixed Window me hote the) —\n";
        cout << "       ek-ek karke khulte hain, theek 10 second baad. Isi liye traffic\n";
        cout << "       smooth rehta hai aur koi jhatka nahi aata.\n\n";
    }

    // ---- Part C: retry_after sach me sahi hota hai -----------------------
    cout << "[C] `retry_after` bilkul theek batata hai (guess nahi):\n\n";
    {
        Probe<rl::SlidingWindowLog> limiter(3, 10.0);
        limiter.at("user1", 2.0);
        limiter.at("user1", 4.0);
        limiter.at("user1", 6.0);

        rl::Decision d = limiter.at("user1", 7.0);
        cout << "    Requests t=2, 4, 6 pe gayi (limit 3, window 10s)\n";
        cout << "    t=7.0s pe 4th request -> DENY, retry_after = " << fixed << setprecision(2)
             << d.retry_after << "s\n";
        cout << "\n    Check karo: sabse purani request t=2 pe thi, wo t=12 pe window se\n";
        cout << "    bahar jaayegi. 12 - 7 = 5.0 second. Bilkul wahi jawab aaya. ✅\n";
        cout << "\n    ⭐ Ye 'exact' jawab isi algorithm me possible hai, kyunki isi ke\n";
        cout << "       paas har request ka asli timestamp hai. Baaki algorithms\n";
        cout << "       retry_after ka sirf ANDAAZA de paate hain.\n\n";
    }

    // ---- Part D: ⚠ MEMORY — asli keemat --------------------------------
    cout << "[D] ⚠ Ab keemat dekho — MEMORY limit ke saath badhti hai:\n\n";
    cout << "    limit | per key timestamps | 1 crore users ke liye\n";
    cout << "    ------+--------------------+----------------------\n";
    for (int limit : {5, 100, 1000, 10000}) {
        double bytesPerKey = limit * sizeof(double);
        double totalGb = (bytesPerKey * 10000000.0) / (1024.0 * 1024.0 * 1024.0);
        cout << "    " << setw(5) << limit << " | " << setw(14) << (limit * 8) << " B | "
             << setw(17) << fixed << setprecision(2) << totalGb << " GB\n";
    }
    cout << "\n    Iske muqable Fixed Window: 1 crore users pe ~160 MB — aur wo\n";
    cout << "    limit badhne pe BADHTA HI NAHI.\n";
    cout << "\n    ⭐ Yahi is algorithm ki asli seemaa hai. Ye sabse SAHI hai, par\n";
    cout << "       sabse MEHNGA bhi. Isliye ise chhoti limits pe hi lagao.\n";

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ 100% accurate — boundary burst bilkul nahi (upar naap ke dekha).\n";
    cout << "  ✅ `retry_after` bilkul exact deta hai (asli timestamp se).\n";
    cout << "  ✅ Slots ek-ek karke khulte hain — traffic smooth rehta hai.\n";
    cout << "  ❌ O(limit) memory PER KEY — limit 1000 aur 1 crore user = ~74 GB.\n";
    cout << "  ❌ Har check pe purane timestamps nikalne ka kaam (amortized O(1),\n";
    cout << "     par ek hi baar me bahut saare nikal sakte hain).\n";
    cout << "\n  Chahiye: Fixed Window ki memory + Sliding Log ki accuracy.\n";
    cout << "  Wahi beech ka raasta -> SLIDING WINDOW COUNTER (file 03).\n";
    cout << "---------------------------------------------------------\n";
    // ---- VERIFY: sliding log boundary burst rok deta hai -------------------
    {
        Probe<rl::SlidingWindowLog> v(5, 10.0);
        int total = sendBurst(v, "verify", 9.99, 5) + sendBurst(v, "verify", 10.01, 5);
        demo::checkEqual(total, 5, "sliding log me boundary pe limit se zyada nahi nikalna chahiye");
    }

    return demo::report();
}

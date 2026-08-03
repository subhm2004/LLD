// ============================================================================
//  #3 — 03_sliding_window_counter.cpp
//  SLIDING WINDOW COUNTER — 99% accuracy, 0.1% memory
// ----------------------------------------------------------------------------
//  Ab tak do extreme dekhe:
//     Fixed Window      -> memory O(1)     , par 2x boundary burst   ❌
//     Sliding Window Log-> accuracy perfect, par memory O(limit)     ❌
//
//  ⭐ Sliding Window Counter beech ka raasta hai, aur bahut chalaak hai:
//     sirf DO counters rakho — abhi wali window ka, aur pichhli window ka.
//     Phir pichhli window ka utna hissa gino jitna abhi bhi rolling window
//     ke andar aata hai:
//
//        overlap  = 1 - (elapsed / window)
//        estimated = current + previous × overlap
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ EK MISAAL SE POORI BAAT                                               │
//  │                                                                          │
//  │    window = 10s, limit = 100                                             │
//  │    pichhli window [0-10) me 80 requests hui                              │
//  │    abhi window [10-20) me 30 hui, aur abhi t=13s hai                     │
//  │                                                                          │
//  │    elapsed = 13 - 10 = 3s                                                │
//  │    overlap = 1 - 3/10 = 0.7   (pichhli window ka 70% abhi bhi range me)  │
//  │    estimated = 30 + 80 × 0.7 = 30 + 56 = 86                              │
//  │                                                                          │
//  │    86 < 100, to request ALLOW.                                           │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠⚠ WO EK MAAN-LI GAYI BAAT (assumption) JISPE SAB TIKA HAI              │
//  │                                                                          │
//  │  `previous × overlap` ka matlab hai: "hum maan rahe hain ki pichhli       │
//  │  window ki requests us poori window me BARABAR bikhri hui thi."           │
//  │                                                                          │
//  │  Par asal me wo sab ek hi second me bhi ho sakti hain! Tab ye andaaza     │
//  │  galat ho jaata hai — kabhi ZYADA sakht, kabhi ZYADA dheela.              │
//  │                                                                          │
//  │  Ye demo DONO galtiyan naap ke dikhayega (part C aur D) — aur sabse       │
//  │  imaandaar tareeke se: Sliding Window LOG ko "sach" maan kar uske saath   │
//  │  compare karke.                                                          │
//  │                                                                          │
//  │  ⭐ Achhi khabar: asli traffic me requests aksar bikhri hoti hi hain,     │
//  │     isliye practice me ye galti bahut chhoti hoti hai (~1% se kam,        │
//  │     Cloudflare ne apne asli traffic pe ye naapa tha). Isi liye ye         │
//  │     production me sabse zyada use hone wala algorithm hai.                │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ✅ Faayde  : O(1) memory, boundary burst khatam, practice me ~99% accurate
//  ❌ Nuksan  : approximation hai — extreme bursty pattern pe thoda galat
//  📌 Kab use : DEFAULT choice. Cloudflare, Kong, aur zyadatar API gateways yahi
//               use karte hain. Jab tak koi khaas wajah na ho, yahi lagao.
//
//  Compile: g++ -std=c++17 -Wall -Wextra 03_sliding_window_counter.cpp -o sliding_counter_demo
// ============================================================================
#include <bits/stdc++.h>

#include "rate_limiter/fixed_window.h"
#include "rate_limiter/sliding_window_counter.h"
#include "rate_limiter/sliding_window_log.h"
#include "rl_harness.h"

using namespace std;
using namespace rlx;

int main()
{
    cout << "=========================================================\n";
    cout << " #3 : SLIDING WINDOW COUNTER\n";
    cout << " estimated = current + previous x overlap_fraction\n";
    cout << "=========================================================\n\n";

    // ---- Part A: boundary burst theek hua? --------------------------------
    cout << "[A] Wahi boundary test (jisme Fixed Window 2x de deta tha):\n\n";
    {
        Probe<rl::FixedWindowCounter> fixedWindow(5, 10.0);
        Probe<rl::SlidingWindowCounter> counter(5, 10.0);
        Probe<rl::SlidingWindowLog> log(5, 10.0);

        int fw = sendBurst(fixedWindow, "a", 9.99, 5) + sendBurst(fixedWindow, "a", 10.01, 5);
        int sc = sendBurst(counter, "a", 9.99, 5) + sendBurst(counter, "a", 10.01, 5);
        int sl = sendBurst(log, "a", 9.99, 5) + sendBurst(log, "a", 10.01, 5);

        cout << "    Fixed Window          : " << fw << " requests  ❌ (2x limit)\n";
        cout << "    Sliding Window Counter: " << sc << " requests  ✅\n";
        cout << "    Sliding Window Log    : " << sl << " requests  ✅ (sach)\n";
        cout << "\n    ⭐ Counter ne bhi burst rok diya — aur wo bhi sirf 2 int rakh ke,\n";
        cout << "       Log ki tarah 5 timestamps rakhe bina.\n\n";
    }

    // ---- Part B: formula ko chalte hue dekho -----------------------------
    cout << "[B] Formula live me — pichhli window ka wazan ghatta jaata hai:\n\n";
    {
        Probe<rl::SlidingWindowCounter> limiter(100, 10.0);

        // Pichhli window [0,10) me 80 requests
        sendBurst(limiter, "user1", 5.0, 80);

        cout << "    Pichhli window [0-10) me 80 requests hui thi.\n";
        cout << "    Ab dekho agli window me estimate kaise girta hai:\n\n";
        cout << "     time  | overlap | estimated (= current + 80 x overlap)\n";
        cout << "    -------+---------+-------------------------------------\n";

        for (double t : {10.5, 12.0, 15.0, 18.0, 19.5}) {
            // Ek probe request bhej ke `used` padh lete hain
            Probe<rl::SlidingWindowCounter> fresh(100, 10.0);
            sendBurst(fresh, "user1", 5.0, 80);
            rl::Decision d = fresh.at("user1", t);

            double overlap = 1.0 - (t - 10.0) / 10.0;
            cout << "    " << setw(5) << fixed << setprecision(1) << t << "s | " << setw(7)
                 << setprecision(2) << overlap << " | " << setw(6) << d.used << "\n";
        }
        cout << "\n    ⭐ Jaise-jaise waqt aage badhta hai, pichhli window ka asar\n";
        cout << "       dheere-dheere khatam hota jaata hai — ek jhatke me nahi.\n";
        cout << "       Yahi cheez Fixed Window me nahi thi (wahan counter ek dam 0).\n\n";
    }

    // ---- Part C: ⚠ GALTI #1 — zyada SAKHT ------------------------------
    cout << "[C] ⚠ Approximation ki GALTI #1 — zaroorat se zyada SAKHT:\n\n";
    {
        Probe<rl::SlidingWindowCounter> counter(5, 10.0);
        Probe<rl::SlidingWindowLog> log(5, 10.0);

        // Saari 5 requests window ke BILKUL SHURU me (t=0.0)
        sendBurst(counter, "u", 0.0, 5);
        sendBurst(log, "u", 0.0, 5);

        rl::Decision dc = counter.at("u", 10.1);
        rl::Decision dl = log.at("u", 10.1);

        cout << "    Scenario: saari 5 requests t=0.0s pe hui (window ke bilkul shuru me)\n";
        cout << "    Ab t=10.1s pe nayi request aayi.\n\n";
        cout << "    SACH kya hai? Rolling window = [0.1s, 10.1s]\n";
        cout << "                  Us range me requests = 0 (sab t=0.0 pe thi, bahar hai)\n";
        cout << "                  To ALLOW hona chahiye.\n\n";
        cout << "    Sliding Window Log     : " << (dl.allowed ? "✅ ALLOW" : "❌ DENY")
             << "   <-- sahi\n";
        cout << "    Sliding Window Counter : " << (dc.allowed ? "✅ ALLOW" : "❌ DENY")
             << "   used=" << fixed << setprecision(2) << dc.used << " <-- GALAT\n";
        cout << "\n    Kyun? Counter ne socha: previous=5, overlap=0.99\n";
        cout << "          => estimated = 5 x 0.99 = 4.95, aur 4.95+1 > 5 to DENY.\n";
        cout << "    Usne maan liya ki wo 5 requests poore 10 second me bikhri thi,\n";
        cout << "    jabki wo sab pehle hi second me ho chuki thi. ⭐\n\n";
    }

    // ---- Part D: ⚠ GALTI #2 — zyada DHEELA -----------------------------
    cout << "[D] ⚠ Approximation ki GALTI #2 — zaroorat se zyada DHEELA:\n\n";
    {
        Probe<rl::SlidingWindowCounter> counter(5, 10.0);
        Probe<rl::SlidingWindowLog> log(5, 10.0);

        // Saari 5 requests window ke BILKUL ANT me (t=9.9)
        sendBurst(counter, "u", 9.9, 5);
        sendBurst(log, "u", 9.9, 5);

        int counterAllowed = sendBurst(counter, "u", 15.0, 5);
        int logAllowed = sendBurst(log, "u", 15.0, 5);

        cout << "    Scenario: saari 5 requests t=9.9s pe hui (window ke bilkul ant me)\n";
        cout << "    Ab t=15.0s pe 5 aur requests aayi.\n\n";
        cout << "    SACH kya hai? Rolling window = [5.0s, 15.0s]\n";
        cout << "                  Us range me pehle se 5 requests hain (t=9.9 wali)\n";
        cout << "                  To 0 allow honi chahiye.\n\n";
        cout << "    Sliding Window Log     : " << logAllowed << " allow  <-- sahi\n";
        cout << "    Sliding Window Counter : " << counterAllowed
             << " allow  <-- GALAT (zyada de di)\n";
        cout << "\n    Kyun? Counter ne socha: previous=5, overlap=0.5\n";
        cout << "          => estimated = 5 x 0.5 = 2.5, to 2 aur requests ki jagah bani.\n";
        cout << "    Par asal me wo 5 requests abhi bhi poori window ke andar hain. ⭐\n\n";
    }

    // ---- Part E: memory ka faayda ----------------------------------------
    cout << "[E] Aur iske badle me mila kya — MEMORY:\n\n";
    cout << "    algorithm              | per key state          | limit=1000, 1 crore users\n";
    cout << "    -----------------------+------------------------+---------------------------\n";
    cout << "    Fixed Window           | 1 counter + start      | ~160 MB\n";
    cout << "    Sliding Window Counter | 2 counters + start     | ~240 MB\n";
    cout << "    Sliding Window Log     | up to 1000 timestamps  | ~74 GB  💀\n";
    cout << "\n    ⭐ Log se ~300 GUNA kam memory, aur accuracy phir bhi practice me\n";
    cout << "       ~99%. Yahi is algorithm ka poora business case hai.\n";

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Boundary burst khatam — Fixed Window ki 2x wali problem gayi.\n";
    cout << "  ✅ O(1) memory (sirf 2 counters) — Log se ~300x sasta.\n";
    cout << "  ✅ Pichhli window ka asar dheere-dheere khatam hota hai (smooth).\n";
    cout << "  ⚠ Approximation hai: maanta hai ki requests barabar bikhri thi.\n";
    cout << "     - sab requests window ke SHURU me  -> zyada SAKHT ho jaata\n";
    cout << "     - sab requests window ke ANT me    -> zyada DHEELA ho jaata\n";
    cout << "  📌 Asli traffic me requests bikhri hoti hain, isliye ye galti\n";
    cout << "     bahut chhoti hoti hai. Isliye ye DEFAULT choice hai\n";
    cout << "     (Cloudflare, Kong, zyadatar API gateways).\n";
    cout << "\n  Ab tak sab 'ginti' wale the. Ek alag soch -> TOKEN BUCKET (file 04).\n";
    cout << "---------------------------------------------------------\n";
    // ---- VERIFY: counter bhi boundary burst rokta hai ---------------------
    {
        Probe<rl::SlidingWindowCounter> v(5, 10.0);
        int total = sendBurst(v, "verify", 9.99, 5) + sendBurst(v, "verify", 10.01, 5);
        demo::check(total <= 5, "sliding counter ko boundary burst rokna chahiye");
    }

    return demo::report();
}

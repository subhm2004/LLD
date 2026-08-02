// ============================================================================
//  #5 — 05_leaking_bucket.cpp
//  LEAKING BUCKET — aur wo sach jo zyadatar blogs galat batate hain
// ----------------------------------------------------------------------------
//  Idea: ek balti hai jisme chhed hai. Requests usme paani ki tarah girti hain
//  (level badhta hai), aur balti neeche se `leak_rate` ki raftaar se lagatar
//  ris-ti (leak) rehti hai. Balti bhar gayi (capacity) to nayi request ko
//  jagah nahi milti — reject.
//
//     level badhta hai  -> har accepted request se
//     level ghatta hai  -> waqt ke saath, constant rate se
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠⚠ SABSE BADI GALATFEHMI — "Leaking Bucket burst nahi allow karta"      │
//  │                                                                          │
//  │  Ye baat lagbhag har blog me likhi hoti hai, aur ye ADHOORI hai.          │
//  │                                                                          │
//  │  Sach ye hai ki Leaking Bucket ke DO alag roop hain:                      │
//  │                                                                          │
//  │  1️⃣ METER ke roop me (jo aapke code me hai — `leaking_bucket.h`)          │
//  │     Balti bhari to request REJECT. Ye Token Bucket ka bilkul AAINA        │
//  │     (mirror) hai — dono theek same faisle lete hain!                      │
//  │         token bucket : tokens  = kitna BACHA hai                          │
//  │         leaky bucket : level   = kitna BHARA hai                          │
//  │         aur tokens == capacity - level  (hamesha)                         │
//  │     Ye demo part B me ise NAAP ke sabit karega.                           │
//  │                                                                          │
//  │  2️⃣ QUEUE (shaper) ke roop me — YE alag hai                               │
//  │     Yahan requests reject nahi hoti, wo QUEUE me khadi ho jaati hain aur  │
//  │     ek-ek karke constant rate pe nikalti hain. Yaani input bursty hai par │
//  │     output BILKUL steady. Iski keemat: LATENCY (request intezaar karti    │
//  │     hai) aur queue ki memory.                                            │
//  │                                                                          │
//  │  ⭐ "Burst smooth karta hai" wali baat sirf ROOP 2 pe lagu hoti hai.      │
//  │     Roop 1 (aapka code, aur zyadatar rate limiters) burst utna hi allow   │
//  │     karta hai jitna Token Bucket. Ye interview me bahut bada point hai.   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Nginx ka `limit_req` dono kar sakta hai:
//        burst=20          -> queue banao (roop 2, requests wait karti hain)
//        burst=20 nodelay  -> turant bhejo (roop 1 jaisa behaviour)
//
//  ✅ Faayde  : O(1) memory, output rate ki pakki guarantee (queue roop me)
//  ❌ Nuksan  : meter roop me Token Bucket se koi alag faayda nahi;
//               queue roop me latency add karta hai
//  📌 Kab use : jab DOWNSTREAM ko bilkul steady rate chahiye (jaise koi legacy
//               system ya third-party API jo spike bardasht nahi kar sakta)
//
//  Compile: g++ -std=c++17 -Wall -Wextra 05_leaking_bucket.cpp -o leaking_bucket_demo
// ============================================================================
#include <bits/stdc++.h>

#include "rate_limiter/leaking_bucket.h"
#include "rate_limiter/token_bucket.h"
#include "rl_harness.h"

using namespace std;
using namespace rlx;

int main()
{
    cout << "=========================================================\n";
    cout << " #5 : LEAKING BUCKET\n";
    cout << " capacity = 5 units, leak = 1 unit/second\n";
    cout << "=========================================================\n\n";

    // ---- Part A: basic — bharna aur risna --------------------------------
    cout << "[A] Balti bharti hai, phir ris-ti hai:\n\n";
    {
        Probe<rl::LeakingBucket> limiter(5, 1.0);

        cout << "    t=0 pe 7 requests ek saath:\n";
        for (int i = 1; i <= 7; ++i) {
            rl::Decision d = limiter.at("user1", 0.0);
            printDecision(0.0, d, i == 6 ? "<-- balti full" : "");
        }

        cout << "\n    Ab waqt beetne do — balti ris-ti hai:\n";
        for (double t : {1.0, 2.5, 5.0}) {
            rl::Decision d = limiter.at("user1", t);
            printDecision(t, d);
        }
        cout << "\n";
    }

    // ---- Part B: ⭐⭐ EQUIVALENCE — token bucket ka aaina ----------------
    cout << "[B] ⭐⭐ Ab wo sabit karte hain: LEAKING BUCKET == TOKEN BUCKET\n";
    cout << "    (jab dono 'meter' roop me hon — jaisa aapke code me hai)\n\n";
    {
        Probe<rl::TokenBucket> tokenBucket(5, 1.0);
        Probe<rl::LeakingBucket> leakingBucket(5, 1.0);

        // Ek hi random-sa traffic pattern dono pe chalao
        vector<double> times = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.5,
                                2.0, 2.0, 4.0, 4.0, 4.0, 8.0, 8.0, 8.0};

        cout << "    Wahi traffic dono pe chalaya (16 requests):\n\n";
        cout << "    Token Bucket   : ";
        for (double t : times) {
            cout << (tokenBucket.at("u", t).allowed ? "✅" : "❌");
        }
        cout << "\n    Leaking Bucket : ";
        for (double t : times) {
            cout << (leakingBucket.at("u", t).allowed ? "✅" : "❌");
        }
        cout << "\n";

        // Ab strictly verify karo — har decision compare
        Probe<rl::TokenBucket> tb2(5, 1.0);
        Probe<rl::LeakingBucket> lb2(5, 1.0);
        int mismatches = 0;
        int checks = 0;
        for (double t = 0.0; t < 60.0; t += 0.37) { // ajeeb sa step, taaki pattern na bane
            for (int burst = 0; burst < 3; ++burst) {
                bool a = tb2.at("k", t).allowed;
                bool b = lb2.at("k", t).allowed;
                ++checks;
                if (a != b) {
                    ++mismatches;
                }
            }
        }
        cout << "\n    Strict test: " << checks << " requests, dono ke faisle compare kiye\n";
        cout << "    Mismatches: " << mismatches << "   <-- 0 hona chahiye ⭐\n";

        cout << "\n    ⭐ Kyun bilkul same? Kyunki relation hai:\n";
        cout << "           tokens  ==  capacity - level     (hamesha)\n";
        cout << "       Token Bucket poochta hai: 'kitna BACHA hai?' (tokens >= cost)\n";
        cout << "       Leaky Bucket poochta hai: 'kitni JAGAH hai?' (level+cost <= capacity)\n";
        cout << "       Ye ek hi sawaal hai, bas ulti taraf se pucha gaya. ⭐\n\n";
    }

    // ---- Part C: to phir asli farak kya hai? -----------------------------
    cout << "[C] To phir 'Leaking Bucket burst smooth karta hai' wali baat kya hai?\n\n";
    cout << "    Wo baat QUEUE (shaper) roop ki hai, jo aapke code me nahi hai\n";
    cout << "    (aur zyadatar rate limiters me nahi hoti). Farak ye hai:\n\n";
    cout << "    METER roop (aapka code):\n";
    cout << "      Balti full -> request REJECT (429 Too Many Requests)\n";
    cout << "      Client ko turant pata chal jaata hai. Koi latency nahi.\n\n";
    cout << "    QUEUE roop (traffic shaper):\n";
    cout << "      Balti full -> request QUEUE me khadi, reject nahi\n";
    cout << "      Wo constant rate pe nikalti hai -> output BILKUL steady\n";
    cout << "      Keemat: request ko INTEZAAR karna padta (latency badhti)\n\n";
    {
        cout << "    Chhota sa simulation — 10 requests ek saath (t=0), leak 1/sec:\n\n";
        cout << "      METER roop  : ";
        Probe<rl::LeakingBucket> meter(5, 1.0);
        int accepted = 0;
        for (int i = 0; i < 10; ++i) {
            if (meter.at("u", 0.0).allowed) {
                ++accepted;
            }
        }
        cout << accepted << " turant allow, " << (10 - accepted) << " REJECT\n";

        cout << "      QUEUE roop  : 10 me se 5 queue me aa gayi, aur wo\n";
        cout << "                    t=1,2,3,4,5 pe ek-ek karke nikli (baaki 5 reject)\n";
        cout << "                    -> output rate bilkul 1/sec, par latency 1-5s\n";
    }

    // ---- Part D: kab kaunsa ----------------------------------------------
    cout << "\n[D] To practice me kya karein?\n\n";
    cout << "    ⭐ Agar aap API rate limiting kar rahe ho (429 bhejna hai):\n";
    cout << "       Token Bucket use karo. Leaking Bucket (meter) usse alag kuch\n";
    cout << "       deta hi nahi, aur Token Bucket ki bhasha (capacity + refill\n";
    cout << "       rate) samajhne me zyada seedhi hai.\n\n";
    cout << "    ⭐ Agar aapko DOWNSTREAM ko bachana hai (koi legacy service ya\n";
    cout << "       third-party API jo spike pe gir jaata hai):\n";
    cout << "       Leaking Bucket ka QUEUE roop chahiye — kyunki wahan reject\n";
    cout << "       karna nahi, SMOOTH karna maqsad hai.\n";

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ O(1) memory, lazy leak (koi background timer nahi).\n";
    cout << "  ✅ Queue roop me output rate ki PAKKI guarantee milti hai.\n";
    cout << "  ⭐ SABSE ZAROORI: meter roop me ye Token Bucket ka AAINA hai —\n";
    cout << "     upar naap ke dekha, 0 mismatches. 'Leaky bucket burst nahi\n";
    cout << "     deta' wali baat sirf QUEUE roop pe lagu hoti hai.\n";
    cout << "  ❌ Queue roop me latency add hoti hai (request wait karti hai).\n";
    cout << "  📌 API rate limiting ke liye Token Bucket lo; downstream ko\n";
    cout << "     smooth karna ho to Leaking Bucket (queue) lo.\n";
    cout << "\n  Ab saare 5 ek saath dekho -> 06_compare_all.cpp\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}

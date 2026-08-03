// ============================================================================
//  #6 — 06_compare_all.cpp
//  SAARE 5 ALGORITHMS, EK HI TRAFFIC PE — side by side
// ----------------------------------------------------------------------------
//  Har algorithm ki apni file me humne uski ek khaas baat dekhi. Ab sabko EK
//  SAATH, BILKUL SAME traffic pe chala kar dekhte hain — kyunki asli samajh
//  tabhi aati hai jab farak aankhon ke saamne ho.
//
//  ⭐ Sabka config aisa rakha hai ki unka "matlab" ek jaisa ho:
//        lagbhag 5 requests per 10 second
//
//     FixedWindow(5, 10)            -> 5 per 10s window
//     SlidingWindowLog(5, 10)       -> 5 per rolling 10s
//     SlidingWindowCounter(5, 10)   -> 5 per rolling 10s (approx)
//     TokenBucket(5, 0.5)           -> capacity 5, 0.5 token/s = 5 per 10s
//     LeakingBucket(5, 0.5)         -> capacity 5, 0.5 leak/s = 5 per 10s
//
//  Config "barabar" hone ke baad bhi inka BEHAVIOUR bahut alag hai — aur yahi
//  is file ka poora point hai.
//
//  Compile: g++ -std=c++17 -Wall -Wextra 06_compare_all.cpp -o compare_demo
// ============================================================================
#include <bits/stdc++.h>

#include "rate_limiter/fixed_window.h"
#include "rate_limiter/leaking_bucket.h"
#include "rate_limiter/sliding_window_counter.h"
#include "rate_limiter/sliding_window_log.h"
#include "rate_limiter/token_bucket.h"
#include "rl_harness.h"

using namespace std;
using namespace rlx;

// Har scenario ke liye fresh limiters banane padte hain (state carry na ho).
struct Limiters {
    Probe<rl::FixedWindowCounter> fixedWindow{5, 10.0};
    Probe<rl::SlidingWindowLog> slidingLog{5, 10.0};
    Probe<rl::SlidingWindowCounter> slidingCounter{5, 10.0};
    Probe<rl::TokenBucket> tokenBucket{5, 0.5};
    Probe<rl::LeakingBucket> leakingBucket{5, 0.5};
};

int main()
{
    cout << "=========================================================\n";
    cout << " #6 : SAARE 5 ALGORITHMS KA MUQABLA\n";
    cout << " Sabka config: ~5 requests per 10 second\n";
    cout << "=========================================================\n\n";

    // ---- Scenario 1: boundary attack -------------------------------------
    cout << "[1] BOUNDARY ATTACK — t=9.99s pe 5, phir t=10.01s pe 5\n";
    cout << "    (0.02 second me kul 10 requests. Limit 5 per 10s thi.)\n\n";
    {
        Limiters lim;
        struct Row { const char *name; int a, b; };
        vector<Row> rows;

        rows.push_back({"Fixed Window", sendBurst(lim.fixedWindow, "u", 9.99, 5),
                        sendBurst(lim.fixedWindow, "u", 10.01, 5)});
        rows.push_back({"Sliding Window Log", sendBurst(lim.slidingLog, "u", 9.99, 5),
                        sendBurst(lim.slidingLog, "u", 10.01, 5)});
        rows.push_back({"Sliding Window Counter", sendBurst(lim.slidingCounter, "u", 9.99, 5),
                        sendBurst(lim.slidingCounter, "u", 10.01, 5)});
        rows.push_back({"Token Bucket", sendBurst(lim.tokenBucket, "u", 9.99, 5),
                        sendBurst(lim.tokenBucket, "u", 10.01, 5)});
        rows.push_back({"Leaking Bucket", sendBurst(lim.leakingBucket, "u", 9.99, 5),
                        sendBurst(lim.leakingBucket, "u", 10.01, 5)});

        cout << "    algorithm              | t=9.99 | t=10.01 | KUL | nateeja\n";
        cout << "    -----------------------+--------+---------+-----+---------\n";
        for (const Row &r : rows) {
            int total = r.a + r.b;
            cout << "    " << left << setw(22) << r.name << " | " << right << setw(6) << r.a
                 << " | " << setw(7) << r.b << " | " << setw(3) << total << " | "
                 << (total > 5 ? "❌ limit toot gayi" : "✅ theek") << "\n"
                 << left;
        }
        cout << right;
        cout << "\n    ⭐ Sirf Fixed Window fail hua. Baaki chaaron ne rok liya.\n\n";
    }

    // ---- Scenario 2: instant burst (thanda start) ------------------------
    cout << "[2] INSTANT BURST — bilkul shuru me (t=0) ek saath 10 requests\n\n";
    {
        Limiters lim;
        cout << "    Fixed Window           : " << sendBurst(lim.fixedWindow, "u", 0.0, 10)
             << " allow\n";
        cout << "    Sliding Window Log     : " << sendBurst(lim.slidingLog, "u", 0.0, 10)
             << " allow\n";
        cout << "    Sliding Window Counter : " << sendBurst(lim.slidingCounter, "u", 0.0, 10)
             << " allow\n";
        cout << "    Token Bucket           : " << sendBurst(lim.tokenBucket, "u", 0.0, 10)
             << " allow\n";
        cout << "    Leaking Bucket         : " << sendBurst(lim.leakingBucket, "u", 0.0, 10)
             << " allow\n";
        cout << "\n    ⭐ Yahan sab barabar — sabne 5 di. Thande start pe koi farak nahi.\n\n";
    }

    // ---- Scenario 3: recovery pattern ------------------------------------
    cout << "[3] RECOVERY — quota khatam karke, har 1 second pe try karo\n";
    cout << "    (t=0 pe 5 requests se quota khatam, phir t=1..20 pe ek-ek)\n\n";
    {
        Limiters lim;
        sendBurst(lim.fixedWindow, "u", 0.0, 5);
        sendBurst(lim.slidingLog, "u", 0.0, 5);
        sendBurst(lim.slidingCounter, "u", 0.0, 5);
        sendBurst(lim.tokenBucket, "u", 0.0, 5);
        sendBurst(lim.leakingBucket, "u", 0.0, 5);

        cout << "                             t=1 -> t=20 (har second ek request)\n";
        cout << "    Fixed Window           : ";
        printTimeline(lim.fixedWindow, "u", 1.0, 1.0, 20);
        cout << "    Sliding Window Log     : ";
        printTimeline(lim.slidingLog, "u", 1.0, 1.0, 20);
        cout << "    Sliding Window Counter : ";
        printTimeline(lim.slidingCounter, "u", 1.0, 1.0, 20);
        cout << "    Token Bucket           : ";
        printTimeline(lim.tokenBucket, "u", 1.0, 1.0, 20);
        cout << "    Leaking Bucket         : ";
        printTimeline(lim.leakingBucket, "u", 1.0, 1.0, 20);

        cout << "\n    ⭐ Ab PATTERN ka farak saaf dikh raha hai:\n";
        cout << "       Fixed Window  -> t=10 pe ACHANAK 5 slot ek saath khul gaye (jhatka)\n";
        cout << "       Sliding Log   -> t=10 pe khule, par exact 10s baad (sahi)\n";
        cout << "       Token/Leaky   -> har 2 second me ek (0.5/sec) — bilkul SMOOTH drip\n";
        cout << "\n       Yahi buckets ki khaasiyat hai: wo quota ek saath nahi dete,\n";
        cout << "       lagatar thoda-thoda dete rehte hain. Isliye backend pe\n";
        cout << "       traffic sthir rehta hai — koi 'saw-tooth' spike nahi.\n\n";
    }

    // ---- Scenario 4: idle user ka quota ----------------------------------
    cout << "[4] IDLE USER — quota khatam karke 1 ghanta chup, phir 10 requests\n\n";
    {
        Limiters lim;
        sendBurst(lim.fixedWindow, "u", 0.0, 5);
        sendBurst(lim.slidingLog, "u", 0.0, 5);
        sendBurst(lim.slidingCounter, "u", 0.0, 5);
        sendBurst(lim.tokenBucket, "u", 0.0, 5);
        sendBurst(lim.leakingBucket, "u", 0.0, 5);

        cout << "    Fixed Window           : " << sendBurst(lim.fixedWindow, "u", 3600.0, 10)
             << " allow\n";
        cout << "    Sliding Window Log     : " << sendBurst(lim.slidingLog, "u", 3600.0, 10)
             << " allow\n";
        cout << "    Sliding Window Counter : " << sendBurst(lim.slidingCounter, "u", 3600.0, 10)
             << " allow\n";
        cout << "    Token Bucket           : " << sendBurst(lim.tokenBucket, "u", 3600.0, 10)
             << " allow\n";
        cout << "    Leaking Bucket         : " << sendBurst(lim.leakingBucket, "u", 3600.0, 10)
             << " allow\n";
        cout << "\n    ⭐ Sab 5 pe cap ho gaye — koi bhi '1 ghante ka jama hua quota'\n";
        cout << "       nahi deta. Buckets me `min(capacity, ...)` isi liye hai.\n";
        cout << "       Warna chup baitha user baad me 3600 requests maar deta. 😅\n\n";
    }

    // ---- Summary table ---------------------------------------------------
    cout << "=========================================================\n";
    cout << " SUMMARY\n";
    cout << "=========================================================\n\n";

    cout << "    algorithm              | memory/key | accuracy | burst allow?\n";
    cout << "    -----------------------+------------+----------+-------------\n";
    cout << "    Fixed Window           | O(1)       | ❌ 2x    | haan (galti se)\n";
    cout << "    Sliding Window Log     | O(limit)   | ✅ 100%  | nahi\n";
    cout << "    Sliding Window Counter | O(1)       | ~99%     | nahi\n";
    cout << "    Token Bucket           | O(1)       | ✅       | HAAN (jaan-bujh ke)\n";
    cout << "    Leaking Bucket (meter) | O(1)       | ✅       | HAAN (token bucket jaisa)\n";

    cout << "\n    KAB KYA USE KAREIN:\n\n";
    cout << "    • Public API bana rahe ho (burst allow karna hai)\n";
    cout << "         -> TOKEN BUCKET  (Stripe/GitHub/AWS yahi use karte hain)\n\n";
    cout << "    • General purpose, crore users, memory bachani hai\n";
    cout << "         -> SLIDING WINDOW COUNTER  (Cloudflare/Kong ka default)\n\n";
    cout << "    • Limit chhoti hai aur ek bhi extra request nahi jaani chahiye\n";
    cout << "      (OTP, login attempts, payment)\n";
    cout << "         -> SLIDING WINDOW LOG  (mehnga hai, par exact)\n\n";
    cout << "    • Downstream ko bilkul steady rate chahiye\n";
    cout << "         -> LEAKING BUCKET (queue roop)\n\n";
    cout << "    • Bas mota-mota quota, sabse sasta chahiye\n";
    cout << "         -> FIXED WINDOW  (par 2x burst maan ke chalna)\n";

    cout << "\n    ⭐ TEEN BAATEIN JO YAAD RAKHNI HAIN:\n";
    cout << "      1. Fixed Window boundary pe HAMESHA 2x deta hai — hisaab me rakho.\n";
    cout << "      2. Leaking Bucket (meter) aur Token Bucket EK HI cheez hain\n";
    cout << "         (05 wali file me 0 mismatches se sabit kiya).\n";
    cout << "      3. Window algorithms quota JHATKE me dete hain, buckets DRIP\n";
    cout << "         karke — backend ke liye drip behtar hai.\n";
    cout << "---------------------------------------------------------\n";
    // ---- VERIFY: sirf fixed window hi boundary pe limit todta hai ---------
    {
        Limiters v;
        demo::checkEqual(sendBurst(v.fixedWindow, "v", 9.99, 5) + sendBurst(v.fixedWindow, "v", 10.01, 5),
                         10, "fixed window boundary pe 2x deta hai");
        demo::checkEqual(sendBurst(v.slidingLog, "v", 9.99, 5) + sendBurst(v.slidingLog, "v", 10.01, 5),
                         5, "sliding log boundary pe limit nahi todta");
        demo::checkEqual(sendBurst(v.tokenBucket, "v", 9.99, 5) + sendBurst(v.tokenBucket, "v", 10.01, 5),
                         5, "token bucket boundary pe limit nahi todta");
    }

    return demo::report();
}

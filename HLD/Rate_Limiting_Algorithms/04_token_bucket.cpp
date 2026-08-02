// ============================================================================
//  #4 — 04_token_bucket.cpp
//  TOKEN BUCKET — burst allow karta hai, par control me
// ----------------------------------------------------------------------------
//  Ab tak ke teeno algorithms "ginti" wale the: window me kitni requests hui.
//  Token Bucket ki soch bilkul alag hai — wo SIKKON (tokens) ki soch hai.
//
//     * Ek balti (bucket) hai jisme zyada se zyada `capacity` tokens aa sakte
//     * Usme lagatar `refill_rate` tokens/second gir te rehte hain
//     * Har request ko chalne ke liye `cost` tokens KHARCH karne padte hain
//     * Tokens nahi bache? Request deny.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ DO ALAG-ALAG KNOB — aur yahi iski sabse badi taakat hai               │
//  │                                                                          │
//  │      capacity     -> "ek saath kitna burst chalega"  (BURST control)      │
//  │      refill_rate  -> "lambe samay me kitni rate"     (RATE control)       │
//  │                                                                          │
//  │  Window wale algorithms me ye dono ek hi cheez se bandhe the — `limit`.   │
//  │  Yahan alag-alag hain. Iska matlab aap keh sakte ho:                      │
//  │                                                                          │
//  │      "average 10 req/sec, par ek saath 100 tak ka burst chalega"          │
//  │       (refill_rate = 10, capacity = 100)                                  │
//  │                                                                          │
//  │  Ye asli duniya ke liye bahut kaam ki baat hai — user aksar ek page       │
//  │  khulte hi 20 API calls maarta hai, phir chup ho jaata hai. Wo BURST      │
//  │  bilkul jayaz hai, use rokna user experience kharab karta hai.            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ "LAZY REFILL" — ek bahut khoobsurat trick (aapke code me hai)         │
//  │                                                                          │
//  │  Tokens ko har second add karne ke liye koi background thread/timer NAHI  │
//  │  chahiye. Aapka code sirf itna karta hai:                                 │
//  │                                                                          │
//  │      elapsed = now - b.last;                                              │
//  │      b.tokens = min(capacity, b.tokens + elapsed * refill_rate);          │
//  │                                                                          │
//  │  Yaani "jab koi poochne aaye, TAB hisaab lagao". Agar 3 ghante koi        │
//  │  request nahi aayi, to 3 ghante kuch nahi chala — agli request pe ek      │
//  │  multiplication me saara refill ho gaya.                                  │
//  │                                                                          │
//  │  ⭐ Isi liye ye crore users pe bhi chalta hai: koi timer nahi, koi cron   │
//  │     nahi, koi background job nahi. Sirf ek `last` timestamp. Yahi trick   │
//  │     leaking bucket me bhi lagi hai (file 05).                             │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ✅ Faayde  : burst aur rate alag-alag set kar sakte ho; O(1) memory; smooth
//  ❌ Nuksan  : do parameter tune karne padte hain (ek nahi)
//  📌 Kab use : public APIs (Stripe, GitHub, AWS — sab yahi use karte hain),
//               jahan burst allow karna hai par average bandha rakhna hai
//
//  Compile: g++ -std=c++17 -Wall -Wextra 04_token_bucket.cpp -o token_bucket_demo
// ============================================================================
#include <bits/stdc++.h>

#include "rate_limiter/sliding_window_counter.h"
#include "rate_limiter/token_bucket.h"
#include "rl_harness.h"

using namespace std;
using namespace rlx;

int main()
{
    cout << "=========================================================\n";
    cout << " #4 : TOKEN BUCKET\n";
    cout << " capacity = 5 tokens, refill = 1 token/second\n";
    cout << "=========================================================\n\n";

    // ---- Part A: balti bhari hui shuru hoti hai -> burst chalta hai -------
    cout << "[A] Balti BHARI hui shuru hoti hai — isliye pehla burst chal jaata hai:\n\n";
    {
        Probe<rl::TokenBucket> limiter(5, 1.0);
        for (int i = 1; i <= 7; ++i) {
            rl::Decision d = limiter.at("user1", 0.0); // sab t=0 pe (instant burst)
            printDecision(0.0, d, i == 6 ? "<-- tokens khatam" : "");
        }
        cout << "\n    5 requests ek saath nikal gayi (capacity 5 thi), 6th se deny.\n";
        cout << "    ⭐ Ye 'burst' jaan-bujh ke allow kiya gaya hai — bug nahi hai.\n\n";
    }

    // ---- Part B: refill hote hue dekho -----------------------------------
    cout << "[B] Ab tokens waapas bhar te hue dekho (1 token/second):\n\n";
    {
        Probe<rl::TokenBucket> limiter(5, 1.0);
        sendBurst(limiter, "user1", 0.0, 5); // balti khaali kar di

        cout << "    t=0 pe balti khaali kar di. Ab har second ek token aayega:\n\n";
        for (double t : {0.5, 1.0, 2.0, 3.0}) {
            rl::Decision d = limiter.at("user1", t);
            printDecision(t, d);
        }
        cout << "\n    t=0.5 pe deny (abhi aadha token hi bana tha),\n";
        cout << "    t=1.0 se aage har second ek request nikal jaati hai. ✅\n\n";
    }

    // ---- Part C: ⭐ do knob — burst vs rate alag-alag --------------------
    cout << "[C] ⭐ Do knob ka kamaal — same average rate, alag burst:\n\n";
    {
        cout << "    Dono ka long-run rate 2 req/sec hai, par burst alag:\n\n";

        struct Config { double capacity, rate; const char *desc; };
        Config configs[] = {{2, 2.0, "capacity 2  (burst kam, sakht)"},
                            {10, 2.0, "capacity 10 (burst zyada, dheela)"},
                            {50, 2.0, "capacity 50 (burst bahut zyada)"}};

        for (const Config &c : configs) {
            Probe<rl::TokenBucket> limiter(c.capacity, c.rate);
            int burst = sendBurst(limiter, "u", 0.0, 60); // ek saath 60 maaro

            // Ab 10 second tak steady 1 req/sec
            int steady = 0;
            for (int i = 1; i <= 10; ++i) {
                if (limiter.at("u", i * 1.0).allowed) {
                    ++steady;
                }
            }
            cout << "    " << left << setw(34) << c.desc << right << " burst me " << setw(2)
                 << burst << " nikli, phir 10s me " << steady << " aur\n";
        }
        cout << "\n    ⭐ Capacity badhane se sirf BURST bada — long-run rate wahi 2/sec.\n";
        cout << "       Window algorithms me ye alag-alag set karna possible hi nahi tha.\n\n";
    }

    // ---- Part D: window algorithm se farak -------------------------------
    cout << "[D] Token Bucket vs Sliding Window Counter — dono 5 per 5s pe:\n\n";
    {
        // Token bucket: capacity 5, refill 1/sec => long-run 5 per 5 sec
        Probe<rl::TokenBucket> bucket(5, 1.0);
        Probe<rl::SlidingWindowCounter> window(5, 5.0);

        cout << "    Traffic: har 0.5 second pe 1 request, 20 requests\n\n";
        cout << "    Token Bucket    : ";
        for (int i = 0; i < 20; ++i) {
            cout << (bucket.at("u", i * 0.5).allowed ? "✅" : "❌");
        }
        cout << "\n    Sliding Counter : ";
        for (int i = 0; i < 20; ++i) {
            cout << (window.at("u", i * 0.5).allowed ? "✅" : "❌");
        }
        cout << "\n\n    ⭐ Dono ~same rate rok rahe hain, par PATTERN alag hai:\n";
        cout << "       Token Bucket   -> pehle burst, phir steady drip (tokens\n";
        cout << "                         jaise-jaise bante hain waise-waise)\n";
        cout << "       Sliding Window -> chunks me khulta hai (window khisakne pe)\n\n";
    }

    // ---- Part E: idle user ka faayda -------------------------------------
    cout << "[E] Lazy refill ka faayda — jo user chup baitha tha:\n\n";
    {
        Probe<rl::TokenBucket> limiter(5, 1.0);
        sendBurst(limiter, "user1", 0.0, 5); // balti khaali

        cout << "    t=0 pe balti khaali. Ab user 1 ghante chup baitha (t=3600).\n";
        int afterIdle = sendBurst(limiter, "user1", 3600.0, 10);
        cout << "    t=3600 pe usne 10 requests bheji -> " << afterIdle << " allow hui\n";
        cout << "\n    ⭐ Balti poori bhar chuki thi (capacity 5 pe cap ho gayi —\n";
        cout << "       `min(capacity, ...)` ki wajah se 3600 tokens nahi bane).\n";
        cout << "       Aur ye sab bina kisi background timer ke hua — ek\n";
        cout << "       multiplication me 1 ghante ka refill ho gaya. ⭐\n";
    }

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Burst aur rate ALAG-ALAG control (capacity vs refill_rate).\n";
    cout << "  ✅ O(1) memory — bas tokens + last timestamp.\n";
    cout << "  ✅ Lazy refill — koi background timer/cron nahi chahiye.\n";
    cout << "  ✅ Idle user ka quota apne aap bhar jaata hai (par cap ke andar).\n";
    cout << "  ✅ Smooth — window algorithms ki tarah jhatke me quota nahi khulta.\n";
    cout << "  ❌ Do parameter tune karne padte hain.\n";
    cout << "  ❌ Burst allow karta hai — agar aapko BILKUL steady rate chahiye\n";
    cout << "     to ye galat choice hai.\n";
    cout << "  📌 Public APIs ka favourite: Stripe, GitHub, AWS — sab yahi.\n";
    cout << "\n  Bilkul steady outflow chahiye? -> LEAKING BUCKET (file 05).\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}

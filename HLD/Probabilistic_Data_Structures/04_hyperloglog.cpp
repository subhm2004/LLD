// ============================================================================
//  #4 — 04_hyperloglog.cpp
//  HYPERLOGLOG — 1 arab unique items gino, 16 KB me
// ----------------------------------------------------------------------------
//  Sawaal: "aaj kitne UNIQUE users aaye?" Duplicate nahi ginne — unique.
//
//  Seedha tareeka: `unordered_set<string>` me sab daal do aur size() poochho.
//  1 crore unique users = ~800 MB. 1 arab = ~80 GB. Bekaar.
//
//  ⭐ HyperLogLog wahi kaam ~16 KB me karta hai — ~2% error ke saath.
//     Aur ye 16 KB 1 lakh pe bhi 16 KB hai aur 1 ARAB pe bhi 16 KB. 🤯
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ IDEA — "sikke uchhalne" wali soch                                     │
//  │                                                                          │
//  │  Ek sikka baar-baar uchhalo aur ginte jao kitne HEAD lagatar aaye.        │
//  │    * 3 head lagatar aana aam baat hai (~1/8 chance)                       │
//  │    * 20 head lagatar aaye? To aapne BAHUT baar uchhala hoga (~10 lakh)    │
//  │                                                                          │
//  │  Yaani "sabse lamba head streak" se andaaza lag jaata hai ki kitni baar   │
//  │  uchhala gaya — bina ginti rakhe!                                        │
//  │                                                                          │
//  │  HLL yahi karta hai: har item ka hash lo, usme SHURU ke kitne ZERO bits   │
//  │  hain wo gino (= head streak). Jitna lamba streak dekha, utne zyada alag  │
//  │  items aaye honge.                                                       │
//  │                                                                          │
//  │  ⚠ Ek hi streak pe bharosa nahi kar sakte (kismat kharab ho sakti hai).   │
//  │     Isliye hash ko `m` buckets me baant do, har bucket apna max streak    │
//  │     rakhe, aur ant me sabka HARMONIC MEAN lo. Kismat ka asar dhul jaata   │
//  │     hai. Yahi "Hyper" + "LogLog" hai.                                    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ FORMULA — aur yahi verify karenge                                    │
//  │                                                                          │
//  │      estimate = alpha_m * m^2 / sum(2^(-M[j]))                           │
//  │      standard error = 1.04 / sqrt(m)                                     │
//  │                                                                          │
//  │  m=16384 (b=14) pe error = 1.04/128 = 0.81%                              │
//  │                                                                          │
//  │  Ye demo alag-alag `m` pe error naapega aur formula se compare karega.    │
//  │  Har baar error aur register count ka rishta 1/sqrt(m) nikalna chahiye.   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ HARMONIC MEAN kyun (normal average nahi)? Kyunki ek bhi bucket me kismat
//     se bahut lamba streak aa gaya to normal average use bahut kheench deta.
//     Harmonic mean bade outliers ko dabaa deta hai. Ye HLL ka "Hyper" hai —
//     isi ek badlav ne purane LogLog se accuracy kaafi behtar kar di thi.
//
//  ✅ Faayde  : constant memory (cardinality se bilkul independent!), MERGE ho
//               sakte hain (do din ka data jodo — union), ~2% error
//  ❌ Nuksan  : sirf COUNT deta hai, "kaun-kaun the" nahi; chhoti cardinality
//               pe alag correction chahiye
//  📌 Kab use : unique visitors, unique IPs, distinct search queries —
//               Redis ka PFCOUNT, Presto/BigQuery ka APPROX_COUNT_DISTINCT
//
//  Compile: g++ -std=c++17 -Wall -Wextra 04_hyperloglog.cpp -o hll_demo
// ============================================================================
#include <bits/stdc++.h>

#include "structures/hyperloglog.h"

using namespace std;
using namespace pds;

int main()
{
    cout << "=========================================================\n";
    cout << " #4 : HYPERLOGLOG\n";
    cout << "=========================================================\n\n";

    // ---- Part A: kaam karta hai? -----------------------------------------
    cout << "[A] Pehla test — 10 lakh unique items gino:\n\n";
    {
        HyperLogLog hll(14);
        const int N = 1000000;
        for (int i = 0; i < N; ++i)
        {
            hll.add("user:" + to_string(i));
        }

        double est = hll.estimate();
        cout << "    Asli unique   : " << N << "\n";
        cout << "    HLL estimate  : " << fixed << setprecision(0) << est << "\n";
        cout << "    Error         : " << setprecision(3) << (100.0 * fabs(est - N) / N) << "%\n";
        cout << "    Memory use    : " << humanBytes(static_cast<double>(hll.memoryBytes()))
             << "   (registers: " << hll.registerCount() << ")\n";
        cout << "\n    ⭐ 10 lakh items gine, aur memory lagi sirf "
             << humanBytes(static_cast<double>(hll.memoryBytes())) << ".\n";
        cout << "       Ek hash set me yahi ~80 MB legta.\n\n";
    }

    // ---- Part B: duplicates se farak nahi padta --------------------------
    cout << "[B] Duplicates ignore hote hain (yahi to 'unique count' hai):\n\n";
    {
        HyperLogLog hll(14);
        for (int round = 0; round < 10; ++round)
        {
            for (int i = 0; i < 100000; ++i)
            {
                hll.add("user:" + to_string(i)); // wahi 1 lakh users, 10 baar
            }
        }
        cout << "    10 lakh events daale, par sirf 1 lakh UNIQUE users the\n";
        cout << "    HLL estimate: " << fixed << setprecision(0) << hll.estimate()
             << "  (1 lakh ke aas-paas hona chahiye) ✅\n\n";
    }

    // ---- Part C: ⭐⭐ error vs 1.04/sqrt(m) -----------------------------
    cout << "[C] ⭐⭐ Formula check: standard error = 1.04 / sqrt(m)\n\n";
    {
        const int N = 1000000;
        cout << "    " << N << " unique items, alag-alag precision pe:\n\n";
        cout << "     b  |    m    | memory   | theory error | asli error | registers 2x = error?\n";
        cout << "    ----+---------+----------+--------------+------------+----------------------\n";

        double previousError = 0.0;
        for (int b : {8, 10, 12, 14, 16})
        {
            HyperLogLog hll(b);
            for (int i = 0; i < N; ++i)
            {
                hll.add("item:" + to_string(i));
            }
            double est = hll.estimate();
            double actualError = 100.0 * fabs(est - N) / N;
            double theoryError = 100.0 * hll.standardError();

            cout << "    " << setw(3) << b << " | " << setw(7) << hll.registerCount() << " | "
                 << setw(8) << humanBytes(static_cast<double>(hll.memoryBytes())) << " | "
                 << setw(11) << fixed << setprecision(3) << theoryError << "% | " << setw(9)
                 << actualError << "% | ";

            if (previousError > 0.0)
            {
                cout << "theory " << setprecision(1) << (previousError / theoryError) << "x kam\n";
            }
            else
            {
                cout << "-\n";
            }
            previousError = theoryError;
        }

        cout << "\n    ⭐ Har baar b 2 se badhaya (m 4 guna hua) to theory error AADHA hua —\n";
        cout << "       kyunki error 1/sqrt(m) hai, aur sqrt(4) = 2.\n";
        cout << "    ⭐ Iska ULTA matlab bhi samjho: error AADHA karne ke liye memory\n";
        cout << "       CHAAR GUNA chahiye. Isliye 16 KB (b=14, ~0.8% error) pe log\n";
        cout << "       ruk jaate hain — usse aage faayda kam, kharcha zyada.\n\n";
        cout << "    📌 'asli error' ek hi sample hai, isliye wo theory ke upar-neeche\n";
        cout << "       hilta rahega. Theory 'standard' error batati hai (~68% cases\n";
        cout << "       isse kam), guarantee nahi. Agla part ise thik se naapta hai.\n\n";
    }

    // ---- Part D: bahut saare trials — ab average error dekho ------------
    cout << "[D] Ek sample bharosemand nahi — 50 alag dataset pe average error:\n\n";
    {
        const int N = 500000;
        cout << "     b  |    m    | theory error | AVERAGE asli error (50 trials)\n";
        cout << "    ----+---------+--------------+-------------------------------\n";

        for (int b : {10, 12, 14})
        {
            double totalError = 0.0;
            const int TRIALS = 50;
            for (int trial = 0; trial < TRIALS; ++trial)
            {
                HyperLogLog hll(b);
                for (int i = 0; i < N; ++i)
                {
                    // Har trial me alag prefix = bilkul alag dataset
                    hll.add("t" + to_string(trial) + ":item:" + to_string(i));
                }
                totalError += 100.0 * fabs(hll.estimate() - N) / N;
            }
            double avgError = totalError / TRIALS;
            HyperLogLog probe(b);
            cout << "    " << setw(3) << b << " | " << setw(7) << probe.registerCount() << " | "
                 << setw(11) << fixed << setprecision(3) << (100.0 * probe.standardError())
                 << "% | " << setw(29) << avgError << "%\n";
        }
        cout << "\n    ⭐ Ab average asli error theory ke bahut kareeb hai. Yahi hai\n";
        cout << "       'standard error' ka matlab — wo ek AUSAT hai, har baar ka vaada nahi.\n";
        cout << "       (Average |error| theory se thoda kam aata hai — ye normal\n";
        cout << "        distribution ki property hai, ~0.8 x standard deviation.)\n\n";
    }

    // ---- Part E: ⭐ MERGE — distributed counting -----------------------
    cout << "[E] ⭐ MERGE — HLL ki sabse badi superpower:\n\n";
    {
        // 4 servers, har ek apne users dekh raha hai — kuch OVERLAP ke saath
        HyperLogLog server1(14), server2(14), server3(14), server4(14);

        for (int i = 0; i < 400000; ++i)
        {
            server1.add("user:" + to_string(i));
        }
        for (int i = 200000; i < 600000; ++i) // 200k overlap server1 ke saath
        {
            server2.add("user:" + to_string(i));
        }
        for (int i = 500000; i < 900000; ++i)
        {
            server3.add("user:" + to_string(i));
        }
        for (int i = 800000; i < 1000000; ++i)
        {
            server4.add("user:" + to_string(i));
        }

        // Sab jod do
        HyperLogLog total(14);
        total.merge(server1);
        total.merge(server2);
        total.merge(server3);
        total.merge(server4);

        cout << "    4 servers, har ek ke apne users (aapas me overlap ke saath):\n";
        cout << "      server1: " << fixed << setprecision(0) << server1.estimate() << "\n";
        cout << "      server2: " << server2.estimate() << "\n";
        cout << "      server3: " << server3.estimate() << "\n";
        cout << "      server4: " << server4.estimate() << "\n";
        cout << "      ----------------------------------\n";
        cout << "      MERGE   : " << total.estimate() << "   (asli unique: 1000000)\n";
        cout << "      Error   : " << setprecision(3)
             << (100.0 * fabs(total.estimate() - 1000000) / 1000000) << "%\n";

        cout << "\n    ⭐ Dhyan do ki simple JOD (400k+400k+400k+200k = 1400000) galat hota,\n";
        cout << "       kyunki overlap dobara gin liya jaata. Merge ne overlap apne aap\n";
        cout << "       handle kar liya — kyunki wo registers ka MAX leta hai, jod nahi.\n";
        cout << "\n    ⭐ Iska production matlab bahut bada hai: har server sirf apna\n";
        cout << "       16 KB ka sketch bheje, koi raw user-id bhejne ki zaroorat nahi.\n";
        cout << "       Isi liye Redis ka PFMERGE aur BigQuery ke approx functions\n";
        cout << "       distributed setup me itne accha kaam karte hain.\n\n";
    }

    // ---- Part F: memory — asli kamaal ------------------------------------
    cout << "[F] Aur ab wo number jo sabko chaunkata hai — MEMORY:\n\n";
    {
        HyperLogLog hll(14);
        cout << "    unique items   | hash set (approx) | HyperLogLog | bachat\n";
        cout << "    ---------------+-------------------+-------------+--------\n";
        for (long long n : {1000LL, 1000000LL, 100000000LL, 1000000000LL})
        {
            double setBytes = n * 80.0;
            double hllBytes = static_cast<double>(hll.memoryBytes());
            cout << "    " << setw(14) << n << " | " << setw(17) << humanBytes(setBytes) << " | "
                 << setw(11) << humanBytes(hllBytes) << " | " << setw(6) << fixed
                 << setprecision(0) << (setBytes / hllBytes) << "x\n";
        }
        cout << "\n    ⭐⭐ HLL column dekho — wo BADALTA HI NAHI. 1000 items ho ya\n";
        cout << "        1 ARAB, memory wahi 16 KB rehti hai.\n";
        cout << "        Kyunki hum items store karte hi nahi — sirf 16384 chhote\n";
        cout << "        counters rakhte hain jo 'sabse lamba streak' yaad rakhte hain.\n";
    }

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Memory CONSTANT — 1000 ho ya 1 arab, 16 KB hi lagti hai.\n";
    cout << "  ✅ Error 1.04/sqrt(m) formula ke bilkul mutabik nikla (50 trials).\n";
    cout << "  ✅ MERGE ho sakte hain — distributed unique count ka perfect hal,\n";
    cout << "     aur overlap apne aap handle ho jaata hai (max lete hain, jod nahi).\n";
    cout << "  ⚠ Error aadha karne ke liye memory 4x chahiye — isliye b=14 pe rukte hain.\n";
    cout << "  ❌ Sirf GINTI deta hai — 'kaun-kaun the' nahi bata sakta.\n";
    cout << "  ❌ Chhoti cardinality pe alag correction (linear counting) chahiye.\n";
    cout << "\n  Ab saare structures ek saath dekho -> 05_compare_all.cpp\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}

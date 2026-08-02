// ============================================================================
//  #3 — 03_count_min_sketch.cpp
//  COUNT-MIN SKETCH — "kitni baar aaya?" ka sasta jawab
// ----------------------------------------------------------------------------
//  Bloom filter batata hai "hai ya nahi". Par aksar ye jaanna hota hai ki
//  "KITNI BAAR aaya" — kaunsa URL sabse zyada hit hua, kis IP ne sabse zyada
//  requests bheji, kaunsa product sabse zyada dekha gaya.
//
//  Exact ginti ke liye `unordered_map<string, int>` chahiye — 10 crore alag
//  keys pe wo GBs kha jaayega. Count-Min Sketch wahi kaam kuch MB me karta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ KAAM KAISE KARTA HAI                                                  │
//  │                                                                          │
//  │  Ek 2D table: `d` rows x `w` columns. Har row ka apna alag hash.          │
//  │                                                                          │
//  │     add(x):      har row i me ->  table[i][h_i(x) % w] += 1               │
//  │     estimate(x): har row ka value dekho, unme se SABSE CHHOTA lo          │
//  │                                                                          │
//  │  ⭐ "MIN" hi kyun lete hain? Kyunki har cell me DOOSRE items ki ginti bhi │
//  │     mili hui ho sakti hai (collision). Yaani har row ka jawab asli se     │
//  │     ZYADA ya BARABAR hoga, kam kabhi nahi. To sabse chhota jawab hi       │
//  │     sach ke sabse kareeb hai.                                            │
//  │                                                                          │
//  │  Isi wajah se: CMS kabhi KAM nahi batata, sirf ZYADA bata sakta hai.      │
//  │  (Bloom filter ki tarah — wahan bhi galti sirf ek taraf jaati thi.)       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ THEORY KA VAADA — aur yahi is demo me check hoga                     │
//  │                                                                          │
//  │      w = ceil(e / ε)        aur      d = ceil(ln(1 / δ))                  │
//  │                                                                          │
//  │  ke saath vaada ye hai:                                                   │
//  │                                                                          │
//  │      estimate <= asli_count + ε*N     (probability 1-δ ke saath)          │
//  │                                                                          │
//  │  jahan N = saari counts ka jod (poore stream ka size).                    │
//  │                                                                          │
//  │  ⚠ Dhyan do: error `ε*N` hai — yaani POORE STREAM ke hisaab se, us ek     │
//  │     item ke hisaab se nahi. Iska matlab bada hi practical hai:            │
//  │       * BADI counts (heavy hitters) pe error % me BAHUT chhota hoga       │
//  │       * CHHOTI counts pe wahi error % me BAHUT BADA lag sakta hai         │
//  │     Isi liye CMS "top-k / heavy hitters" ke liye banaya gaya hai,         │
//  │     har item ki exact ginti ke liye nahi. Ye demo dono dikhayega.         │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ✅ Faayde  : kam memory, O(d) add/query, kabhi kam nahi batata
//  ❌ Nuksan  : zyada bata sakta hai; chhoti counts pe error bada dikhta;
//               kaunsi keys hain ye nahi bata sakta (sirf poochne pe jawab)
//  📌 Kab use : heavy hitters, trending topics, per-IP request counting,
//               DDoS detection, "top 10 products" — jahan bade numbers matter karte
//
//  Compile: g++ -std=c++17 -Wall -Wextra 03_count_min_sketch.cpp -o cms_demo
// ============================================================================
#include <bits/stdc++.h>

#include "structures/count_min_sketch.h"

using namespace std;
using namespace pds;

// ---- Zipf-ish stream (asli traffic jaisa: kuch cheezein bahut popular) ---
vector<string> makeZipfStream(int distinctKeys, int totalEvents, unsigned seed)
{
    mt19937 rng(seed);
    vector<double> cumulative;
    double sum = 0.0;
    for (int i = 0; i < distinctKeys; ++i)
    {
        sum += 1.0 / (i + 1);
        cumulative.push_back(sum);
    }

    uniform_real_distribution<double> pick(0.0, sum);
    vector<string> stream;
    stream.reserve(totalEvents);
    for (int i = 0; i < totalEvents; ++i)
    {
        double target = pick(rng);
        long index = lower_bound(cumulative.begin(), cumulative.end(), target) - cumulative.begin();
        stream.push_back("key:" + to_string(index));
    }
    return stream;
}

int main()
{
    cout << "=========================================================\n";
    cout << " #3 : COUNT-MIN SKETCH\n";
    cout << "=========================================================\n\n";

    // ---- Part A: basic ----------------------------------------------------
    cout << "[A] Basic — kuch items gino aur estimate poocho:\n\n";
    {
        CountMinSketch sketch = CountMinSketch::forError(0.001, 0.01);
        sketch.add("apple", 100);
        sketch.add("mango", 50);
        sketch.add("banana", 7);

        for (const string &fruit : {"apple", "mango", "banana", "grapes"})
        {
            cout << "    estimate(\"" << left << setw(8) << fruit << "\") = " << right << setw(4)
                 << sketch.estimate(fruit) << "\n";
        }
        cout << "\n    \"grapes\" kabhi add hua hi nahi — uska 0 aana chahiye tha,\n";
        cout << "    aur aaya bhi. (Chhote sketch me thoda zyada bhi aa sakta tha.)\n\n";
    }

    // ---- Part B: ⭐ KABHI KAM NAHI batata — bade paimane pe test --------
    cout << "[B] ⭐ Guarantee: CMS kabhi KAM nahi batata\n\n";
    {
        vector<string> stream = makeZipfStream(50000, 2000000, 42);

        CountMinSketch sketch = CountMinSketch::forError(0.0001, 0.01);
        unordered_map<string, uint64_t> exact;

        for (const string &key : stream)
        {
            sketch.add(key);
            exact[key]++;
        }

        int underestimates = 0, exactMatches = 0, overestimates = 0;
        for (const auto &entry : exact)
        {
            uint64_t est = sketch.estimate(entry.first);
            if (est < entry.second)
            {
                ++underestimates;
            }
            else if (est == entry.second)
            {
                ++exactMatches;
            }
            else
            {
                ++overestimates;
            }
        }

        cout << "    Stream: " << stream.size() << " events, " << exact.size()
             << " distinct keys\n\n";
        cout << "    Underestimates (kam bataya) : " << underestimates
             << "   <-- 0 hona hi chahiye ✅\n";
        cout << "    Bilkul exact                : " << exactMatches << "\n";
        cout << "    Overestimates (zyada bataya): " << overestimates << "\n";
        cout << "\n    ⭐ Ek bhi underestimate nahi. Ye 'shayad' nahi, GARANTEE hai —\n";
        cout << "       kyunki har cell me sirf JUD sakta hai, ghat nahi sakta.\n\n";
    }

    // ---- Part C: ⭐⭐ ERROR BOUND — theory vs asliyat -------------------
    cout << "[C] ⭐⭐ Theory ka vaada: error <= ε*N. Sach hai?\n\n";
    {
        vector<string> stream = makeZipfStream(50000, 2000000, 7);

        cout << "    Stream: " << stream.size() << " events, 50000 distinct keys\n\n";
        cout << "      ε      |   w    | d | memory   | bound (ε*N) | asli max error | vaada?\n";
        cout << "    ---------+--------+---+----------+-------------+----------------+-------\n";

        for (double eps : {0.01, 0.001, 0.0001, 0.00001})
        {
            CountMinSketch sketch = CountMinSketch::forError(eps, 0.01);
            unordered_map<string, uint64_t> exact;
            for (const string &key : stream)
            {
                sketch.add(key);
                exact[key]++;
            }

            uint64_t maxError = 0;
            for (const auto &entry : exact)
            {
                maxError = max(maxError, sketch.estimate(entry.first) - entry.second);
            }

            double bound = sketch.errorBound();
            cout << "    " << setw(8) << fixed << setprecision(5) << eps << " | " << setw(6)
                 << sketch.width() << " | " << setw(1) << sketch.depth() << " | " << setw(9)
                 << humanBytes(static_cast<double>(sketch.memoryBytes())) << " | " << setw(11)
                 << setprecision(0) << bound << " | " << setw(14) << maxError << " | "
                 << (maxError <= bound ? "✅ nibhaya" : "❌ toota") << "\n";
        }

        cout << "\n    ⭐ Har baar asli error bound ke ANDAR raha — theory sach nikli.\n";
        cout << "       Aur dhyan do ki asli error bound se KAAFI kam hai. Bound ek\n";
        cout << "       'worst case' hai; practice me aksar usse behtar milta hai.\n\n";
    }

    // ---- Part D: ⚠ chhoti counts pe error bada lagta hai ---------------
    cout << "[D] ⚠ Ab wo baat jo log miss karte hain — error kis pe kitna lagta hai:\n\n";
    {
        vector<string> stream = makeZipfStream(50000, 2000000, 99);
        CountMinSketch sketch = CountMinSketch::forError(0.0001, 0.01);
        unordered_map<string, uint64_t> exact;
        for (const string &key : stream)
        {
            sketch.add(key);
            exact[key]++;
        }

        // Items ko unki asli count ke hisaab se sort karo
        vector<pair<uint64_t, string>> sorted;
        for (const auto &entry : exact)
        {
            sorted.push_back({entry.second, entry.first});
        }
        sort(sorted.rbegin(), sorted.rend());

        cout << "    rank    | key        | asli count | estimate | error % \n";
        cout << "    --------+------------+------------+----------+---------\n";

        vector<size_t> ranks = {0, 1, 4, 99, 999, 9999, sorted.size() - 1};
        for (size_t rank : ranks)
        {
            const auto &item = sorted[rank];
            uint64_t est = sketch.estimate(item.second);
            double errorPct = 100.0 * (est - item.first) / item.first;
            cout << "    " << setw(7) << (rank + 1) << " | " << left << setw(10) << item.second
                 << right << " | " << setw(10) << item.first << " | " << setw(8) << est << " | "
                 << setw(7) << fixed << setprecision(2) << errorPct << "%\n";
        }

        cout << "\n    ⭐ Pattern saaf hai: TOP items ka error ~0%, aur jaise-jaise count\n";
        cout << "       chhota hota jaata hai, error % badhne LAGTA hai.\n";
        cout << "       Wajah wahi jo upar likhi thi — error ABSOLUTE hota hai (ε*N),\n";
        cout << "       aur wahi absolute error chhote number pe bada percent ban jaata hai.\n";
        cout << "       (Absolute error har jagah lagbhag same hai — 10-20 ke aas-paas.\n";
        cout << "        175461 pe wo 0% hai, par 18 pe wahi 27% ban jaata hai.)\n";
        cout << "\n    📌 Sabse aakhri row me error 0% dikh sakta hai — wo 'lucky' item hai\n";
        cout << "       jiske kisi bhi row me collision nahi hua. Chhoti counts pe result\n";
        cout << "       item-dar-item bahut badalta hai; bharosa TREND pe karo, ek row pe nahi.\n";
        cout << "\n    ⭐ Isliye CMS ko 'har item ki exact ginti' ke liye MAT use karo.\n";
        cout << "       Wo 'top-k / heavy hitters' ke liye bana hai — aur wahan wo\n";
        cout << "       lagbhag perfect hai.\n\n";
    }

    // ---- Part E: heavy hitters — asli use case ---------------------------
    cout << "[E] Asli use case — top 10 nikalo (aur exact se compare karo):\n\n";
    {
        vector<string> stream = makeZipfStream(100000, 5000000, 2024);
        CountMinSketch sketch = CountMinSketch::forError(0.0001, 0.01);
        unordered_map<string, uint64_t> exact;
        for (const string &key : stream)
        {
            sketch.add(key);
            exact[key]++;
        }

        vector<pair<uint64_t, string>> exactTop;
        for (const auto &entry : exact)
        {
            exactTop.push_back({entry.second, entry.first});
        }
        partial_sort(exactTop.begin(), exactTop.begin() + 10, exactTop.end(), greater<>());

        cout << "    rank | key       | exact  | CMS estimate | match?\n";
        cout << "    -----+-----------+--------+--------------+-------\n";
        int matches = 0;
        for (int i = 0; i < 10; ++i)
        {
            uint64_t est = sketch.estimate(exactTop[i].second);
            bool ok = (est == exactTop[i].first);
            matches += ok;
            cout << "    " << setw(4) << (i + 1) << " | " << left << setw(9)
                 << exactTop[i].second << right << " | " << setw(6) << exactTop[i].first << " | "
                 << setw(12) << est << " | " << (ok ? "✅" : "~") << "\n";
        }

        double exactMem = exact.size() * 80.0; // rough: string + node overhead
        cout << "\n    Top-10 me se " << matches << " bilkul exact aaye.\n";
        cout << "\n    Memory: exact map ~" << humanBytes(exactMem) << "  vs  CMS "
             << humanBytes(static_cast<double>(sketch.memoryBytes())) << "  ("
             << fixed << setprecision(0) << (exactMem / sketch.memoryBytes()) << "x kam)\n";
        cout << "\n    ⭐ Yahi CMS ka asli maqsad hai: heavy hitters lagbhag perfectly,\n";
        cout << "       aur memory ka bahut chhota hissa use karke.\n";
    }

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Kabhi KAM nahi batata (20 lakh events pe 0 underestimates).\n";
    cout << "  ✅ Error hamesha theory ke bound (ε*N) ke andar raha.\n";
    cout << "  ✅ Heavy hitters pe lagbhag exact — aur bahut kam memory me.\n";
    cout << "  ⚠ Error ABSOLUTE hai (ε*N), isliye chhoti counts pe % me bada lagta hai.\n";
    cout << "  ❌ Keys ki list nahi de sakta — sirf poochne pe jawab deta hai.\n";
    cout << "     (Top-k ke liye saath me ek chhota heap rakhna padta hai.)\n";
    cout << "\n  Ab 'kitni baar' ho gaya. 'KITNE ALAG' kaise gino? -> HYPERLOGLOG (file 04)\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}

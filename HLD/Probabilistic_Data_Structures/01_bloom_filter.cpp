// ============================================================================
//  #1 — 01_bloom_filter.cpp
//  BLOOM FILTER — "shayad hai" ya "pakka nahi hai"
// ----------------------------------------------------------------------------
//  Sawaal: 10 crore URLs me se poochna hai "ye URL pehle dekha tha kya?"
//  `unordered_set<string>` me rakhoge to GBs chahiye. Bloom filter wahi kaam
//  ~100 MB me kar deta hai — ek chhoti si shart pe.
//
//  ⭐ WO SHART: Bloom filter do me se ek jawab deta hai —
//        "PAKKA NAHI HAI"   (100% bharosemand)
//        "SHAYAD HAI"       (thoda sa jhooth bol sakta hai)
//
//  Yaani FALSE POSITIVE ho sakta hai, FALSE NEGATIVE kabhi nahi.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ KAAM KAISE KARTA HAI                                                  │
//  │                                                                          │
//  │  Ek `m` bits ki khaali array. Har item ke liye `k` hash nikalo, aur un    │
//  │  k jagah pe bit 1 kar do.                                                │
//  │                                                                          │
//  │     add("apple")   -> bits 3, 17, 42 ko 1 kar diya                        │
//  │     check("apple") -> teeno 1 hain? => "shayad hai"                       │
//  │     check("mango") -> koi ek bhi 0 mila? => "PAKKA NAHI HAI"              │
//  │                                                                          │
//  │  False negative kyun IMPOSSIBLE hai: agar item add hua tha to uske saare  │
//  │  k bits 1 kiye gaye the, aur bit kabhi 0 hoti hi nahi. To wo bits aaj bhi │
//  │  1 hain. Isliye "nahi hai" wala jawab galat ho hi nahi sakta. ⭐          │
//  │                                                                          │
//  │  False positive kyun HOTA hai: "mango" ke teeno bits kisi AUR items ne    │
//  │  milkar 1 kar diye ho sakte hain. Tab filter dhokha kha jaata hai.        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ FORMULA — aur yahi is demo ka asli maqsad hai                        │
//  │                                                                          │
//  │  False positive rate:                                                     │
//  │         p = (1 - e^(-k*n/m))^k                                            │
//  │                                                                          │
//  │  Diye gaye n aur p ke liye best size aur hash count:                       │
//  │         m = -(n * ln p) / (ln 2)^2                                        │
//  │         k = (m/n) * ln 2                                                  │
//  │                                                                          │
//  │  Ye demo formula pe bharosa nahi karega — wo formula ka jawab nikalega,   │
//  │  phir lakhon lookups chala kar ASLI rate naapega, aur dono compare karega.│
//  │  Agar dono match karte hain, to theory bhi sahi hai aur hash bhi. ⭐      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ✅ Faayde  : bahut kam memory, O(k) add/check, false negative kabhi nahi
//  ❌ Nuksan  : false positive, DELETE nahi kar sakte, gine nahi sakte,
//               aur jo daala wo wapas padh nahi sakte (items store hote hi nahi)
//  📌 Kab use : "pehle se hai kya?" wale check jahan thoda jhooth chalta ho —
//               DB pe jaane se pehle filter (Cassandra/HBase), URL dedup,
//               cache me hai ya nahi, chori hue passwords ki list
//
//  Compile: g++ -std=c++17 -Wall -Wextra 01_bloom_filter.cpp -o bloom_demo
// ============================================================================
#include <bits/stdc++.h>

#include "structures/bloom_filter.h"

using namespace std;
using namespace pds;

int main()
{
    cout << "=========================================================\n";
    cout << " #1 : BLOOM FILTER\n";
    cout << "=========================================================\n\n";

    // ---- Part A: basic behaviour ------------------------------------------
    cout << "[A] Sabse pehle basic — kya kehta hai ye:\n\n";
    {
        BloomFilter filter = BloomFilter::forCapacity(1000, 0.01);
        for (const string &fruit : {"apple", "mango", "banana"})
        {
            filter.add(fruit);
        }

        for (const string &query : {"apple", "mango", "banana", "grapes", "orange"})
        {
            bool result = filter.mightContain(query);
            cout << "    check(\"" << left << setw(8) << query << "\") -> "
                 << (result ? "SHAYAD HAI  (add kiya tha? "
                            : "PAKKA NAHI HAI (add kiya tha? ")
                 << ((query == "apple" || query == "mango" || query == "banana") ? "haan)" : "nahi)")
                 << "\n"
                 << right;
        }
        cout << "\n    Filter ne teeno added items pe 'shayad hai' kaha. ✅\n\n";
    }

    // ---- Part B: ⭐ FALSE NEGATIVE kabhi nahi — 10 lakh pe test ----------
    cout << "[B] ⭐ Sabse zaroori guarantee: FALSE NEGATIVE KABHI NAHI\n\n";
    {
        const int N = 1000000;
        BloomFilter filter = BloomFilter::forCapacity(N, 0.01);

        vector<string> keys = makeKeys("user:", N);
        for (const string &key : keys)
        {
            filter.add(key);
        }

        int falseNegatives = 0;
        for (const string &key : keys)
        {
            if (!filter.mightContain(key))
            {
                ++falseNegatives; // ye kabhi nahi hona chahiye
            }
        }
        cout << "    " << N << " items add kiye, phir sabko wapas check kiya\n";
        cout << "    False negatives: " << falseNegatives << "   <-- 0 hona hi chahiye ✅\n";
        cout << "\n    ⭐ Ye 'shayad' nahi, GARANTEE hai. Bit kabhi 0 nahi hoti, isliye\n";
        cout << "       jo add hua uske bits hamesha 1 rahenge. Isi guarantee pe\n";
        cout << "       Cassandra jaise DB bharosa karte hain.\n\n";
    }

    // ---- Part C: ⭐⭐ FORMULA vs ASLIYAT --------------------------------
    cout << "[C] ⭐⭐ Ab asli test — FORMULA kehta kya hai, aur hota kya hai:\n\n";
    {
        const int N = 100000;       // itne items add karenge
        const int PROBES = 1000000; // itne NAYE items se test karenge

        cout << "    " << N << " items add kiye, phir " << PROBES
             << " aise items check kiye jo kabhi add hi nahi hue.\n";
        cout << "    Jitni baar 'shayad hai' aaya = false positive.\n\n";

        cout << "    target p |  m (bits) |  k | fill% | theory FP | measured FP | farak\n";
        cout << "    ---------+-----------+----+-------+-----------+-------------+-------\n";

        for (double targetP : {0.10, 0.05, 0.01, 0.001})
        {
            BloomFilter filter = BloomFilter::forCapacity(N, targetP);

            vector<string> added = makeKeys("in:", N);
            for (const string &key : added)
            {
                filter.add(key);
            }

            // Ab bilkul alag prefix ke items — inme se koi bhi add nahi hua
            int falsePositives = 0;
            for (int i = 0; i < PROBES; ++i)
            {
                if (filter.mightContain("out:" + to_string(i)))
                {
                    ++falsePositives;
                }
            }

            double measured = 100.0 * falsePositives / PROBES;
            double theory = 100.0 * filter.theoreticalFpRate(N);
            double gap = theory > 0 ? 100.0 * fabs(measured - theory) / theory : 0.0;

            cout << "      " << setw(6) << fixed << setprecision(3) << targetP << " | " << setw(9)
                 << filter.bitCount() << " | " << setw(2) << filter.hashCount() << " | " << setw(5)
                 << setprecision(1) << (100.0 * filter.fillRatio()) << " | " << setw(8)
                 << setprecision(4) << theory << "% | " << setw(10) << measured << "% | " << setw(5)
                 << setprecision(1) << gap << "%\n";
        }

        cout << "\n    ⭐ Theory aur asliyat lagbhag bilkul match kar rahe hain.\n";
        cout << "       Iska matlab DO baatein sabit hui:\n";
        cout << "         1. Formula (1 - e^(-kn/m))^k sach me kaam karta hai\n";
        cout << "         2. Humara hash function theek se bikhra hua hai\n";
        cout << "            (kharab hash hota to measured theory se kaafi zyada aata)\n\n";
        cout << "    📌 Aur 'fill%' column dekho — wo hamesha ~50% ke aas-paas hai.\n";
        cout << "       Ye ittefaq nahi: optimal k wahi hota hai jo aadhi bits 1 kare.\n";
        cout << "       Isse zyada bharo to collisions badhte, kam bharo to memory waste.\n\n";
    }

    // ---- Part D: k badal ke dekho — optimal sach me optimal hai? ---------
    cout << "[D] Optimal k sach me optimal hai? (m fix, k badalte hain):\n\n";
    {
        const int N = 100000;
        const size_t M = 958506; // ~1% ke liye jitni bits banti hain
        const int PROBES = 500000;

        int optimalK = static_cast<int>(round((static_cast<double>(M) / N) * M_LN2));
        cout << "    n=" << N << ", m=" << M << " bits => formula ka optimal k = " << optimalK
             << "\n\n";
        cout << "     k  | theory FP | measured FP\n";
        cout << "    ----+-----------+------------\n";

        for (int k : {2, 4, 6, 7, 9, 12, 16})
        {
            BloomFilter filter(M, k);
            for (int i = 0; i < N; ++i)
            {
                filter.add("in:" + to_string(i));
            }
            int fp = 0;
            for (int i = 0; i < PROBES; ++i)
            {
                if (filter.mightContain("out:" + to_string(i)))
                {
                    ++fp;
                }
            }
            cout << "    " << setw(3) << k << " | " << setw(8) << fixed << setprecision(4)
                 << (100.0 * filter.theoreticalFpRate(N)) << "% | " << setw(9)
                 << (100.0 * fp / PROBES) << "%"
                 << (k == optimalK ? "   <-- optimal ⭐" : "") << "\n";
        }
        cout << "\n    ⭐ 'theory' column me saaf dikh raha hai ki k=" << optimalK
             << " pe FP sabse kam hai.\n";
        cout << "       Dono taraf ja kar wo badhta hai:\n";
        cout << "         k kam  -> har item ke kam nishaan, to takraana aasan\n";
        cout << "         k zyada-> array jaldi bhar jaata (fill% badh jaata), phir\n";
        cout << "                   har lookup ke saare bits waise hi 1 mil jaate\n";
        cout << "       Beech me sweet spot — wahi (m/n) ln 2 hai.\n\n";
        cout << "    📌 'measured' column me k=6 aur k=7 lagbhag barabar dikhenge —\n";
        cout << "       curve wahan itna flat hai ki " << PROBES << " probes ka sampling\n";
        cout << "       shor unhe alag nahi kar paata. Isi liye yahan theory ko dekho.\n\n";
    }

    // ---- Part E: memory ka faayda ----------------------------------------
    cout << "[E] Aur ye sab kis liye — MEMORY:\n\n";
    {
        cout << "    items      | unordered_set (approx) | Bloom (1% FP) | bachat\n";
        cout << "    -----------+------------------------+---------------+--------\n";
        for (long long n : {100000LL, 1000000LL, 10000000LL, 100000000LL})
        {
            // set: string (~32B avg URL) + node overhead (~48B) — mota andaaza
            double setBytes = n * 80.0;
            BloomFilter filter = BloomFilter::forCapacity(static_cast<size_t>(n), 0.01);
            double bloomBytes = static_cast<double>(filter.memoryBytes());

            cout << "    " << setw(10) << n << " | " << setw(22) << humanBytes(setBytes) << " | "
                 << setw(13) << humanBytes(bloomBytes) << " | " << setw(5) << fixed
                 << setprecision(0) << (setBytes / bloomBytes) << "x\n";
        }
        cout << "\n    ⭐ ~65x kam memory. Aur dhyan do: Bloom filter items ko STORE\n";
        cout << "       nahi karta — sirf unke 'nishaan' (bits) rakhta hai. Isliye\n";
        cout << "       aap usme se items wapas padh NAHI sakte. Ye feature hai,\n";
        cout << "       bug nahi — isi wajah se itni memory bachi.\n";
    }

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ False negative KABHI nahi (10 lakh pe test kiya — 0).\n";
    cout << "  ✅ FP rate formula se bilkul match karta hai (upar table dekho).\n";
    cout << "  ✅ ~65x kam memory hash set se.\n";
    cout << "  ✅ Optimal k = (m/n) ln 2 sach me optimal nikla.\n";
    cout << "  ❌ False positive hota hai (par aap uska rate CHUN sakte ho).\n";
    cout << "  ❌ DELETE nahi kar sakte — ek bit kai items ki ho sakti hai.\n";
    cout << "  ❌ Items wapas nahi nikal sakte, gin bhi nahi sakte.\n";
    cout << "\n  Delete karna hai? -> COUNTING BLOOM FILTER (file 02)\n";
    cout << "  Ginti chahiye?     -> COUNT-MIN SKETCH (file 03)\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}

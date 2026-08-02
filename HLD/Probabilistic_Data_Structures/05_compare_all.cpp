// ============================================================================
//  #5 — 05_compare_all.cpp
//  SAARE STRUCTURES EK SAATH — kaun kis sawaal ka jawab deta hai
// ----------------------------------------------------------------------------
//  In chaaron ko log aksar gadd-madd kar dete hain, kyunki teeno "hash + kam
//  memory + thoda approximate" hain. Par ye BILKUL alag sawaalon ke jawab dete
//  hain — aur ek ka kaam doosra kar hi nahi sakta.
//
//     Bloom Filter    -> "ye item HAI kya?"          (haan/nahi)
//     Counting Bloom  -> wahi, par DELETE ke saath
//     Count-Min       -> "ye item KITNI BAAR aaya?"  (ginti)
//     HyperLogLog     -> "KITNE ALAG items the?"     (unique count)
//
//  ⭐ Ek line me farak yaad rakhne ka tareeka:
//        Bloom = membership, CMS = frequency, HLL = cardinality
//
//  Compile: g++ -std=c++17 -Wall -Wextra 05_compare_all.cpp -o compare_demo
// ============================================================================
#include <bits/stdc++.h>

#include "structures/bloom_filter.h"
#include "structures/count_min_sketch.h"
#include "structures/counting_bloom_filter.h"
#include "structures/hyperloglog.h"

using namespace std;
using namespace pds;

int main()
{
    cout << "=========================================================\n";
    cout << " #5 : SAARE PROBABILISTIC STRUCTURES KA MUQABLA\n";
    cout << "=========================================================\n\n";

    // ---- Part A: ek hi stream, chaar alag sawaal -------------------------
    cout << "[A] Ek hi data pe chaaron ko chala kar dekho:\n\n";
    {
        // 2 lakh events, par sirf 50 hazaar unique items
        const int EVENTS = 200000;
        const int UNIQUE = 50000;

        BloomFilter bloom = BloomFilter::forCapacity(UNIQUE, 0.01);
        CountMinSketch sketch = CountMinSketch::forError(0.0001, 0.01);
        HyperLogLog hll(14);

        mt19937 rng(2024);
        uniform_int_distribution<int> pick(0, UNIQUE - 1);
        unordered_map<string, int> exactCounts;

        for (int i = 0; i < EVENTS; ++i)
        {
            string key = "page:" + to_string(pick(rng));
            bloom.add(key);
            sketch.add(key);
            hll.add(key);
            exactCounts[key]++;
        }

        cout << "    Stream: " << EVENTS << " events, " << exactCounts.size()
             << " unique pages\n\n";

        // Sabse popular page dhoondho
        string topPage;
        int topCount = 0;
        for (const auto &entry : exactCounts)
        {
            if (entry.second > topCount)
            {
                topCount = entry.second;
                topPage = entry.first;
            }
        }

        cout << "    Sawaal 1: \"" << topPage << "\" dekha gaya tha kya?\"\n";
        cout << "      Bloom Filter -> " << (bloom.mightContain(topPage) ? "SHAYAD HAI ✅" : "NAHI")
             << "\n";
        cout << "      (CMS/HLL is sawaal ka seedha jawab dene ke liye nahi bane)\n\n";

        cout << "    Sawaal 2: \"" << topPage << "\" kitni baar aaya?\"\n";
        cout << "      Count-Min    -> " << sketch.estimate(topPage) << "   (asli: " << topCount
             << ")\n";
        cout << "      (Bloom sirf haan/nahi jaanta, ginti nahi)\n\n";

        cout << "    Sawaal 3: \"Kitne ALAG pages dekhe gaye?\"\n";
        cout << "      HyperLogLog  -> " << fixed << setprecision(0) << hll.estimate()
             << "   (asli: " << exactCounts.size() << ")\n";
        cout << "      (Bloom aur CMS ye bata hi nahi sakte)\n\n";

        cout << "    ⭐ Teeno ne wahi data dekha, par teeno ka jawab alag sawaal ka hai.\n";
        cout << "       Isi liye asli systems me ye SAATH-SAATH chalte hain, ek doosre\n";
        cout << "       ki jagah nahi.\n\n";
    }

    // ---- Part B: memory ka muqabla ---------------------------------------
    cout << "[B] MEMORY — 1 crore items pe:\n\n";
    {
        const size_t N = 10000000;

        BloomFilter bloom = BloomFilter::forCapacity(N, 0.01);
        CountingBloomFilter counting = CountingBloomFilter::forCapacity(N, 0.01);
        CountMinSketch sketch = CountMinSketch::forError(0.0001, 0.01);
        HyperLogLog hll(14);

        double exactSet = N * 80.0;

        cout << "    structure              | memory     | vs exact | kya deta hai\n";
        cout << "    -----------------------+------------+----------+------------------------\n";
        cout << "    unordered_set (exact)  | " << setw(10) << humanBytes(exactSet)
             << " |      1x  | sab kuch (par mehnga)\n";
        cout << "    Bloom Filter (1% FP)   | " << setw(10)
             << humanBytes(static_cast<double>(bloom.memoryBytes())) << " | " << setw(6) << fixed
             << setprecision(0) << (exactSet / bloom.memoryBytes()) << "x  | hai ya nahi\n";
        cout << "    Counting Bloom (1% FP) | " << setw(10)
             << humanBytes(static_cast<double>(counting.memoryBytes())) << " | " << setw(6)
             << (exactSet / counting.memoryBytes()) << "x  | hai ya nahi + delete\n";
        cout << "    Count-Min Sketch       | " << setw(10)
             << humanBytes(static_cast<double>(sketch.memoryBytes())) << " | " << setw(6)
             << (exactSet / sketch.memoryBytes()) << "x  | kitni baar aaya\n";
        cout << "    HyperLogLog (b=14)     | " << setw(10)
             << humanBytes(static_cast<double>(hll.memoryBytes())) << " | " << setw(6)
             << (exactSet / hll.memoryBytes()) << "x  | kitne alag the\n";

        cout << "\n    ⭐ HyperLogLog ka number sabse chaunkane wala hai — aur wo isliye\n";
        cout << "       ki uski memory item count pe DEPEND HI NAHI karti. Baaki teeno\n";
        cout << "       ki memory `n` ke saath badhti hai, HLL ki nahi.\n\n";
    }

    // ---- Part C: ⭐ error kis taraf jaata hai — ye bahut zaroori hai ----
    cout << "[C] ⭐ Sabki galti EK HI TARAF jaati hai (ye design hai, ittefaq nahi):\n\n";
    cout << "    structure        | galti ka type        | ulta kabhi hota hai?\n";
    cout << "    -----------------+----------------------+---------------------\n";
    cout << "    Bloom Filter     | \"hai\" bol sakta jab  | \"nahi hai\" GALAT ho —\n";
    cout << "                     | nahi hai (FP)        | KABHI NAHI ✅\n";
    cout << "    Count-Min Sketch | ZYADA bata sakta hai | KAM — KABHI NAHI ✅\n";
    cout << "    HyperLogLog      | dono taraf (±2%)     | -\n";
    cout << "\n    ⭐ Ye 'ek-tarfa galti' hi in structures ko USEFUL banati hai.\n";
    cout << "       Misaal: Cassandra har read se pehle Bloom filter poochta hai —\n";
    cout << "       \"ye key is file me ho sakti hai?\" Agar jawab \"NAHI\" aaya to wo\n";
    cout << "       file ko BINA PADHE skip kar deta hai, kyunki wo \"nahi\" 100% pakka\n";
    cout << "       hai. Agar galti dono taraf hoti, to wo skip karna khatarnak hota\n";
    cout << "       aur poora faayda khatam ho jaata.\n\n";

    // ---- Part D: asli duniya me ye saath-saath chalte hain --------------
    cout << "[D] Asli analytics pipeline — teeno ek saath:\n\n";
    {
        const int EVENTS = 500000;

        BloomFilter seenBefore = BloomFilter::forCapacity(200000, 0.01);
        CountMinSketch frequency = CountMinSketch::forError(0.0001, 0.01);
        HyperLogLog uniqueVisitors(14);

        mt19937 rng(7);
        // Zipf-ish: kuch users bahut active
        vector<double> cumulative;
        double sum = 0.0;
        const int USERS = 100000;
        for (int i = 0; i < USERS; ++i)
        {
            sum += 1.0 / (i + 1);
            cumulative.push_back(sum);
        }
        uniform_real_distribution<double> pickUser(0.0, sum);

        int newUsers = 0;
        for (int i = 0; i < EVENTS; ++i)
        {
            double target = pickUser(rng);
            long index = lower_bound(cumulative.begin(), cumulative.end(), target) -
                         cumulative.begin();
            string user = "user:" + to_string(index);

            // 1. Pehli baar aaya hai kya? (Bloom)
            if (!seenBefore.mightContain(user))
            {
                ++newUsers;
                seenBefore.add(user);
            }
            // 2. Kitna active hai? (CMS)
            frequency.add(user);
            // 3. Kul kitne alag log? (HLL)
            uniqueVisitors.add(user);
        }

        cout << "    " << EVENTS << " page views process kiye. Ek hi pass me:\n\n";
        cout << "      Naye users (Bloom se)        : " << newUsers << "\n";
        cout << "      Kul unique users (HLL se)    : " << fixed << setprecision(0)
             << uniqueVisitors.estimate() << "\n";
        cout << "      Top user ka count (CMS se)   : " << frequency.estimate("user:0") << "\n";

        size_t totalMemory = seenBefore.memoryBytes() + frequency.memoryBytes() +
                             uniqueVisitors.memoryBytes();
        cout << "\n      Teeno ki kul memory          : "
             << humanBytes(static_cast<double>(totalMemory)) << "\n";
        cout << "      Exact karne me lagti          : ~"
             << humanBytes(100000 * 80.0) << " (aur wo bhi sirf ek metric ke liye)\n";

        cout << "\n    ⭐ Ek hi pass me teen alag sawaalon ke jawab, aur memory itni\n";
        cout << "       kam ki ye sab ek chhote server pe streaming me chal sakta hai.\n";
        cout << "       Yahi wajah hai ki analytics systems (Redis, Druid, ClickHouse)\n";
        cout << "       in structures pe itna bharosa karte hain.\n";
    }

    cout << "\n=========================================================\n";
    cout << " SUMMARY — kaunsa kab\n";
    cout << "=========================================================\n\n";
    cout << "    sawaal                          | structure           | memory\n";
    cout << "    --------------------------------+---------------------+-----------\n";
    cout << "    \"ye item hai kya?\"              | Bloom Filter        | O(n) bits\n";
    cout << "    \"...aur delete bhi karna hai\"   | Counting Bloom      | 4x Bloom\n";
    cout << "    \"kitni baar aaya?\"              | Count-Min Sketch    | O(w*d)\n";
    cout << "    \"kitne ALAG the?\"               | HyperLogLog         | CONSTANT ⭐\n";

    cout << "\n    ⭐ TEEN BAATEIN JO YAAD RAKHNI HAIN:\n\n";
    cout << "    1. Sabki galti EK taraf jaati hai — aur wahi unhe safe banata hai.\n";
    cout << "       Bloom ka \"nahi hai\", CMS ka \"itne se kam nahi\" — ye 100% pakke hain.\n\n";
    cout << "    2. Ye ek doosre ki JAGAH nahi lete. Membership, frequency, cardinality\n";
    cout << "       teen alag sawaal hain. Asli systems teeno saath chalate hain.\n\n";
    cout << "    3. Ye tabhi lagao jab EXACT jawab ki zaroorat na ho. Payment, billing,\n";
    cout << "       ya auth me ye BILKUL mat use karo — wahan 1% galti bhi bhaari padti hai.\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}

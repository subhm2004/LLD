// ============================================================================
//  STEP 5 — 05_bounded_loads.cpp
//  HOT KEYS ka dard, aur "Consistent Hashing with Bounded Loads" ka ilaaj
// ----------------------------------------------------------------------------
//  Step 3 me humne KEYS barabar baant di thi. Par ruko —
//
//        keys barabar bantna  ≠  TRAFFIC barabar bantna
//
//  Asli duniya me traffic hamesha tedha (skewed) hota hai:
//     * Virat Kohli ka profile vs aapka profile — dono ek-ek key hain, par hits
//       me zameen-aasmaan ka farak.
//     * Ek viral tweet, ek trending video, ek flash sale ka product.
//  Ye "HOT KEYS" hain. Ring ko inki koi khabar nahi — wo to bas hash dekhta hai.
//  Nateeja: jis node pe hot key gir gayi, wo node jal gaya; baaki khaali baithe.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ CONSISTENT HASHING WITH BOUNDED LOADS (Google, 2016)                  │
//  │                                                                          │
//  │  Har node pe ek CAPACITY ka dhakkan (cap) laga do:                        │
//  │                                                                          │
//  │        capacity = ceil( c × total_load / num_nodes )   , c > 1            │
//  │                                                                          │
//  │  Routing: pehle wahi purana clockwise wala node dekho. Agar wo apni       │
//  │  capacity tak bhar chuka hai -> aage badho, agle node pe daal do.         │
//  │  Yaani ring wahi, bas "full" node ko SKIP kar dete hain.                  │
//  │                                                                          │
//  │  `c` = kitni chhoot deni hai. c=1.25 => koi bhi node average se 25% se    │
//  │  zyada bhara nahi ho sakta. GUARANTEE hai, ummeed nahi.                   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ ISKI KEEMAT (ye samajhna zaroori hai — free lunch nahi hai):
//     1. Ab assignment STATEFUL hai — wo "abhi kitna load hai" pe depend karta
//        hai. Isliye do alag clients ka jawab alag ho sakta hai. Iska matlab:
//        ye ROUTING/load-balancing ke liye achha hai, permanent DATA PLACEMENT
//        ke liye nahi (data ka ghar to fix hona chahiye).
//     2. Overflow hui key doosre node pe gayi = cache ke liye ek extra miss.
//     3. `c` jitna 1 ke paas, utna balance sakht — par utni hi zyada keys apne
//        asli node se hatengi (zyada churn). Classic trade-off.
//
//  📌 Real duniya: Google ne ye Vimeo ke load balancer ke liye likha tha (HAProxy
//     ke saath), Vimeo ne report kiya ki peak load ~8x se ~1.25x pe aa gaya.
//
//  Compile: g++ -std=c++17 -Wall -Wextra 05_bounded_loads.cpp -o bounded_demo
// ============================================================================
#include <bits/stdc++.h>

#include "hash_util.h"

using namespace std;
using namespace consistent_hashing;

// ============================================================================
//  BoundedLoadRing — ring + har node pe capacity ka dhakkan
// ============================================================================
class BoundedLoadRing
{
public:
    BoundedLoadRing(int virtualNodesPerNode, double c)
        : vnodesPerNode_(virtualNodesPerNode), c_(c) {}

    void addNode(const string &nodeId)
    {
        load_[nodeId] = 0;
        for (int i = 0; i < vnodesPerNode_; ++i)
        {
            ring_[hashKey(nodeId + "#" + to_string(i))] = nodeId;
        }
    }

    // Capacity tabhi pata chalegi jab total load ka andaaza ho.
    // (Real system me ye ek rolling estimate hota hai, fixed number nahi.)
    void setExpectedTotalLoad(long long totalLoad)
    {
        capacity_ = static_cast<long long>(
            ceil((c_ * static_cast<double>(totalLoad)) / static_cast<double>(load_.size())));
    }

    long long capacity() const { return capacity_; }

    // ---- ⭐ ROUTE — bounded load ke saath -------------------------------
    // Return: {chuna gaya node, kya usko deflect karna pada}
    pair<string, bool> route(const string &key)
    {
        if (ring_.empty())
        {
            return {"", false};
        }

        auto it = ring_.lower_bound(hashKey(key));
        if (it == ring_.end())
        {
            it = ring_.begin();
        }
        string firstChoice = it->second; // jise normally milti (Step 3 wala jawab)

        // Clockwise chalo jab tak koi aisa node na mile jisme jagah bachi ho.
        // Poora ring ghoom liya to bhi kuch na mile — theoretically ho hi nahi
        // sakta, kyunki sum(capacity) = n × ceil(c×L/n) >= c×L > L.
        for (size_t steps = 0; steps < ring_.size(); ++steps)
        {
            if (it == ring_.end())
            {
                it = ring_.begin();
            }
            const string &node = it->second;
            if (load_[node] < capacity_)
            { // ⭐ jagah hai? le lo
                load_[node]++;
                return {node, node != firstChoice};
            }
            ++it; // bhara hua hai -> agla dekho
        }

        // Safety net (yahan pahunchna nahi chahiye).
        load_[firstChoice]++;
        return {firstChoice, false};
    }

    // ---- Bina capacity ke (Step 3 wala plain behavior) — comparison ke liye
    string routeUnbounded(const string &key)
    {
        auto it = ring_.lower_bound(hashKey(key));
        if (it == ring_.end())
        {
            it = ring_.begin();
        }
        load_[it->second]++;
        return it->second;
    }

    void resetLoads()
    {
        for (auto &entry : load_)
        {
            entry.second = 0;
        }
    }

    const map<string, long long> &loads() const { return load_; }

private:
    int vnodesPerNode_;
    double c_;
    long long capacity_ = 0;
    map<uint32_t, string> ring_;
    map<string, long long> load_; // nodeId -> abhi kitna load
};

// ---- Zipf-ish (hot key) traffic banane ke liye ----------------------------
// Key i ka weight 1/(i+1) — yaani key_0 sabse garam, phir aadha, phir tihaai...
// Ye asli traffic ke bahut kareeb hota hai (Zipf's law).
class ZipfKeyGenerator
{
public:
    ZipfKeyGenerator(int distinctKeys, unsigned seed) : rng_(seed)
    {
        cumulative_.reserve(distinctKeys);
        double sum = 0.0;
        for (int i = 0; i < distinctKeys; ++i)
        {
            sum += 1.0 / (i + 1);
            cumulative_.push_back(sum);
        }
        total_ = sum;
    }

    string next()
    {
        uniform_real_distribution<double> dist(0.0, total_);
        double target = dist(rng_);
        auto it = lower_bound(cumulative_.begin(), cumulative_.end(), target);
        long index = it - cumulative_.begin();
        return "key_" + to_string(index);
    }

private:
    mt19937 rng_;
    vector<double> cumulative_;
    double total_ = 0.0;
};

// ---- Load stats print -----------------------------------------------------
void printLoads(const map<string, long long> &loads, long long totalRequests)
{
    long long maxLoad = 0;
    double ideal = static_cast<double>(totalRequests) / loads.size();
    for (const auto &entry : loads)
    {
        double percent = (100.0 * entry.second) / totalRequests;
        cout << "        " << entry.first << " : " << setw(7) << entry.second << " ("
             << fixed << setprecision(1) << setw(5) << percent << "%)";
        // Chhota sa bar chart — aankh se dikh jaaye
        cout << "  " << string(static_cast<size_t>(percent / 2), '#') << "\n";
        maxLoad = max(maxLoad, entry.second);
    }
    cout << "        --> sabse bhara node average se " << fixed << setprecision(2)
         << (maxLoad / ideal) << "x hai\n";
}

int main()
{
    const int TOTAL_REQUESTS = 100000;
    // Sirf 100 distinct keys, par 1 lakh requests — jaise "top 100 trending
    // products" pe poora traffic tut padta hai. Yahi hot-key ka asli scenario.
    const int DISTINCT_KEYS = 100;
    const int VNODES = 150;
    vector<string> nodes = {"NodeA", "NodeB", "NodeC", "NodeD", "NodeE"};

    cout << "=========================================================\n";
    cout << " STEP 5 : BOUNDED LOADS (hot keys ka ilaaj)\n";
    cout << " " << TOTAL_REQUESTS << " requests, " << DISTINCT_KEYS
         << " distinct keys (Zipf), " << nodes.size() << " nodes\n";
    cout << "=========================================================\n\n";

    // Ek hi traffic dono baar chale, tabhi comparison fair hoga.
    vector<string> traffic;
    traffic.reserve(TOTAL_REQUESTS);
    {
        ZipfKeyGenerator gen(DISTINCT_KEYS, 42u); // fixed seed = repeatable
        for (int i = 0; i < TOTAL_REQUESTS; ++i)
        {
            traffic.push_back(gen.next());
        }
    }

    // Traffic kitna tedha hai, wo dikhao
    map<string, int> keyHits;
    for (const string &key : traffic)
    {
        keyHits[key]++;
    }
    cout << "[A] Traffic kitna tedha hai (top hot keys):\n";
    vector<pair<int, string>> sortedHits;
    for (const auto &entry : keyHits)
    {
        sortedHits.push_back({entry.second, entry.first});
    }
    sort(sortedHits.rbegin(), sortedHits.rend());
    for (int i = 0; i < 5; ++i)
    {
        cout << "        " << setw(8) << sortedHits[i].second << " : " << setw(6)
             << sortedHits[i].first << " hits (" << fixed << setprecision(1)
             << (100.0 * sortedHits[i].first / TOTAL_REQUESTS) << "% traffic)\n";
    }
    cout << "        => Sirf ek key " << fixed << setprecision(1)
         << (100.0 * sortedHits[0].first / TOTAL_REQUESTS)
         << "% traffic kha rahi hai. Ring ko iski khabar hi nahi.\n\n";

    // ---- Part B: BINA bounded loads ---------------------------------------
    cout << "[B] Normal consistent hashing (vnodes=" << VNODES << ", koi cap nahi):\n\n";
    BoundedLoadRing plain(VNODES, 1.25);
    for (const string &node : nodes)
    {
        plain.addNode(node);
    }
    for (const string &key : traffic)
    {
        plain.routeUnbounded(key);
    }
    printLoads(plain.loads(), TOTAL_REQUESTS);
    cout << "\n        ❌ Keys to barabar bati thi, par TRAFFIC nahi.\n"
         << "           Hot key jis node pe giri, wo node overload.\n\n";

    // ---- Part C: ⭐ BOUNDED LOADS ke saath, alag-alag `c` -----------------
    cout << "[C] Ab capacity ka dhakkan lagao — alag-alag `c` pe:\n\n";

    for (double c : {2.0, 1.5, 1.25, 1.05})
    {
        BoundedLoadRing bounded(VNODES, c);
        for (const string &node : nodes)
        {
            bounded.addNode(node);
        }
        bounded.setExpectedTotalLoad(TOTAL_REQUESTS);

        int deflected = 0;
        for (const string &key : traffic)
        {
            if (bounded.route(key).second)
            {
                ++deflected;
            }
        }

        long long maxLoad = 0;
        for (const auto &entry : bounded.loads())
        {
            maxLoad = max(maxLoad, entry.second);
        }
        double ideal = static_cast<double>(TOTAL_REQUESTS) / nodes.size();

        cout << "      c = " << fixed << setprecision(2) << c
             << "  | capacity/node = " << setw(6) << bounded.capacity()
             << "  | max load = " << setw(6) << maxLoad << " (" << setprecision(2)
             << (maxLoad / ideal) << "x avg)"
             << "  | deflected = " << fixed << setprecision(1)
             << (100.0 * deflected / TOTAL_REQUESTS) << "%\n";
    }

    cout << "\n      ⭐ Padho is table ko dhyan se — yahi poora trade-off hai:\n"
         << "         `c` ghatao  -> load utna hi barabar (max load 1.0x ke paas)\n"
         << "         `c` ghatao  -> par utni zyada requests apne asli node se hati\n"
         << "                        (deflected % badhta hai = zyada cache miss)\n";

    // ---- Part D: c=1.25 wala detail -------------------------------------
    cout << "\n[D] c=1.25 ke saath final tasveer:\n\n";
    BoundedLoadRing bounded(VNODES, 1.25);
    for (const string &node : nodes)
    {
        bounded.addNode(node);
    }
    bounded.setExpectedTotalLoad(TOTAL_REQUESTS);
    for (const string &key : traffic)
    {
        bounded.route(key);
    }
    printLoads(bounded.loads(), TOTAL_REQUESTS);
    cout << "\n        ✅ Koi bhi node capacity (" << bounded.capacity()
         << ") se upar nahi gaya — ye GUARANTEE hai, ummeed nahi.\n";

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ⚠ Keys barabar bantna ≠ traffic barabar bantna (hot keys!).\n";
    cout << "  ✅ Bounded loads: capacity = ceil(c × total / n), bhara node SKIP.\n";
    cout << "  ✅ Max load ki hard guarantee milti hai (c × average se upar nahi).\n";
    cout << "  ⚖ Trade-off: c chhota = better balance, par zyada keys apni jagah se hilti.\n";
    cout << "  ⚠ Assignment ab STATEFUL — LOAD BALANCING ke liye theek,\n";
    cout << "     permanent data placement ke liye nahi.\n";
    cout << "  📌 Google/Vimeo: peak load ~8x se ~1.25x pe aa gaya tha.\n";
    cout << "\n  Ab tak sab kuch RING pe tha. Step 6 me ek bilkul alag rasta —\n";
    cout << "  bina ring ke: RENDEZVOUS (HRW) HASHING.\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}

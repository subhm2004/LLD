// ============================================================================
//  STATIC #2 — 02_weighted_round_robin.cpp
//  WEIGHTED ROUND ROBIN — "bade server ko zyada, chhote ko kam"
// ----------------------------------------------------------------------------
//  Round Robin ki ek badi galti hai: wo maan ke chalta hai ki saare servers
//  EK JAISE hain. Asli duniya me aisa kabhi nahi hota — ek 64GB/16-core machine
//  hai aur doosri 8GB/2-core. Dono ko barabar requests dena chhoti machine ko
//  maar dega, aur badi machine khaali baithi rahegi.
//
//  ⭐ Ilaaj: har server ko ek WEIGHT do (uski capacity ka multiplier), aur
//     requests usi ratio me baanto.
//
//        S1(weight 4), S2(weight 1)  ->  S1 ko 4 guna requests
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠ SEEDHA (NAIVE) TAREEKA — jo galat hai                                 │
//  │                                                                          │
//  │  "S1 ko 4 baar do, phir S2 ko 1 baar" — yaani:                           │
//  │        S1, S1, S1, S1, S2, S1, S1, S1, S1, S2, ...                       │
//  │                                                                          │
//  │  Ratio to sahi hai (4:1), par ye BURSTY hai! S1 pe ek saath 4 requests    │
//  │  ka jhatka padta hai, phir kuch nahi. Traffic spiky ho jaata hai —        │
//  │  connection pools, CPU spikes, aur cache behaviour sab bigadta hai.       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SMOOTH WEIGHTED ROUND ROBIN (nginx wala tareeka) — yahi sahi hai      │
//  │                                                                          │
//  │  Har server ka ek `currentWeight` hota hai. Har request pe:               │
//  │                                                                          │
//  │    1. har server: currentWeight += weight                                 │
//  │    2. jiska currentWeight sabse ZYADA, wo jeeta                           │
//  │    3. jeetne wale ka: currentWeight -= (sab weights ka total)             │
//  │                                                                          │
//  │  Step 3 hi jaadu hai — jeetne wale ko "peeche" dhakel deta hai taaki      │
//  │  agli baar doosron ka number aaye. Nateeja: wahi 4:1 ratio, par           │
//  │  requests SMOOTH tareeke se bikhri hui (S1,S1,S2,S1,S1 jaisa).            │
//  │                                                                          │
//  │  Yahi algorithm nginx aur LVS asal me use karte hain.                    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ✅ Faayde   : alag-alag capacity ke servers sahi se handle hote hain
//  ❌ Nuksan   : weight HAATH SE set karna padta hai, aur wo STATIC hota hai —
//                agar koi server abhi sach me bhara hua hai to bhi uska weight
//                wahi rahega (real-time load phir bhi ignore hoti hai)
//  📌 Kab use  : servers alag-alag capacity ke hon, par requests lagbhag ek jaisi
//
//  Compile: g++ -std=c++17 -Wall -Wextra 02_weighted_round_robin.cpp -o wrr_demo
// ============================================================================
#include <bits/stdc++.h>

#include "../lb_common.h"

using namespace std;
using namespace lb;

// ============================================================================
//  SmoothWeightedRoundRobin — nginx wala algorithm
// ============================================================================
class SmoothWeightedRoundRobin
{
public:
    explicit SmoothWeightedRoundRobin(const vector<int> &weights)
        : weights_(weights), currentWeights_(weights.size(), 0)
    {
        totalWeight_ = 0;
        for (int weight : weights_)
        {
            totalWeight_ += weight;
        }
    }

    // ---- ⭐ POORA ALGORITHM — teen step -----------------------------------
    size_t selectNext()
    {
        size_t best = 0;

        // Step 1: har server apna weight jama karta hai
        for (size_t i = 0; i < currentWeights_.size(); ++i)
        {
            currentWeights_[i] += weights_[i];

            // Step 2: sabse zyada currentWeight wala jeetega
            if (currentWeights_[i] > currentWeights_[best])
            {
                best = i;
            }
        }

        // Step 3: jeetne wale ko peeche dhakelo (yahi "smooth" banata hai)
        currentWeights_[best] -= totalWeight_;
        return best;
    }

private:
    vector<int> weights_;
    vector<int> currentWeights_;
    int totalWeight_;
};

int main()
{
    cout << "=========================================================\n";
    cout << " STATIC #2 : WEIGHTED ROUND ROBIN\n";
    cout << " 2 bade servers (weight 16) + 2 chhote (weight 4)  =  4:1 ratio\n";
    cout << "=========================================================\n\n";

    vector<string> names = {"S1-big", "S2-big", "S3-sml", "S4-sml"};
    vector<int> weights = {4, 4, 1, 1}; // ratio dikhane ke liye chhote numbers

    // ---- Part A: naive vs smooth — sequence dekho ------------------------
    cout << "[A] Wahi 4:1:4:1 ratio, do alag tareeko se (pehli 20 requests):\n\n";

    cout << "    NAIVE (bursty)  : ";
    for (size_t i = 0; i < names.size(); ++i)
    {
        for (int w = 0; w < weights[i]; ++w)
        {
            cout << names[i].substr(0, 2) << " ";
        }
    }
    for (size_t i = 0; i < names.size(); ++i)
    {
        for (int w = 0; w < weights[i]; ++w)
        {
            cout << names[i].substr(0, 2) << " ";
        }
    }
    cout << "\n                      ^ dekho: S1 pe 4 ka JHATKA, phir S2 pe 4 ka...\n\n";

    cout << "    SMOOTH (nginx)  : ";
    {
        SmoothWeightedRoundRobin smooth(weights);
        for (int i = 0; i < 20; ++i)
        {
            cout << names[smooth.selectNext()].substr(0, 2) << " ";
        }
    }
    cout << "\n                      ^ wahi ratio, par achhe se bikhra hua ⭐\n\n";

    // Ratio verify karo — 1000 requests pe
    {
        SmoothWeightedRoundRobin smooth(weights);
        vector<int> counts(names.size(), 0);
        for (int i = 0; i < 1000; ++i)
        {
            counts[smooth.selectNext()]++;
        }
        cout << "    1000 requests baad ginti: ";
        for (size_t i = 0; i < names.size(); ++i)
        {
            cout << names[i] << "=" << counts[i] << (i + 1 < names.size() ? ", " : "\n");
        }
        cout << "    => Bilkul 4:4:1:1 ka ratio. Smooth hone se ratio bigda nahi. ✅\n\n";
    }

    // ---- Part B: PLAIN Round Robin unequal servers pe (disaster) ---------
    cout << "[B] Pehle dekho PLAIN Round Robin in servers pe kya karta hai:\n\n";

    vector<Server> rrServers = makeWeightedServers();
    size_t nextIndex = 0;
    Selector plainRoundRobin = [&nextIndex](const vector<Server> &pool, const string &ip) -> size_t
    {
        (void)ip;
        return nextIndex++ % pool.size();
    };
    SimResult rrResult = runSimulation(rrServers, plainRoundRobin);

    printServerTable(rrServers);
    cout << "\n";
    printResult(rrResult);
    cout << "\n    ❌ Chhote servers (S3-sml, S4-sml) ko bade jitni hi requests mili,\n"
         << "       par unki capacity 1/4 hai. Unki latency aasman pe pahunch gayi.\n";

    // ---- Part C: WEIGHTED Round Robin ------------------------------------
    cout << "\n[C] Ab WEIGHTED Round Robin (capacity ke hisaab se):\n\n";

    vector<Server> wrrServers = makeWeightedServers();

    // Servers ke asli weights se smooth WRR banao
    vector<int> actualWeights;
    for (const Server &server : wrrServers)
    {
        actualWeights.push_back(server.weight);
    }
    SmoothWeightedRoundRobin scheduler(actualWeights);

    Selector weightedRoundRobin = [&scheduler](const vector<Server> &pool, const string &ip) -> size_t
    {
        (void)pool;
        (void)ip; // ⭐ ye bhi STATIC hai — server ki live state nahi dekhta
        return scheduler.selectNext();
    };
    SimResult wrrResult = runSimulation(wrrServers, weightedRoundRobin);

    printServerTable(wrrServers);
    cout << "\n";
    printResult(wrrResult);

    // ---- Part D: muqabla -------------------------------------------------
    cout << "\n[D] ⭐ MUQABLA — wahi servers, wahi traffic, sirf algorithm alag:\n\n";
    cout << "                        | Plain RR | Weighted RR\n";
    cout << "    --------------------+----------+------------\n";
    cout << "    Avg latency         | " << setw(8) << fixed << setprecision(2)
         << rrResult.avgLatency << " | " << setw(11) << wrrResult.avgLatency << "\n";
    cout << "    p95 latency         | " << setw(8) << rrResult.p95Latency << " | " << setw(11)
         << wrrResult.p95Latency << "\n";
    cout << "    Max latency         | " << setw(8) << rrResult.maxLatency << " | " << setw(11)
         << wrrResult.maxLatency << "\n";

    double improvement = rrResult.avgLatency > 0
                             ? (100.0 * (rrResult.avgLatency - wrrResult.avgLatency) / rrResult.avgLatency)
                             : 0.0;
    cout << "\n    ⭐ Weighted RR ne average latency " << setprecision(1) << improvement
         << "% kam kar di —\n";
    cout << "       sirf isliye ki usne servers ki CAPACITY ka khayal rakha.\n";

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Alag capacity ke servers ab sahi ratio me load uthate hain.\n";
    cout << "  ✅ Smooth WRR (nginx wala) ratio bhi deta hai aur burst bhi nahi.\n";
    cout << "  ✅ Abhi bhi O(n) per request — bahut sasta.\n";
    cout << "  ❌ Weight HAATH se set karna padta (capacity aap batao).\n";
    cout << "  ❌ Ye ab bhi STATIC hai — server ABHI bhara hai ya khaali, dikhta nahi.\n";
    cout << "     Ek server pe 3 bhaari requests aa gayi? WRR phir bhi usko\n";
    cout << "     uske ratio ke hisaab se request dega.\n";
    cout << "\n  Real-time load dekhne ka ilaaj -> DYNAMIC algorithms.\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}

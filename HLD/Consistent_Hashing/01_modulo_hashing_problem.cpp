// ============================================================================
//  STEP 1 — 01_modulo_hashing_problem.cpp
//  "Pehle PROBLEM samjho, tabhi solution ki keemat pata chalegi"
// ----------------------------------------------------------------------------
//  Scenario: 4 cache servers hain, aur crores keys unme baantni hain.
//  Sabse seedha jawab jo har kisi ke dimaag me aata hai:
//
//        node_index = hash(key) % N
//
//  Aur sach ye hai ki ye BILKUL SAHI kaam karta hai... jab tak N nahi badalta.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ ASLI SAWAAL — "distribution even hai kya?" NAHI.                      │
//  │     Asli sawaal hai: "jab N badle to kitni keys apni jagah badalti hain?" │
//  │                                                                          │
//  │  Modulo ka distribution to bilkul even hota hai (ye demo dikhayega).      │
//  │  Uski maut is baat se hoti hai ki N badalte hi `% N` ka poora mapping     │
//  │  hi badal jaata hai — key ka hash wahi rehta hai, par remainder alag।     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ Ye demo ye NAAP kar dikhayega ki 4 -> 5 nodes pe jaane par kitne % keys
//     move hoti hain. Jawab ~80% aayega (theory: 1 - 1/N_new). Yaani ek server
//     add karne ki keemat = 80% cache miss = DB pe achanak 80% traffic.
//
//  Compile: g++ -std=c++17 -Wall -Wextra 01_modulo_hashing_problem.cpp -o modulo_demo
// ============================================================================
#include <bits/stdc++.h>

#include "hash_util.h"

using namespace std;
using namespace consistent_hashing;

// ---- Modulo hashing ka poora "algorithm" — bas ek line ----------------------
// Isi ek line ki wajah se saara problem hai: node ka index N pe DEPEND karta hai.
int getNodeIndex(const string &key, int numNodes)
{
    return static_cast<int>(hashKey(key) % static_cast<uint32_t>(numNodes));
}

// ---- Test ke liye nakli keys: "user_0", "user_1", ... -----------------------
vector<string> makeKeys(int count)
{
    vector<string> keys;
    keys.reserve(count);
    for (int i = 0; i < count; ++i)
    {
        keys.push_back("user_" + to_string(i));
    }
    return keys;
}

// ---- Kaun si key kis node pe gayi, uska count nikaalo ----------------------
// Ye sirf ye dikhane ke liye ki distribution to EVEN hai — problem wo nahi hai.
void printDistribution(const vector<string> &keys, int numNodes)
{
    vector<int> countPerNode(numNodes, 0);
    for (const string &key : keys)
    {
        countPerNode[getNodeIndex(key, numNodes)]++;
    }

    cout << "  " << numNodes << " nodes pe distribution:\n";
    for (int i = 0; i < numNodes; ++i)
    {
        double percent = (100.0 * countPerNode[i]) / keys.size();
        cout << "    Node " << i << " : " << setw(6) << countPerNode[i]
             << " keys (" << fixed << setprecision(1) << percent << "%)\n";
    }
}

// ---- ⭐ ASLI NAAP — N badalne pe kitni keys ne node badla? ------------------
// Har key ka purana node aur naya node compare karo. Alag hai? => wo key "move"
// hui. Cache me iska matlab: purane node pe entry bekaar, naye node pe MISS.
double measureRemapPercent(const vector<string> &keys, int oldN, int newN)
{
    int moved = 0;
    for (const string &key : keys)
    {
        if (getNodeIndex(key, oldN) != getNodeIndex(key, newN))
        {
            ++moved;
        }
    }
    return (100.0 * moved) / keys.size();
}

int main()
{
    const int TOTAL_KEYS = 100000;
    vector<string> keys = makeKeys(TOTAL_KEYS);

    cout << "=========================================================\n";
    cout << " STEP 1 : SIMPLE MODULO HASHING  ->  hash(key) % N\n";
    cout << " Total keys: " << TOTAL_KEYS << "\n";
    cout << "=========================================================\n\n";

    // ---- Part A: distribution dekho — ye to bilkul theek hai ---------------
    cout << "[A] Distribution (modulo ki ye baat ACHHI hai):\n";
    printDistribution(keys, 4);
    cout << "\n  => Har node ko ~25% keys. Balance perfect hai.\n"
         << "     Yaani modulo ka problem 'uneven load' NAHI hai.\n\n";

    // ---- Part B: ab ek node add karo — aur yahin sab bikhar jaata hai ------
    cout << "[B] Ab ek node ADD karo (4 -> 5) aur naapo kitni keys move hui:\n\n";

    double moved4to5 = measureRemapPercent(keys, 4, 5);
    cout << "    4 -> 5 nodes : " << fixed << setprecision(1) << moved4to5
         << "% keys ne node BADAL diya\n";

    // Ek node hatane pe bhi wahi kahani.
    double moved4to3 = measureRemapPercent(keys, 4, 3);
    cout << "    4 -> 3 nodes : " << moved4to3 << "% keys ne node BADAL diya\n\n";

    // Aur bade cluster me haalat aur kharaab — theory: 1 - 1/N_new.
    cout << "    Bade cluster me aur bura (theory: 1 - 1/N_new):\n";
    const int sizes[][2] = {{10, 11}, {50, 51}, {100, 101}};
    for (const auto &pair : sizes)
    {
        cout << "      " << setw(3) << pair[0] << " -> " << setw(3) << pair[1]
             << " nodes : " << measureRemapPercent(keys, pair[0], pair[1]) << "% moved\n";
    }

    // ---- Part C: ek key ka safar — problem "aankhon se" dekho --------------
    cout << "\n[C] Ek hi key ka safar (hash to kabhi badla hi nahi!):\n";
    const string sample = "user_42";
    uint32_t h = hashKey(sample);
    cout << "    key = \"" << sample << "\", hash = " << h << " (ye CONSTANT hai)\n";
    for (int n = 3; n <= 7; ++n)
    {
        cout << "      N=" << n << " -> " << h << " % " << n << " = Node "
             << getNodeIndex(sample, n) << "\n";
    }
    cout << "    => Key wahi, hash wahi, par har N pe alag node. Yahi jadd hai.\n";

    // ---- Nateeja ----------------------------------------------------------
    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  * Distribution even hai — wo problem nahi.\n";
    cout << "  * Par ek server add/remove karte hi ~" << fixed << setprecision(0)
         << moved4to5 << "% keys jagah badal deti hain.\n";
    cout << "  * Cache me = mass MISS -> saara traffic DB pe (cache stampede).\n";
    cout << "  * Sharded DB me = TB-scale data movement -> ghanto ka rebalance.\n";
    cout << "  * Isliye jaha nodes aate-jaate rehte hain, wahan `% N` unusable hai.\n";
    cout << "\n  Chahiye kya: node add/remove pe sirf ~1/N keys move hon.\n";
    cout << "  Wahi STEP 2 (hash ring) me banayenge.\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}

// ============================================================================
//  STEP 3 — 03_virtual_nodes.cpp
//  VIRTUAL NODES — ek hi ilaaj, Step 2 ki DONO bimariyon ka
// ----------------------------------------------------------------------------
//  Step 2 me ring to chal gaya, par do dard reh gaye:
//     ❌ Load tedha  — kisi node ko 68% keys, kisi ko 1%.
//     ❌ Node marne pe uska POORA bojh ek hi padosi pe gira.
//
//  Dono ki jadd EK hi hai: har node ring pe SIRF EK point tha. Aur sirf 4-5
//  random points daalo to arcs barabar ho hi nahi sakte (kismat ki baat hai).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ VIRTUAL NODES (vnodes) — "ek node ko ring pe 150 jagah rakh do"       │
//  │                                                                          │
//  │  NodeA ko ek point pe rakhne ki jagah, 150 points pe rakho:               │
//  │      hash("NodeA#0"), hash("NodeA#1"), ... hash("NodeA#149")              │
//  │  Ye 150 points poore ring pe bikhar jaayenge. Har ek chhota sa arc.       │
//  │                                                                          │
//  │  Isse do jaadu ek saath hote hain:                                        │
//  │   1️⃣ EVEN LOAD — 4 bade random arcs ki jagah 600 chhote arcs. Law of      │
//  │      large numbers: jitne zyada tukde, utna barabar batwara.              │
//  │   2️⃣ EVEN REBALANCE — NodeA mara to uske 150 alag-alag arcs the, aur      │
//  │      har arc ka padosi ALAG node hai. Isliye uska bojh SAARE bache        │
//  │      nodes me baant jaata hai — kisi ek pe nahi girta.                    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ "Virtual" ka matlab: koi asli server nahi hai. Sirf ring pe ek aur ENTRY
//     hai jo ussi physical node ko point karti hai. Cost = thodi si memory
//     (nodes × vnodes entries), aur kuch nahi.
//
//  📌 Real duniya me: Cassandra default 256 vnodes/node, DynamoDB & Riak bhi
//     100-256 range me. Ye demo dikhayega ki ye number aaya kahan se.
//
//  Compile: g++ -std=c++17 -Wall -Wextra 03_virtual_nodes.cpp -o vnodes_demo
// ============================================================================
#include <bits/stdc++.h>

#include "hash_util.h"

using namespace std;
using namespace consistent_hashing;

// ============================================================================
//  VNodeHashRing — ab har physical node ke ring pe kai points
// ============================================================================
class VNodeHashRing
{
public:
    explicit VNodeHashRing(int virtualNodesPerNode)
        : vnodesPerNode_(virtualNodesPerNode) {}

    // ---- Node add — ek nahi, `vnodesPerNode_` entries daalo ---------------
    void addNode(const string &nodeId)
    {
        for (int i = 0; i < vnodesPerNode_; ++i)
        {
            // ⭐ Har vnode ka apna alag naam -> apni alag ring position.
            //    "NodeA#0", "NodeA#1", ... — hash alag, isliye jagah alag.
            //    Value me PHYSICAL node ka naam rakhte hain, kyunki lookup ko
            //    aakhir me asli server chahiye, vnode ka naam nahi.
            ring_[hashKey(virtualNodeName(nodeId, i))] = nodeId;
        }
    }

    // ---- Node remove — uske saare vnodes hatao ----------------------------
    void removeNode(const string &nodeId)
    {
        for (int i = 0; i < vnodesPerNode_; ++i)
        {
            ring_.erase(hashKey(virtualNodeName(nodeId, i)));
        }
    }

    // ---- Lookup — Step 2 se BILKUL same, ek line bhi nahi badli ----------
    // ⭐ Yahi vnodes ki khoobsurti hai: lookup ko pata bhi nahi ki vnodes hain.
    //    Wo bas "clockwise agla point" dhundhta hai; point kis physical node ka
    //    hai, wo map ki value bata deti hai.
    string getNode(const string &key) const
    {
        if (ring_.empty())
        {
            return "";
        }
        auto it = ring_.lower_bound(hashKey(key));
        if (it == ring_.end())
        {
            it = ring_.begin(); // wrap around
        }
        return it->second;
    }

    size_t ringPoints() const { return ring_.size(); }

private:
    static string virtualNodeName(const string &nodeId, int index)
    {
        return nodeId + "#" + to_string(index);
    }

    int vnodesPerNode_;
    map<uint32_t, string> ring_; // position -> PHYSICAL node id
};

// ---- Helpers ---------------------------------------------------------------
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

map<string, int> countLoad(const VNodeHashRing &ring, const vector<string> &keys)
{
    map<string, int> load;
    for (const string &key : keys)
    {
        load[ring.getNode(key)]++;
    }
    return load;
}

// ---- Distribution kitna "even" hai — ise NAAPNA zaroori hai ---------------
// Sirf aankh se dekhne ki jagah ek number chahiye. Do use karte hain:
//   * spread     = (max - min) / average  -> kitna faasla hai sabse bhare aur
//                  sabse khaali node me (0 = perfect)
//   * std dev %  -> average se kitna bhatak rahe hain
struct Spread
{
    double maxPercent;
    double minPercent;
    double stdDevPercent;
};

Spread measureSpread(const map<string, int> &load, int totalKeys)
{
    double idealPercent = 100.0 / load.size();
    double maxP = 0.0, minP = 100.0, sumSq = 0.0;

    for (const auto &entry : load)
    {
        double percent = (100.0 * entry.second) / totalKeys;
        maxP = max(maxP, percent);
        minP = min(minP, percent);
        sumSq += (percent - idealPercent) * (percent - idealPercent);
    }
    return {maxP, minP, sqrt(sumSq / load.size())};
}

int main()
{
    const int TOTAL_KEYS = 100000;
    const int NUM_NODES = 5;
    vector<string> keys = makeKeys(TOTAL_KEYS);
    vector<string> nodes = {"NodeA", "NodeB", "NodeC", "NodeD", "NodeE"};

    cout << "=========================================================\n";
    cout << " STEP 3 : VIRTUAL NODES\n";
    cout << " " << TOTAL_KEYS << " keys, " << NUM_NODES << " nodes"
         << "  |  ideal share = " << fixed << setprecision(1) << (100.0 / NUM_NODES) << "%\n";
    cout << "=========================================================\n\n";

    // ---- Part A: vnode count badhao, aur distribution sudharta dekho -------
    cout << "[A] Vnodes badhane se distribution kaise sudharta hai:\n\n";
    cout << "    vnodes/node | ring points |   max%  |   min%  | std-dev\n";
    cout << "    ------------+-------------+---------+---------+---------\n";

    for (int vnodes : {1, 5, 20, 50, 100, 200, 500})
    {
        VNodeHashRing ring(vnodes);
        for (const string &node : nodes)
        {
            ring.addNode(node);
        }
        Spread s = measureSpread(countLoad(ring, keys), TOTAL_KEYS);
        cout << "    " << setw(11) << vnodes << " | " << setw(11) << ring.ringPoints()
             << " | " << setw(6) << fixed << setprecision(2) << s.maxPercent
             << "% | " << setw(6) << s.minPercent << "% | " << setw(6)
             << s.stdDevPercent << "\n";
    }

    cout << "\n    ⭐ vnodes=1 pe haalat kharaab (Step 2 wali). ~100-200 pe aakar\n"
         << "       curve flat ho jaata hai — isiliye Cassandra jaise systems\n"
         << "       100-256 pe rukte hain. Aage badhane se sirf memory badhti hai.\n\n";

    // ---- Part B: 1 vs 150 vnodes ka side-by-side --------------------------
    cout << "[B] Side by side — har node ko kitni keys mili:\n\n";

    VNodeHashRing plain(1), withVnodes(150);
    for (const string &node : nodes)
    {
        plain.addNode(node);
        withVnodes.addNode(node);
    }
    map<string, int> plainLoad = countLoad(plain, keys);
    map<string, int> vnodeLoad = countLoad(withVnodes, keys);

    cout << "      node   |  vnodes=1        |  vnodes=150\n";
    cout << "      -------+------------------+------------------\n";
    for (const string &node : nodes)
    {
        cout << "      " << node << "  | " << setw(6) << plainLoad[node] << " ("
             << fixed << setprecision(1) << setw(4) << (100.0 * plainLoad[node] / TOTAL_KEYS)
             << "%) | " << setw(6) << vnodeLoad[node] << " (" << setw(4)
             << (100.0 * vnodeLoad[node] / TOTAL_KEYS) << "%)\n";
    }
    cout << "\n      => Left me koi node bhookha koi bhara. Right me sab ~20%. ⭐\n\n";

    // ---- Part C: ⭐ DOOSRA faayda — node failure ka bojh BAT jaata hai -----
    cout << "[C] Ab NodeC ko maaro — aur dekho uska bojh kaha gaya:\n\n";

    // -- vnodes=1 ke saath: saara bojh EK padosi pe
    unordered_map<string, string> beforePlain;
    for (const string &key : keys)
    {
        beforePlain[key] = plain.getNode(key);
    }
    plain.removeNode("NodeC");
    map<string, int> inheritedPlain;
    for (const auto &entry : beforePlain)
    {
        if (entry.second == "NodeC")
        {
            inheritedPlain[plain.getNode(entry.first)]++;
        }
    }
    cout << "      vnodes=1  -> NodeC ki keys inhone li:\n";
    for (const auto &entry : inheritedPlain)
    {
        cout << "         " << entry.first << " : " << entry.second << " keys\n";
    }
    cout << "         ❌ Sirf " << inheritedPlain.size()
         << " node(s) ne poora bojh utha liya -> wo bhi gir sakta hai (cascading failure).\n\n";

    // -- vnodes=150 ke saath: bojh SAB me baant gaya
    unordered_map<string, string> beforeVnodes;
    for (const string &key : keys)
    {
        beforeVnodes[key] = withVnodes.getNode(key);
    }
    withVnodes.removeNode("NodeC");
    map<string, int> inheritedVnodes;
    int untouched = 0;
    for (const auto &entry : beforeVnodes)
    {
        if (entry.second == "NodeC")
        {
            inheritedVnodes[withVnodes.getNode(entry.first)]++;
        }
        else if (withVnodes.getNode(entry.first) == entry.second)
        {
            ++untouched;
        }
    }
    cout << "      vnodes=150 -> NodeC ki keys inhone li:\n";
    for (const auto &entry : inheritedVnodes)
    {
        cout << "         " << entry.first << " : " << entry.second << " keys\n";
    }
    cout << "         ✅ Saare " << inheritedVnodes.size()
         << " bache nodes me barabar bat gaya — kisi pe spike nahi.\n";
    cout << "         ✅ Aur baaki " << untouched << " keys bilkul nahi hili.\n";

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Vnodes = ek physical node ki ring pe kai chhoti entries.\n";
    cout << "  ✅ Faayda 1: load lagbhag perfectly even (~" << fixed << setprecision(0)
         << (100.0 / NUM_NODES) << "% har node).\n";
    cout << "  ✅ Faayda 2: node marne pe uska bojh SAARE nodes me bat jaata hai.\n";
    cout << "  ✅ Lookup code me ek line bhi nahi badli — sirf entries zyada hain.\n";
    cout << "  📌 Sweet spot 100-256 vnodes/node (ye trade-off: evenness vs memory).\n";
    cout << "  💰 Cost: O(nodes × vnodes) memory + utna hi bada map.\n";
    cout << "\n  Ab load theek hai. Par data ki ek hi copy hai — node mara to data gaya.\n";
    cout << "  Uska ilaaj -> Step 4 (REPLICATION).\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}

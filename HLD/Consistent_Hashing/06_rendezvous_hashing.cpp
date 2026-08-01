// ============================================================================
//  STEP 6 — 06_rendezvous_hashing.cpp
//  RENDEZVOUS (HRW) HASHING — bina ring ke wahi kaam
// ----------------------------------------------------------------------------
//  Ab tak humne ring banaya, vnodes daale, sorted map sambhala. Ek sawaal:
//  kya bina ring ke bhi ye ho sakta hai?
//
//  Ho sakta hai. Rendezvous Hashing (aka HRW — Highest Random Weight), 1996 —
//  consistent hashing se bhi purana, aur soch me kahin zyada seedha:
//
//     ┌────────────────────────────────────────────────────────────────────┐
//     │  Har (key, node) JODI ka ek score nikalo:  hash(key + node_id)      │
//     │  Jis node ka score sabse ZYADA — key uski. Bas. Khatam.             │
//     └────────────────────────────────────────────────────────────────────┘
//
//  Koi ring nahi, koi sorted map nahi, koi virtual node nahi, koi wrap-around
//  nahi. Bas ek loop aur ek max().
//
//  ⭐ Ye "consistent" kyun hai? Sochiye — NodeC hata diya. Baaki har (key,node)
//     jodi ka score to wahi ka wahi hai (wo sirf key aur node ke naam pe depend
//     karta hai). To jis key ka winner NodeC nahi tha, uska winner aaj bhi wahi
//     hai — kuch nahi badla. Sirf NodeC ki keys ko naya winner dhoondhna padega,
//     aur wo "second best" pe chali jaayengi — jo har key ke liye alag node hai.
//     Isliye bojh apne aap sab me bat jaata hai. Vnodes ki zaroorat hi nahi padi.
//
//  ⚖ RING vs RENDEZVOUS:
//     * Lookup speed  — Ring O(log V) jeetta. HRW O(n): har node ka score. 1000
//       nodes = 1000 hash har lookup. Chhote clusters me farak nahi padta, bade
//       me padta hai. (Iska ek ilaaj: nodes ko tree me rakho -> O(log n).)
//     * Distribution  — HRW bina kisi tuning ke perfectly even. Ring ko 150
//       vnodes ki bakhedi karni padti hai.
//     * Memory        — HRW me bas node list. Ring me nodes × vnodes entries.
//     * Replication   — HRW me top-N score le lo, bas. Ring me clockwise chalke
//       distinct physical nodes dhoondho (jo Step 4 me thoda tricky tha).
//     * Weights       — HRW me alag capacity wale nodes handle karna aasan hai
//       (score ko weight se scale kar do). Ring me weight = zyada vnodes.
//
//  📌 Kaha use hota: Apache Ignite, Ceph (CRUSH isi family se), kai CDN aur
//     internal LB systems. Chhote/medium node counts pe ye aksar behtar choice hai.
//
//  Compile: g++ -std=c++17 -Wall -Wextra 06_rendezvous_hashing.cpp -o rendezvous_demo
// ============================================================================
#include <bits/stdc++.h>

#include "hash_util.h"

using namespace std;
using namespace consistent_hashing;

// ============================================================================
//  RendezvousHash — poora algorithm, bas itna sa
// ============================================================================
class RendezvousHash
{
public:
    void addNode(const string &nodeId) { nodes_.push_back(nodeId); }

    void removeNode(const string &nodeId)
    {
        nodes_.erase(remove(nodes_.begin(), nodes_.end(), nodeId), nodes_.end());
    }

    // ---- ⭐ POORA ALGORITHM — ek loop, ek max ---------------------------
    string getNode(const string &key) const
    {
        string winner;
        uint32_t bestScore = 0;

        for (const string &node : nodes_)
        {
            uint32_t score = weightFor(key, node);
            // ">" nahi ">=" bhi chalega, par tie pe deterministic rehna zaroori
            // hai. Tie tootne ke liye node id compare kar lete hain — taaki har
            // machine pe jawab BILKUL same aaye.
            if (score > bestScore || (score == bestScore && node > winner))
            {
                bestScore = score;
                winner = node;
            }
        }
        return winner;
    }

    // ---- Replication bhi utna hi aasan: top-N scores --------------------
    // ⭐ Ring wali "distinct physical node" ki bakhedi yahan hai hi nahi —
    //    har node list me ek hi baar hai, to top-N apne aap alag nodes hain.
    vector<string> getNodes(const string &key, size_t count) const
    {
        vector<pair<uint32_t, string>> scored;
        scored.reserve(nodes_.size());
        for (const string &node : nodes_)
        {
            scored.push_back({weightFor(key, node), node});
        }
        // Sabse bade score pehle. Sirf top-N chahiye to partial_sort kaafi hai.
        size_t want = min(count, scored.size());
        partial_sort(scored.begin(), scored.begin() + want, scored.end(),
                     greater<pair<uint32_t, string>>());

        vector<string> result;
        for (size_t i = 0; i < want; ++i)
        {
            result.push_back(scored[i].second);
        }
        return result;
    }

    size_t nodeCount() const { return nodes_.size(); }

private:
    // ⭐ Score = hash(key + "|" + node). Separator "|" isliye taaki
    //    ("ab", "c") aur ("a", "bc") ka score galti se same na ho jaaye.
    static uint32_t weightFor(const string &key, const string &node)
    {
        return hashKey(key + "|" + node);
    }

    vector<string> nodes_; // bas itni si state — koi ring, koi vnode nahi
};

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

int main()
{
    const int TOTAL_KEYS = 100000;
    vector<string> keys = makeKeys(TOTAL_KEYS);
    vector<string> nodes = {"NodeA", "NodeB", "NodeC", "NodeD", "NodeE"};

    RendezvousHash hrw;
    for (const string &node : nodes)
    {
        hrw.addNode(node);
    }

    cout << "=========================================================\n";
    cout << " STEP 6 : RENDEZVOUS (HRW) HASHING — ring ke bina\n";
    cout << " " << TOTAL_KEYS << " keys, " << nodes.size() << " nodes, 0 virtual nodes\n";
    cout << "=========================================================\n\n";

    // ---- Part A: ek key ka faisla khol ke dikhao -------------------------
    cout << "[A] Ek key ka faisla — har node ka score, sabse bada jeeta:\n\n";
    const string sampleKey = "user_42";
    cout << "      key = \"" << sampleKey << "\"\n";
    for (const string &node : nodes)
    {
        uint32_t score = hashKey(sampleKey + "|" + node);
        cout << "        hash(\"" << sampleKey << "|" << node << "\") = " << setw(10)
             << score << (node == hrw.getNode(sampleKey) ? "   <-- SABSE BADA = winner ⭐" : "")
             << "\n";
    }

    // ---- Part B: distribution — bina kisi tuning ke ----------------------
    cout << "\n[B] Distribution (yaad rakho: ZERO virtual nodes, zero tuning):\n\n";
    map<string, int> load;
    for (const string &key : keys)
    {
        load[hrw.getNode(key)]++;
    }
    double ideal = 100.0 / nodes.size(), sumSq = 0.0;
    for (const auto &entry : load)
    {
        double percent = (100.0 * entry.second) / TOTAL_KEYS;
        sumSq += (percent - ideal) * (percent - ideal);
        cout << "        " << entry.first << " : " << setw(6) << entry.second << " ("
             << fixed << setprecision(2) << percent << "%)\n";
    }
    cout << "        std-dev = " << fixed << setprecision(2) << sqrt(sumSq / load.size())
         << "  (Step 3 me ring ko yahan aane ke liye 150-200 vnodes lage the!)\n";

    // ---- Part C: consistency test — node hatao ---------------------------
    cout << "\n[C] Ab NodeC hatao — kitni keys hili?\n\n";
    unordered_map<string, string> before;
    for (const string &key : keys)
    {
        before[key] = hrw.getNode(key);
    }

    hrw.removeNode("NodeC");

    int movedNecessary = 0, movedUnnecessary = 0;
    map<string, int> inherited;
    for (const string &key : keys)
    {
        string now = hrw.getNode(key);
        if (before[key] == "NodeC")
        {
            ++movedNecessary;
            inherited[now]++;
        }
        else if (before[key] != now)
        {
            ++movedUnnecessary; // ye kabhi nahi hona chahiye
        }
    }
    cout << "        NodeC ki keys (inko to jaana hi tha) : " << movedNecessary << "\n";
    cout << "        Baaki keys jo bewajah hili           : " << movedUnnecessary
         << "   <-- 0 ✅ (yahi 'consistent' ka matlab)\n";
    cout << "\n        NodeC ka bojh kaha bata:\n";
    for (const auto &entry : inherited)
    {
        cout << "          " << entry.first << " : " << entry.second << " keys ("
             << fixed << setprecision(1) << (100.0 * entry.second / movedNecessary) << "%)\n";
    }
    cout << "        ⭐ Bina vnodes ke bhi bojh saare nodes me barabar bata!\n"
         << "           (Ring me ye kaam 150 vnodes ke bina hota hi nahi tha.)\n";

    // ---- Part D: replication -------------------------------------------
    cout << "\n[D] Replication = top-3 scores. Bas.\n\n";
    hrw.addNode("NodeC"); // wapas jodo
    for (int i = 0; i < 5; ++i)
    {
        string key = "user_" + to_string(i);
        vector<string> replicas = hrw.getNodes(key, 3);
        cout << "        " << setw(8) << key << " -> [ ";
        for (size_t r = 0; r < replicas.size(); ++r)
        {
            cout << replicas[r] << (r + 1 < replicas.size() ? ", " : "");
        }
        cout << " ]\n";
    }
    cout << "\n        Step 4 me ring pe ye karne ke liye clockwise chalke\n"
         << "        'distinct physical node' filter karna pada tha. Yahan zaroorat hi nahi.\n";

    // ---- Part E: keemat — speed --------------------------------------
    cout << "\n[E] Par muft kuch nahi — LOOKUP COST:\n\n";
    cout << "        Ring       : O(log(nodes × vnodes))  -> 5 nodes × 150 = ~9 steps\n";
    cout << "        Rendezvous : O(nodes) hashes         -> 5 hashes\n";
    cout << "        Chhote cluster me HRW aage. Par 1000 nodes pe:\n";
    cout << "        Ring       : ~17 steps  |  Rendezvous : 1000 hashes ❌\n";
    cout << "        => Bade clusters me ring jeetta hai.\n";

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA — kab kya use karein:\n";
    cout << "  ✅ RENDEZVOUS: code chhota, distribution bina tuning ke perfect,\n";
    cout << "     replication trivial (top-N), weights aasan. Nodes kam hon to best.\n";
    cout << "  ✅ RING: lookup O(log n) — HAZAARON nodes pe yahi chalega.\n";
    cout << "     Aur bounded loads (Step 5) jaise extensions ring pe hi bane hain.\n";
    cout << "  📌 Interview me dono ka naam lena impress karta hai — khaas kar ye\n";
    cout << "     bolna ki 'chhote cluster pe main rendezvous prefer karunga\n";
    cout << "     kyunki vnodes ki tuning hi nahi karni padti'.\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}

// ============================================================================
//  STEP 4 — 04_replication.cpp
//  REPLICATION — "node mar gaya to data bhi mar gaya" ka ilaaj
// ----------------------------------------------------------------------------
//  Ab tak har key EK node pe thi. Wo node gaya -> wo data gaya. Distributed
//  system me ye manzoor nahi (nodes to marte hi rehte hain — wo normal hai).
//
//  ⭐ IDEA: key ko ek node pe rukne mat do. Ring pe clockwise CHALTE RAHO aur
//     pehle N ALAG-ALAG physical nodes pe uski copy rakh do.
//
//     N = replication factor (RF). RF=3 ka matlab: 3 copies, 3 alag servers pe.
//
//     Is list ko Dynamo/Cassandra ki bhasha me "PREFERENCE LIST" kehte hain:
//        preference_list(key) = [primary, replica1, replica2, ...]
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠ SABSE BADI GALTI (aur interview ka favourite trap)                     │
//  │                                                                          │
//  │  "Clockwise agle 3 POINTS le lo" — ye GALAT hai. Kyunki vnodes ke baad    │
//  │  ring pe agle 3 points aaram se EK HI physical node ke ho sakte hain      │
//  │  (NodeA#7, NodeA#88, NodeA#3 — teeno NodeA!). Tab aapki "3 copies" asal   │
//  │  me ek hi server pe hain — wo server gaya, teeno copies gayi. RF ka poora │
//  │  matlab hi khatam.                                                       │
//  │                                                                          │
//  │  ✅ SAHI: chalte raho jab tak N alag PHYSICAL nodes na mil jaayein.       │
//  │     Isi liye neeche `seen` set hai — duplicate physical node skip.        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Real duniya: Cassandra/DynamoDB me RF=3 default. Aur "rack aware" versions
//     me to ye bhi dekha jaata hai ki teeno copies alag rack/AZ me hon — taaki
//     poora rack gir jaaye tab bhi data zinda rahe. (Concept wahi, filter aur sakht.)
//
//  Compile: g++ -std=c++17 -Wall -Wextra 04_replication.cpp -o replication_demo
// ============================================================================
#include <bits/stdc++.h>

#include "hash_util.h"

using namespace std;
using namespace consistent_hashing;

// ============================================================================
//  ReplicatedHashRing — vnodes + replication
// ============================================================================
class ReplicatedHashRing
{
public:
    ReplicatedHashRing(int virtualNodesPerNode, int replicationFactor)
        : vnodesPerNode_(virtualNodesPerNode), replicationFactor_(replicationFactor) {}

    void addNode(const string &nodeId)
    {
        physicalNodes_.insert(nodeId);
        for (int i = 0; i < vnodesPerNode_; ++i)
        {
            ring_[hashKey(nodeId + "#" + to_string(i))] = nodeId;
        }
    }

    void removeNode(const string &nodeId)
    {
        physicalNodes_.erase(nodeId);
        for (int i = 0; i < vnodesPerNode_; ++i)
        {
            ring_.erase(hashKey(nodeId + "#" + to_string(i)));
        }
    }

    // ---- ⭐ PREFERENCE LIST — key ki saari copies kahan rahengi ------------
    // Return: [primary, replica1, replica2, ...] — sab ALAG physical nodes.
    vector<string> getNodes(const string &key) const
    {
        vector<string> result;
        if (ring_.empty())
        {
            return result;
        }

        // Kitni copies chahiye? RF, par cluster me utne nodes hi na hon to
        // jitne hain utne (warna neeche wala loop kabhi khatam hi nahi hoga).
        size_t wanted = min(static_cast<size_t>(replicationFactor_), physicalNodes_.size());

        set<string> seen;                          // duplicate physical node roko
        auto it = ring_.lower_bound(hashKey(key)); // primary ka point

        // Ring pe clockwise chalte raho jab tak `wanted` alag nodes na mil jaayein.
        // Ye loop `ring_.size()` baar se zyada nahi ghoomta — kyunki utne me to
        // saare physical nodes mil hi chuke honge.
        for (size_t steps = 0; steps < ring_.size() && result.size() < wanted; ++steps)
        {
            if (it == ring_.end())
            {
                it = ring_.begin(); // wrap around — ring hai, line nahi
            }

            // `insert` ka `.second` batata hai ki naya tha ya pehle se maujood.
            // Naya => ye ek NAYA physical node hai => list me le lo.
            if (seen.insert(it->second).second)
            {
                result.push_back(it->second);
            }
            ++it;
        }
        return result;
    }

    // Sirf primary chahiye (ek copy wala purana behavior).
    string getPrimary(const string &key) const
    {
        vector<string> nodes = getNodes(key);
        return nodes.empty() ? "" : nodes[0];
    }

    bool hasNode(const string &nodeId) const
    {
        return physicalNodes_.count(nodeId) > 0;
    }

private:
    int vnodesPerNode_;
    int replicationFactor_;
    map<uint32_t, string> ring_;
    set<string> physicalNodes_; // asli servers (vnodes nahi)
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
    const int VNODES = 150;
    const int RF = 3;

    vector<string> keys = makeKeys(TOTAL_KEYS);
    vector<string> nodes = {"NodeA", "NodeB", "NodeC", "NodeD", "NodeE"};

    ReplicatedHashRing ring(VNODES, RF);
    for (const string &node : nodes)
    {
        ring.addNode(node);
    }

    cout << "=========================================================\n";
    cout << " STEP 4 : REPLICATION (preference list)\n";
    cout << " " << nodes.size() << " nodes | vnodes=" << VNODES << " | RF=" << RF << "\n";
    cout << "=========================================================\n\n";

    // ---- Part A: kuch keys ki preference list ----------------------------
    cout << "[A] Har key ki " << RF << " copies (clockwise agle " << RF
         << " ALAG physical nodes):\n\n";
    for (int i = 0; i < 6; ++i)
    {
        string key = "user_" + to_string(i);
        vector<string> replicas = ring.getNodes(key);
        cout << "      " << setw(8) << key << " -> [ ";
        for (size_t r = 0; r < replicas.size(); ++r)
        {
            cout << replicas[r] << (r == 0 ? "(primary)" : "(replica)");
            if (r + 1 < replicas.size())
                cout << ", ";
        }
        cout << " ]\n";
    }

    // ---- Part B: verify — teeno copies sach me ALAG nodes pe hain? --------
    cout << "\n[B] Sanity check (wo GALTI wali baat verify kar rahe hain):\n";
    int badKeys = 0, shortKeys = 0;
    for (const string &key : keys)
    {
        vector<string> replicas = ring.getNodes(key);
        set<string> unique(replicas.begin(), replicas.end());
        if (unique.size() != replicas.size())
            ++badKeys; // duplicate node
        if (replicas.size() != static_cast<size_t>(RF))
            ++shortKeys; // kam copies
    }
    cout << "      Keys jinki 2 copies EK hi node pe : " << badKeys
         << "   <-- 0 hona chahiye ✅\n";
    cout << "      Keys jinko poori " << RF << " copies nahi mili  : " << shortKeys
         << "   <-- 0 hona chahiye ✅\n";

    // ---- Part C: har node pe kitna data (primary + replica) --------------
    cout << "\n[C] Storage ka asli kharcha — har node kitni keys rakhta hai:\n\n";
    map<string, int> primaryCount, totalCount;
    for (const string &key : keys)
    {
        vector<string> replicas = ring.getNodes(key);
        primaryCount[replicas[0]]++;
        for (const string &node : replicas)
        {
            totalCount[node]++;
        }
    }
    cout << "      node   | primary keys | total (primary+replica)\n";
    cout << "      -------+--------------+------------------------\n";
    for (const string &node : nodes)
    {
        cout << "      " << node << "  | " << setw(12) << primaryCount[node] << " | "
             << setw(10) << totalCount[node] << "\n";
    }
    cout << "\n      => Har node ~" << (100 / nodes.size()) << "% primary rakhta hai,\n"
         << "         par total data = " << RF << "x (kyunki har key ki " << RF
         << " copies hain).\n"
         << "      💰 RF=" << RF << " ka matlab " << RF << " guna storage. Free nahi hai.\n";

    // ---- Part D: ⭐ ASLI FAAYDA — ek node mara, data phir bhi zinda -------
    cout << "\n[D] Ab NodeB ko maaro — kya koi data GAYAB hua?\n\n";

    // Failure se pehle har key ki list yaad kar lo.
    unordered_map<string, vector<string>> beforeFail;
    for (const string &key : keys)
    {
        beforeFail[key] = ring.getNodes(key);
    }

    ring.removeNode("NodeB");

    int lostKeys = 0, servedByReplica = 0, unaffected = 0;
    for (const string &key : keys)
    {
        const vector<string> &old = beforeFail[key];

        // Kya is key ki ek bhi copy kisi ZINDA node pe hai?
        bool aliveCopy = false;
        for (const string &node : old)
        {
            if (ring.hasNode(node))
            {
                aliveCopy = true;
                break;
            }
        }
        if (!aliveCopy)
        {
            ++lostKeys; // saari copies mar gayi — data gaya
        }
        else if (old[0] == "NodeB")
        {
            ++servedByReplica; // primary gaya, replica ne sambhal liya
        }
        else
        {
            ++unaffected; // primary zinda tha, kuch hua hi nahi
        }
    }

    cout << "      Keys bilkul unaffected (primary zinda)   : " << unaffected << "\n";
    cout << "      Keys jinka primary gaya, REPLICA ne serve : " << servedByReplica
         << "  <-- yahi RF ka faayda ⭐\n";
    cout << "      Keys jo sach me KHO gayi                  : " << lostKeys
         << "  <-- 0 hona chahiye ✅\n";

    cout << "\n      ⭐ RF=" << RF << " ke saath " << (RF - 1)
         << " nodes ek saath marein tab bhi data zinda rahega.\n";

    // ---- Part E: failure ke baad naya replica kahan bana ------------------
    cout << "\n[E] NodeB ke jaane ke baad list apne aap heal ho gayi:\n\n";
    for (int i = 0; i < 4; ++i)
    {
        string key = "user_" + to_string(i);
        const vector<string> &old = beforeFail[key];
        vector<string> now = ring.getNodes(key);

        bool hadB = find(old.begin(), old.end(), "NodeB") != old.end();
        cout << "      " << setw(8) << key << " : ";
        for (size_t r = 0; r < old.size(); ++r)
            cout << old[r] << (r + 1 < old.size() ? "," : "");
        cout << "  ->  ";
        for (size_t r = 0; r < now.size(); ++r)
            cout << now[r] << (r + 1 < now.size() ? "," : "");
        cout << (hadB ? "   (NodeB ki jagah naya replica ⭐)" : "   (koi badlav nahi)") << "\n";
    }
    cout << "\n      Note: list to turant sahi ho gayi, par us naye replica pe data\n"
         << "      abhi PHYSICALLY copy nahi hua. Wo background repair/hinted-handoff\n"
         << "      karta hai (Cassandra me isi ko 'read repair' kehte hain).\n";

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Preference list = clockwise agle RF ALAG PHYSICAL nodes.\n";
    cout << "  ⚠ Agle RF 'points' lena GALAT — vnodes ki wajah se wo ek hi node ho sakte.\n";
    cout << "  ✅ RF-1 nodes marne tak data safe.\n";
    cout << "  ✅ Node marte hi list khud heal ho jaati hai (agla node replica ban jaata).\n";
    cout << "  💰 Cost: RF guna storage + writes RF jagah jaani chahiye.\n";
    cout << "  📌 Yahi Dynamo/Cassandra ka model hai (RF=3 default).\n";
    cout << "\n  Ab ek aur asli duniya ka dard: HOT KEYS. Distribution even hai,\n";
    cout << "  par traffic even nahi (ek celebrity key sab kuch kha jaati hai).\n";
    cout << "  Uska ilaaj -> Step 5 (BOUNDED LOADS).\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}

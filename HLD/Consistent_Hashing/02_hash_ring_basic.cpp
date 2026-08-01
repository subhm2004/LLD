// ============================================================================
//  STEP 2 — 02_hash_ring_basic.cpp
//  "hash % N" ko fenko — ab HASH RING banate hain
// ----------------------------------------------------------------------------
//  Step 1 ka jadd kya tha? Node ka pata `N` pe depend karta tha. N badla, sab badla.
//
//  ⭐ CORE IDEA (poora consistent hashing isi ek line me hai):
//     Key ko sidha node se mat jodo. Dono ko ek hi space (0 .. 2^32-1) me
//     rakho, aur key ko us node ko de do jo usse CLOCKWISE me sabse pehle mile.
//
//     hash(key)  -> ring pe ek point
//     hash(node) -> ring pe ek point
//     key ka owner = clockwise agla node
//
//  Ab node ki position `hash(node_id)` se aati hai — `N` se uska koi lena-dena
//  hi nahi. Isliye ek node aane-jaane se baaki nodes ki position hilti hi nahi,
//  aur sirf uske aas-paas ki keys ka owner badalta hai. Yahi "consistent" ka matlab.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ DATA STRUCTURE: `std::map<uint32_t, string>` (Red-Black tree)         │
//  │                                                                          │
//  │  Ring = SORTED positions. "Clockwise agla node" = "is position se bada    │
//  │  ya barabar pehla element" = `lower_bound()` — O(log n).                  │
//  │                                                                          │
//  │  Aur agar `lower_bound` end pe pahunch gaya (matlab ring ke aakhri node   │
//  │  se bhi aage nikal gaye), to circle ki tarah GHOOM ke `begin()` pe aao.   │
//  │  Bas yahi ek line ring ko "circle" banati hai — warna wo seedhi line hai. │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ Is step me JAAN-BUJH KE virtual nodes NAHI daale — taaki aap khud dekh sako
//     ki bina unke distribution kitna kharaab (uneven) hota hai. Wahi Step 3 ki bhumika.
//
//  Compile: g++ -std=c++17 -Wall -Wextra 02_hash_ring_basic.cpp -o ring_demo
// ============================================================================
#include <bits/stdc++.h>

#include "hash_util.h"

using namespace std;
using namespace consistent_hashing;

// ============================================================================
//  BasicHashRing — sabse simple consistent hash ring (1 node = 1 ring point)
// ============================================================================
class BasicHashRing
{
public:
    // ---- Node ko ring pe rakho -------------------------------------------
    // Position `hash(nodeId)` se aati hai. Note: ismein kahin `N` nahi hai —
    // isliye baaki nodes ki position pe koi asar nahi padta.
    void addNode(const string &nodeId)
    {
        ring_[hashKey(nodeId)] = nodeId;
    }

    // ---- Node ko ring se hatao -------------------------------------------
    // Uska point gayab. Uski keys apne-aap clockwise agle node ki ho jaati hain
    // — koi "rebalancing loop" likhna hi nahi padta, lookup khud sambhaal leta hai.
    void removeNode(const string &nodeId)
    {
        ring_.erase(hashKey(nodeId));
    }

    // ---- ⭐ DIL KA TUKDA: key -> node lookup ------------------------------
    string getNode(const string &key) const
    {
        if (ring_.empty())
        {
            return ""; // koi node hi nahi hai
        }

        uint32_t position = hashKey(key);

        // "is position se bada-ya-barabar pehla node" = clockwise agla node.
        auto it = ring_.lower_bound(position);

        // Ring ke aakhri node se bhi aage nikal gaye? To WRAP AROUND —
        // 2^32 ke baad wapas 0 aata hai, yaani sabse pehla node.
        if (it == ring_.end())
        {
            it = ring_.begin();
        }

        return it->second;
    }

    // ---- Ring pe abhi kaun-kaun, kis position pe (debug ke liye) ----------
    void printRing() const
    {
        cout << "    Ring (position % of 2^32 -> node):\n";
        for (const auto &entry : ring_)
        {
            cout << "      " << fixed << setprecision(2) << setw(6)
                 << ringPercent(entry.first) << "%  ->  " << entry.second << "\n";
        }
    }

    size_t size() const { return ring_.size(); }

private:
    // position -> nodeId. `map` sorted rehta hai, isiliye ye "ring" ban paata hai.
    map<uint32_t, string> ring_;
};

// ---- Test keys ------------------------------------------------------------
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

// ---- Har key ka abhi ka owner (snapshot) ----------------------------------
// Isse pehle-baad compare karke naapenge ki kitni keys sach me move hui.
unordered_map<string, string> snapshotOwners(const BasicHashRing &ring,
                                             const vector<string> &keys)
{
    unordered_map<string, string> owners;
    owners.reserve(keys.size());
    for (const string &key : keys)
    {
        owners[key] = ring.getNode(key);
    }
    return owners;
}

// ---- Kitni keys ne malik badla? -------------------------------------------
double comparePercentMoved(const unordered_map<string, string> &before,
                           const unordered_map<string, string> &after)
{
    int moved = 0;
    for (const auto &entry : before)
    {
        if (entry.second != after.at(entry.first))
        {
            ++moved;
        }
    }
    return (100.0 * moved) / before.size();
}

// ---- Load distribution print ----------------------------------------------
void printLoad(const unordered_map<string, string> &owners, size_t totalKeys)
{
    map<string, int> countPerNode; // map isliye ki output sorted dikhe
    for (const auto &entry : owners)
    {
        countPerNode[entry.second]++;
    }
    for (const auto &entry : countPerNode)
    {
        double percent = (100.0 * entry.second) / totalKeys;
        cout << "      " << entry.first << " : " << setw(6) << entry.second
             << " keys (" << fixed << setprecision(1) << setw(5) << percent << "%)\n";
    }
}

int main()
{
    const int TOTAL_KEYS = 100000;
    vector<string> keys = makeKeys(TOTAL_KEYS);

    cout << "=========================================================\n";
    cout << " STEP 2 : CONSISTENT HASHING — HASH RING\n";
    cout << " Total keys: " << TOTAL_KEYS << "  |  virtual nodes: abhi nahi\n";
    cout << "=========================================================\n\n";

    // ---- Part A: ring banao ------------------------------------------------
    BasicHashRing ring;
    ring.addNode("NodeA");
    ring.addNode("NodeB");
    ring.addNode("NodeC");
    ring.addNode("NodeD");

    cout << "[A] 4 nodes ring pe (position = hash(node_id)):\n";
    ring.printRing();
    cout << "\n    Dhyan do — positions BIKHRI hui hain, barabar-barabar nahi.\n"
         << "    Kyunki wo hash se aayi hain, kisi ne set nahi ki.\n\n";

    // ---- Part B: lookup kaise chalta hai ----------------------------------
    cout << "[B] Lookup: key ki position se CLOCKWISE agla node:\n";
    for (int i = 0; i < 5; ++i)
    {
        string key = "user_" + to_string(i);
        cout << "      " << setw(8) << key << " @ " << fixed << setprecision(2)
             << setw(6) << ringPercent(hashKey(key)) << "%  ->  " << ring.getNode(key) << "\n";
    }

    unordered_map<string, string> before = snapshotOwners(ring, keys);
    cout << "\n    Load abhi (4 nodes):\n";
    printLoad(before, TOTAL_KEYS);
    cout << "\n    ⚠ Dekha? 25-25-25-25 nahi hai. Ye TEDHAPAN Step 3 me theek karenge.\n\n";

    // ---- Part C: ⭐ ASLI TEST — node add karo -----------------------------
    cout << "[C] Ab NodeE ADD karo — aur wahi cheez naapo jo Step 1 me 80% thi:\n\n";
    ring.addNode("NodeE");
    unordered_map<string, string> afterAdd = snapshotOwners(ring, keys);

    double movedOnAdd = comparePercentMoved(before, afterAdd);
    cout << "      Keys moved : " << fixed << setprecision(1) << movedOnAdd << "%\n";
    cout << "      (Step 1 me yahi 79.8% tha — bahut kam ho gaya)\n\n";
    cout << "      ⚠ Par ye theory wala 1/N (=20%) bhi nahi hai. Kyun?\n"
         << "        Kyunki ring TEDHI hai — NodeE theek us node ke arc me gira\n"
         << "        jiske paas sabse zyada keys thi. Vnodes (Step 3) ke baad ye\n"
         << "        number sach me 1/N ke paas aa jaayega.\n\n";

    // Kaun si keys move hui? Sirf NodeE ke arc me aane wali — ek hi node se.
    map<string, int> movedFrom;
    for (const auto &entry : before)
    {
        const string &newOwner = afterAdd.at(entry.first);
        if (entry.second != newOwner)
        {
            movedFrom[entry.second]++;
        }
    }
    cout << "      Aur ye keys kis-kis node se gayi:\n";
    for (const auto &entry : movedFrom)
    {
        cout << "        " << entry.first << " se " << entry.second << " keys -> NodeE\n";
    }
    cout << "      => Sirf UNKO chhua jo NodeE ke theek peeche wale arc me thi.\n"
         << "         Baaki har node ki har key jaha thi wahin hai. ⭐\n\n";

    // ---- Part D: node remove ---------------------------------------------
    cout << "[D] Ab NodeB REMOVE karo (jaise koi server crash ho gaya):\n\n";
    ring.removeNode("NodeB");
    unordered_map<string, string> afterRemove = snapshotOwners(ring, keys);

    int reassigned = 0, disturbed = 0;
    for (const auto &entry : afterAdd)
    {
        if (entry.second == "NodeB")
        {
            ++reassigned; // ye to move honi hi thi (unka ghar hi chala gaya)
        }
        else if (entry.second != afterRemove.at(entry.first))
        {
            ++disturbed; // ye BILKUL nahi hilni chahiye
        }
    }
    cout << "      NodeB ki keys (majburan move hui) : " << reassigned << "\n";
    cout << "      Baaki nodes ki keys jo hili        : " << disturbed
         << "   <-- ZERO hona chahiye ✅\n";
    cout << "\n      NodeB ki saari keys ek hi node ko mili:\n";
    map<string, int> inherited;
    for (const auto &entry : afterAdd)
    {
        if (entry.second == "NodeB")
        {
            inherited[afterRemove.at(entry.first)]++;
        }
    }
    for (const auto &entry : inherited)
    {
        cout << "        " << entry.first << " ko " << entry.second << " keys mili\n";
    }
    cout << "      ⚠ Ek hi node ne poora bojh utha liya — ye bhi problem hai.\n"
         << "         Ise bhi Step 3 (virtual nodes) hi theek karta hai.\n";

    // ---- Nateeja ----------------------------------------------------------
    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Node add/remove pe sirf ~" << fixed << setprecision(0) << movedOnAdd
         << "% keys move (Step 1 me 80% thi). Yahi jeet hai.\n";
    cout << "  ✅ Baaki nodes ko haath tak nahi laga (disturbed = " << disturbed << ").\n";
    cout << "  ✅ Lookup O(log n) — map ka lower_bound + wrap-around.\n";
    cout << "  ❌ Par load bahut TEDHA hai (upar dekho: kisi ko 68%, kisi ko 1%).\n";
    cout << "  ❌ Aur node marne pe uska POORA bojh ek hi padosi pe girta hai.\n";
    cout << "\n  Dono kamiyon ka ek hi ilaaj: VIRTUAL NODES -> Step 3.\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}

// ============================================================================
//  ConsistentHashRing.h  —  CAPSTONE: sab kuch ek reusable class me
// ----------------------------------------------------------------------------
//  Steps 1-6 me humne ek-ek concept alag-alag file me tod kar samjha. Ab unhe
//  ek jagah, saaf-suthri class me jodte hain — jo aap sach me kisi project me
//  utha ke use kar sakte ho.
//
//  Ismein kya-kya hai:
//     ✅ Virtual nodes           (Step 3) — even distribution
//     ✅ Replication / pref list (Step 4) — distinct physical nodes
//     ✅ Weighted nodes          (naya)   — bade server ko zyada hissa
//     ✅ Thread-safe reads       (naya)   — shared_mutex se
//     ✅ Distribution stats                — tuning ke liye
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ WEIGHTED NODES — asli cluster me sab servers barabar nahi hote        │
//  │                                                                          │
//  │  Ek 64GB ka server aur ek 16GB ka — dono ko barabar keys dena galat hai.  │
//  │  Ring pe iska ilaaj bahut seedha hai: bade server ke ZYADA vnodes daal do.│
//  │      vnodes = baseVnodes × weight                                        │
//  │  weight=4 wale node ke ring pe 4 guna points -> 4 guna arcs -> 4 guna     │
//  │  keys. Bas. Lookup ka code isse bilkul anjaan rehta hai.                 │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ THREAD SAFETY — `shared_mutex` kyun, normal `mutex` kyun nahi         │
//  │                                                                          │
//  │  Is ring pe kaam ka batwara aisa hai:                                     │
//  │     * getNode() — har request pe chalta hai (LAKHON baar)                 │
//  │     * addNode() — mahine me kabhi-kabhi (server add/remove)               │
//  │                                                                          │
//  │  Yaani reads bahut zyada, writes na ke barabar. Normal `mutex` lagate to  │
//  │  har read doosre reads ko bhi rokta — bekaar me. `shared_mutex` me:       │
//  │     shared_lock  (read)  -> kai threads EK SAATH padh sakte hain          │
//  │     unique_lock  (write) -> sirf ek, aur tab koi reader nahi              │
//  │  Ye classic "reader-writer lock" hai, aur yahan bilkul fit baithta hai.   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Ye ring sirf ROUTING batata hai ("ye key kis server pe rehni chahiye").
//     Data khud store karna, network call karna — wo upar wali layer ka kaam hai
//     (main.cpp me uska ek simulation hai).
// ============================================================================
#ifndef CONSISTENT_HASHING_CONSISTENTHASHRING_H
#define CONSISTENT_HASHING_CONSISTENTHASHRING_H

#include <algorithm>
#include <map>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <vector>

#include "hash_util.h"

using namespace std;

namespace consistent_hashing
{

class ConsistentHashRing
{
public:
    // baseVirtualNodes: har node ke kitne ring points (Step 3 wala sweet spot 100-256)
    // replicationFactor: har key ki kitni copies (Step 4)
    explicit ConsistentHashRing(int baseVirtualNodes = 150, int replicationFactor = 3)
        : baseVirtualNodes_(baseVirtualNodes), replicationFactor_(replicationFactor)
    {
        if (baseVirtualNodes <= 0)
        {
            throw invalid_argument("baseVirtualNodes 0 se bada hona chahiye");
        }
        if (replicationFactor <= 0)
        {
            throw invalid_argument("replicationFactor 0 se bada hona chahiye");
        }
    }

    // ---- Node add (weight = uski capacity ka multiplier) -----------------
    // weight=2 matlab is node ko doosron se dugni keys milengi.
    void addNode(const string &nodeId, int weight = 1)
    {
        if (weight <= 0)
        {
            throw invalid_argument("weight 0 se bada hona chahiye");
        }

        unique_lock<shared_mutex> lock(mutex_); // WRITE — poora lock

        if (nodeWeights_.count(nodeId))
        {
            return; // pehle se hai, dobara mat daalo
        }
        nodeWeights_[nodeId] = weight;

        // ⭐ Weight ka poora implementation bas yahi hai — utne guna vnodes.
        int points = baseVirtualNodes_ * weight;
        for (int i = 0; i < points; ++i)
        {
            ring_[hashKey(virtualNodeName(nodeId, i))] = nodeId;
        }
    }

    // ---- Node remove ------------------------------------------------------
    void removeNode(const string &nodeId)
    {
        unique_lock<shared_mutex> lock(mutex_); // WRITE

        auto it = nodeWeights_.find(nodeId);
        if (it == nodeWeights_.end())
        {
            return; // hai hi nahi
        }

        // Jitne daale the utne hi hatao — isliye weight yaad rakhna zaroori tha.
        int points = baseVirtualNodes_ * it->second;
        for (int i = 0; i < points; ++i)
        {
            ring_.erase(hashKey(virtualNodeName(nodeId, i)));
        }
        nodeWeights_.erase(it);
    }

    // ---- Primary node (sabse common call) --------------------------------
    string getNode(const string &key) const
    {
        shared_lock<shared_mutex> lock(mutex_); // READ — dusre readers bhi chal sakte
        if (ring_.empty())
        {
            return "";
        }
        return locateClockwise(hashKey(key))->second;
    }

    // ---- Preference list: primary + replicas (Step 4 wala logic) ---------
    vector<string> getNodes(const string &key) const
    {
        shared_lock<shared_mutex> lock(mutex_); // READ

        vector<string> result;
        if (ring_.empty())
        {
            return result;
        }

        // Cluster me nodes hi kam hain to jitne hain utni copies.
        size_t wanted = min(static_cast<size_t>(replicationFactor_), nodeWeights_.size());

        set<string> seen; // ⭐ DISTINCT PHYSICAL nodes — Step 4 wali galti se bachne ke liye
        auto it = locateClockwise(hashKey(key));

        for (size_t steps = 0; steps < ring_.size() && result.size() < wanted; ++steps)
        {
            if (it == ring_.end())
            {
                it = ring_.begin(); // wrap around
            }
            if (seen.insert(it->second).second)
            {
                result.push_back(it->second);
            }
            ++it;
        }
        return result;
    }

    // ---- Introspection ----------------------------------------------------
    bool hasNode(const string &nodeId) const
    {
        shared_lock<shared_mutex> lock(mutex_);
        return nodeWeights_.count(nodeId) > 0;
    }

    size_t nodeCount() const
    {
        shared_lock<shared_mutex> lock(mutex_);
        return nodeWeights_.size();
    }

    size_t ringPoints() const
    {
        shared_lock<shared_mutex> lock(mutex_);
        return ring_.size();
    }

    vector<string> nodes() const
    {
        shared_lock<shared_mutex> lock(mutex_);
        vector<string> result;
        for (const auto &entry : nodeWeights_)
        {
            result.push_back(entry.first);
        }
        return result;
    }

    int replicationFactor() const { return replicationFactor_; }

    // ---- Har node ko ring ka kitna % hissa mila --------------------------
    // Ye ACTUAL arc measurement hai (keys sample kiye bina) — vnode count tune
    // karne ke liye sabse seedha tareeka.
    map<string, double> ownershipPercent() const
    {
        shared_lock<shared_mutex> lock(mutex_);

        map<string, double> share;
        for (const auto &entry : nodeWeights_)
        {
            share[entry.first] = 0.0;
        }
        if (ring_.empty())
        {
            return share;
        }

        // Har ring point us arc ka maalik hai jo PICHHLE point ke baad shuru hota.
        uint32_t previous = ring_.rbegin()->first; // wrap: aakhri point se shuru
        for (const auto &entry : ring_)
        {
            // Ghatana modular hai (2^32 pe wrap) — unsigned arithmetic khud
            // ye sambhaal leti hai, isliye koi extra `if` nahi chahiye.
            uint32_t arc = entry.first - previous;
            share[entry.second] += (100.0 * arc) / 4294967296.0;
            previous = entry.first;
        }
        return share;
    }

private:
    // Lock pehle se liya hua hona chahiye — ye internal helper hai.
    map<uint32_t, string>::const_iterator locateClockwise(uint32_t position) const
    {
        auto it = ring_.lower_bound(position);
        return (it == ring_.end()) ? ring_.begin() : it;
    }

    static string virtualNodeName(const string &nodeId, int index)
    {
        return nodeId + "#" + to_string(index);
    }

    int baseVirtualNodes_;
    int replicationFactor_;
    map<uint32_t, string> ring_;   // ring position -> physical node
    map<string, int> nodeWeights_; // physical node -> weight
    mutable shared_mutex mutex_;   // `mutable` — const reads bhi lock lete hain
};

} // namespace consistent_hashing

#endif // CONSISTENT_HASHING_CONSISTENTHASHRING_H

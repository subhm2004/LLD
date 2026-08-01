// ============================================================================
//  models/Node.h  —  Ek PHYSICAL node (asli server jispe data rakha jaata hai)
// ----------------------------------------------------------------------------
//  Har node ke paas: unique id, host address, health status, aur ek WEIGHT.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `weight_` — asli cluster me sab servers barabar nahi hote             │
//  │                                                                          │
//  │  Ek 64GB RAM ka server aur ek 16GB ka — dono ko barabar keys dena galat   │
//  │  hai. Ring pe iska ilaaj bahut seedha hai: bade server ke ZYADA virtual   │
//  │  nodes daal do.                                                          │
//  │                                                                          │
//  │        virtualNodes = baseVirtualNodes × weight                          │
//  │                                                                          │
//  │  weight=3 wale node ke ring pe 3 guna points -> 3 guna arcs -> 3 guna     │
//  │  keys. Aur mazedaar baat: lookup ka code isse BILKUL anjaan rehta hai.    │
//  │  Wo bas "clockwise agla point" dhundhta hai — kis node ke kitne points    │
//  │  hain, usse koi matlab nahi.                                             │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `isAvailable()` = status ACTIVE hai kya. Ring isi ek helper se health
//     check karta hai — "ACTIVE hai to hi key do" ka logic ek jagah (DRY).
//
//  📌 Ye ek "rich model" hai — sirf data nahi, thoda behavior (isAvailable,
//     virtualNodeCount) bhi rakhta hai. Ye behavior node ki apni state
//     (weight, status) se juda hai, isliye yahin rehna sahi hai (encapsulation).
//
//  ⚠ Dhyan do: Node ko ring ki ya virtual nodes ki koi khabar NAHI hai. Wo bas
//     "ek server" hai. Ring uska use karta hai, ulta nahi — ye one-way
//     dependency design ko saaf rakhti hai.
// ============================================================================
#ifndef CONSISTENT_HASHING_LLD_MODELS_NODE_H
#define CONSISTENT_HASHING_LLD_MODELS_NODE_H

#include <stdexcept>
#include <string>
#include <utility>

#include "../enums/NodeStatus.h"

using namespace std;

namespace consistent_hashing_lld {

class Node {
public:
    // Naya node hamesha ACTIVE se shuru hota hai.
    Node(string nodeId, string host, int weight)
        : nodeId_(std::move(nodeId)),
          host_(std::move(host)),
          weight_(weight),
          status_(NodeStatus::ACTIVE) {
        if (weight_ <= 0) { // fail-fast: weight 0 matlab ring pe zero points
            throw invalid_argument("Node weight must be greater than 0");
        }
    }

    // ---- Getters ------------------------------------------------------------
    const string &getId() const { return nodeId_; }
    const string &getHost() const { return host_; }
    int getWeight() const { return weight_; }
    NodeStatus getStatus() const { return status_; }

    // ---- Health ------------------------------------------------------------
    bool isAvailable() const { return status_ == NodeStatus::ACTIVE; } // key lene layak?
    void setStatus(NodeStatus status) { status_ = status; }

    // ---- Is node ke ring pe kitne points banenge ---------------------------
    // ⭐ Weight ka poora implementation bas yahi ek line hai.
    int virtualNodeCount(int baseVirtualNodes) const { return baseVirtualNodes * weight_; }

private:
    string nodeId_;     // "cache-1" — unique
    string host_;       // "10.0.0.1:11211"
    int weight_;        // capacity multiplier (1 = normal, 3 = teen guna bada)
    NodeStatus status_; // ACTIVE / DOWN
};

} // namespace consistent_hashing_lld

#endif // CONSISTENT_HASHING_LLD_MODELS_NODE_H

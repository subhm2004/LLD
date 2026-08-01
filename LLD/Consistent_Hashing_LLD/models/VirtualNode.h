// ============================================================================
//  models/VirtualNode.h  —  Ring pe ek POINT (physical node ka "clone")
// ----------------------------------------------------------------------------
//  Virtual node koi asli server NAHI hai. Ye bas ring pe ek entry hai jo kisi
//  physical node ko point karti hai. Ek physical node ke aise 150-256 points
//  hote hain, poore ring pe bikhre hue.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ VIRTUAL NODES KYUN? (LLD ka sabse zaroori "why")                      │
//  │                                                                          │
//  │  Bina vnodes ke har node ring pe SIRF EK point hota. Aur sirf 4-5 random  │
//  │  points daalo to arcs barabar bante hi nahi — kisi node ko ring ka 60%    │
//  │  mil jaata, kisi ko 2%. (HLD notes me ye naap ke dikhaya gaya hai.)       │
//  │                                                                          │
//  │  Vnodes se DO problem ek saath theek hoti hain:                           │
//  │   1️⃣ EVEN LOAD — 5 bade random arcs ki jagah 750 chhote arcs. Jitne       │
//  │      zyada tukde, utna barabar batwara (law of large numbers).           │
//  │   2️⃣ EVEN REBALANCE — node mara to uske 150 alag-alag arcs the, aur har   │
//  │      arc ka padosi ALAG node hai. Isliye uska bojh SAARE bache nodes me   │
//  │      baant jaata hai — kisi ek padosi pe nahi girta (cascading failure    │
//  │      se bachav).                                                         │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `virtualId()` = "cache-1#7" — yahi string hash hoti hai ring position
//     nikalne ke liye. Har replica index ka alag naam -> alag hash -> ring pe
//     alag jagah. Bas isi trick se ek node kai jagah "dikhta" hai.
//
//  ⚠ `physicalNode_` ek NON-OWNING pointer hai. VirtualNode Node ko delete
//     NAHI karta — Node ka maalik ConsistentHashRing hai (`unique_ptr` me).
//     Yahan raw pointer sirf "reference" ke liye hai, ownership ke liye nahi.
//     Isliye Node hamesha VirtualNode se ZYADA jeeta hai (lifetime safe).
// ============================================================================
#ifndef CONSISTENT_HASHING_LLD_MODELS_VIRTUALNODE_H
#define CONSISTENT_HASHING_LLD_MODELS_VIRTUALNODE_H

#include <cstdint>
#include <string>

#include "Node.h"

using namespace std;

namespace consistent_hashing_lld {

class VirtualNode {
public:
    VirtualNode(uint32_t ringPosition, int replicaIndex, Node *physicalNode)
        : ringPosition_(ringPosition), replicaIndex_(replicaIndex), physicalNode_(physicalNode) {}

    uint32_t getRingPosition() const { return ringPosition_; }
    int getReplicaIndex() const { return replicaIndex_; }

    // Non-owning — ring is Node ka maalik hai, ye sirf point karta hai.
    Node *getPhysicalNode() const { return physicalNode_; }

    // "cache-1#7" — ring position isi string ke hash se aayi thi.
    static string virtualId(const string &nodeId, int replicaIndex) {
        return nodeId + "#" + to_string(replicaIndex);
    }

private:
    uint32_t ringPosition_; // 0 .. 2^32-1 — ring pe iski jagah
    int replicaIndex_;      // 0, 1, 2, ... (is node ka kaunsa clone)
    Node *physicalNode_;    // ⚠ NON-OWNING (maalik: ConsistentHashRing)
};

} // namespace consistent_hashing_lld

#endif // CONSISTENT_HASHING_LLD_MODELS_VIRTUALNODE_H

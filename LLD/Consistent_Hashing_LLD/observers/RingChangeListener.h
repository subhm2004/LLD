// ============================================================================
//  observers/RingChangeListener.h  —  OBSERVER PATTERN ka interface
// ----------------------------------------------------------------------------
//  Jab ring ka topology badalta hai (node aaya / gaya / DOWN hua), to bahut se
//  logon ko khabar chahiye hoti hai:
//     * DATA MIGRATION service — "ab kaunsa data kahan bhejna hai"
//     * MONITORING / metrics    — "topology change hua, alert karo"
//     * AUDIT log               — "kisne kab node hataya"
//     * CACHE WARMER            — "naye node ko pre-populate karo"
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ OBSERVER PATTERN — "khabar do, par jaanne ki zaroorat nahi kaun sun   │
//  │     raha hai"                                                            │
//  │                                                                          │
//  │  ConsistentHashRing = SUBJECT (publisher)                                │
//  │  RingChangeListener = OBSERVER interface (subscriber)                    │
//  │                                                                          │
//  │  Ring ko bilkul pata nahi ki kaun sun raha hai ya wo log karenge kya.     │
//  │  Wo bas `notifyNodeAdded(...)` chilla deta hai. Naya listener add karna   │
//  │  ho? Bas nayi class banao — ring ka ek line nahi badlega                  │
//  │  (OPEN/CLOSED PRINCIPLE).                                                │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ OBSERVER YAHAN KYUN ZAROORI HAI (sirf "pattern dikhane" ke liye nahi):
//     Bina iske ring ko khud `DataMigrationService` ko call karna padta. Matlab
//     ring ka migration pe DEPENDENCY ban jaati — ek routing data-structure ko
//     data migration ki koi khabar nahi honi chahiye. Ye SINGLE RESPONSIBILITY
//     ka ulanghan hota. Observer se dono bilkul alag rehte hain.
//
//  ⭐ `= 0` nahi, KHAALI DEFAULT BODY ({}) di hai — taaki listener sirf wahi
//     event override kare jisme uski dilchaspi hai. Migration tracker ko
//     status-change se matlab nahi, to wo use chhod sakta hai. (Ise "adapter"
//     ya optional-hook style kehte hain — pure virtual se zyada practical.)
//
//  ⚠ VIRTUAL DESTRUCTOR: listeners base pointer ke through rakhe jaate hain.
//     Bina virtual destructor ke derived ka cleanup nahi chalega -> UB.
// ============================================================================
#ifndef CONSISTENT_HASHING_LLD_OBSERVERS_RINGCHANGELISTENER_H
#define CONSISTENT_HASHING_LLD_OBSERVERS_RINGCHANGELISTENER_H

#include "../models/Node.h"

namespace consistent_hashing_lld {

class RingChangeListener {
public:
    virtual ~RingChangeListener() = default;

    // Naya node ring me aaya (uske `virtualNodeCount` points add hue).
    virtual void onNodeAdded(const Node &node, int virtualNodeCount) {
        (void)node;
        (void)virtualNodeCount;
    }

    // Node ring se hataya gaya (permanent).
    virtual void onNodeRemoved(const Node &node, int virtualNodeCount) {
        (void)node;
        (void)virtualNodeCount;
    }

    // Node ka health badla (ACTIVE <-> DOWN) — ring pe positions wahi rehti hain.
    virtual void onNodeStatusChanged(const Node &node, NodeStatus oldStatus) {
        (void)node;
        (void)oldStatus;
    }

    // Hash function badla -> POORA ring dobara bana -> lagbhag saari keys hilengi.
    virtual void onRingRebuilt(const string &reason, size_t totalVirtualNodes) {
        (void)reason;
        (void)totalVirtualNodes;
    }
};

} // namespace consistent_hashing_lld

#endif // CONSISTENT_HASHING_LLD_OBSERVERS_RINGCHANGELISTENER_H

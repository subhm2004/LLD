// ============================================================================
//  observers/MigrationTracker.h  —  CONCRETE OBSERVER: data migration ka plan
// ----------------------------------------------------------------------------
//  Ring sirf ye batata hai ki "ab kaunsi key kis node ki hai". Par jab topology
//  badalta hai to koi to hona chahiye jo ASAL DATA ek node se doosre pe bheje.
//  Wo kaam is listener ka hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ YE CLASS OBSERVER PATTERN KA ASLI JUSTIFICATION HAI                   │
//  │                                                                          │
//  │  Sochiye agar ye na hota — to `ConsistentHashRing::addNode()` ke andar    │
//  │  seedha migration ka code likhna padta. Matlab ek ROUTING data-structure  │
//  │  ko data transfer, network, retry, progress tracking sab ki khabar        │
//  │  rakhni padti. Wo class do-teen kaam ek saath karne lagti (Single         │
//  │  Responsibility gaya paani me).                                          │
//  │                                                                          │
//  │  Observer se ring bas "kya hua" bolta hai; "uska kya karna hai" ye class  │
//  │  decide karti hai. Dono alag-alag test bhi kiye ja sakte hain.            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ NODE ADD vs NODE REMOVE — migration ka kaam bilkul ULTA hota hai:
//     * ADD    -> naya node KHAALI hai. Uske hisse ka data padosi nodes se
//                 usme aana chahiye (pull / stream in).
//     * REMOVE -> gaya hua node ab data serve nahi karega. Uski copies ki
//                 REPLICA COUNT kam ho gayi, to bache nodes pe nayi copies
//                 banani padengi (re-replication).
//     * DOWN   -> koi migration NAHI. Node wapas aa sakta hai; uska data wahin
//                 hai. Bas temporarily traffic doosre node pe ja raha hai.
//                 ⚠ Yahi wo galti hai jo log karte hain — har blip pe migration
//                 shuru kar dena. Isi liye neeche DOWN pe hum kuch nahi karte,
//                 sirf note karte hain (real system me "hinted handoff").
//
//  📌 Ye demo hai — yahan hum sirf migration TASKS ki list banate hain. Asli
//     system me ye tasks ek queue me jaate aur background workers unhe chalate
//     (rate-limited, taaki live traffic pe asar na pade).
// ============================================================================
#ifndef CONSISTENT_HASHING_LLD_OBSERVERS_MIGRATIONTRACKER_H
#define CONSISTENT_HASHING_LLD_OBSERVERS_MIGRATIONTRACKER_H

#include <iostream>
#include <string>
#include <vector>

#include "RingChangeListener.h"

using namespace std;

namespace consistent_hashing_lld {

// Ek migration ka kaam (kis node ke liye, kis type ka).
struct MigrationTask {
    string nodeId;
    string action; // "STREAM_IN" | "RE_REPLICATE"
    string reason;
};

class MigrationTracker : public RingChangeListener {
public:
    // Naya node aaya -> use apne hisse ka data padosiyon se lena hai.
    void onNodeAdded(const Node &node, int virtualNodeCount) override {
        tasks_.push_back({node.getId(), "STREAM_IN",
                          "naya node khaali hai; " + to_string(virtualNodeCount) +
                              " vnode ranges ka data padosiyon se aayega"});
    }

    // Node gaya -> uski copies ab kam pad gayi, bache nodes pe nayi banao.
    void onNodeRemoved(const Node &node, int virtualNodeCount) override {
        tasks_.push_back({node.getId(), "RE_REPLICATE",
                          "node gaya; uski " + to_string(virtualNodeCount) +
                              " vnode ranges ki replica count bhar ni hai"});
    }

    // ⭐ DOWN pe JAAN-BUJH KE koi task nahi banate — upar wala note dekho.
    void onNodeStatusChanged(const Node &node, NodeStatus oldStatus) override {
        (void)oldStatus;
        if (!node.isAvailable()) {
            ++transientDownCount_;
        }
    }

    void onRingRebuilt(const string &reason, size_t totalVirtualNodes) override {
        (void)totalVirtualNodes;
        tasks_.push_back({"<all-nodes>", "FULL_RESHUFFLE", reason});
    }

    // ---- Report ------------------------------------------------------------
    void printPendingTasks() const {
        cout << "    Pending migration tasks: " << tasks_.size() << "\n";
        for (const MigrationTask &task : tasks_) {
            cout << "      - [" << task.action << "] " << task.nodeId << " : " << task.reason << "\n";
        }
        cout << "    DOWN events jinpe koi migration NAHI hui: " << transientDownCount_
             << "  ⭐ (temporary failure pe data hilana bekaar hai)\n";
    }

    size_t taskCount() const { return tasks_.size(); }
    void clear() { tasks_.clear(); }

private:
    vector<MigrationTask> tasks_;
    int transientDownCount_ = 0;
};

} // namespace consistent_hashing_lld

#endif // CONSISTENT_HASHING_LLD_OBSERVERS_MIGRATIONTRACKER_H

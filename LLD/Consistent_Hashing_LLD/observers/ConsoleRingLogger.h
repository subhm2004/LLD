// ============================================================================
//  observers/ConsoleRingLogger.h  —  CONCRETE OBSERVER: sab kuch console pe
// ----------------------------------------------------------------------------
//  Sabse seedha listener — jo bhi topology change hua, use print kar do.
//  Real system me yahi kaam structured logging (JSON logs) ya metrics system
//  (Prometheus counter) karta hai.
//
//  ⭐ Dhyan do ki ye class ring ke baare me KUCH NAHI jaanti — na ring ka
//     pointer rakhti hai, na usse kuch poochti hai. Bas events aate hain aur
//     ye print kar deti hai. Yahi Observer ka loose coupling hai.
//
//  📌 Ye "audit trail" ka bhi kaam karta hai — production me sabse pehle yahi
//     dekha jaata hai jab koi pooche "raat 2 baje load kyun shift hua tha?"
// ============================================================================
#ifndef CONSISTENT_HASHING_LLD_OBSERVERS_CONSOLERINGLOGGER_H
#define CONSISTENT_HASHING_LLD_OBSERVERS_CONSOLERINGLOGGER_H

#include <iostream>
#include <string>

#include "RingChangeListener.h"

using namespace std;

namespace consistent_hashing_lld {

class ConsoleRingLogger : public RingChangeListener {
public:
    void onNodeAdded(const Node &node, int virtualNodeCount) override {
        cout << "    [RING-LOG] JOINED  : " << node.getId() << " (" << node.getHost()
             << ", weight=" << node.getWeight() << ") -> " << virtualNodeCount
             << " virtual nodes ring pe add hue\n";
    }

    void onNodeRemoved(const Node &node, int virtualNodeCount) override {
        cout << "    [RING-LOG] LEFT    : " << node.getId() << " -> " << virtualNodeCount
             << " virtual nodes ring se hate\n";
    }

    void onNodeStatusChanged(const Node &node, NodeStatus oldStatus) override {
        cout << "    [RING-LOG] HEALTH  : " << node.getId() << " " << toText(oldStatus)
             << " -> " << toText(node.getStatus())
             << "  (ring positions wahi hain, sirf routing badli)\n";
    }

    void onRingRebuilt(const string &reason, size_t totalVirtualNodes) override {
        cout << "    [RING-LOG] REBUILD : " << reason << " -> " << totalVirtualNodes
             << " virtual nodes dobara compute hue ⚠ (saari keys hil sakti hain)\n";
    }

private:
    static string toText(NodeStatus status) {
        return status == NodeStatus::ACTIVE ? "ACTIVE" : "DOWN";
    }
};

} // namespace consistent_hashing_lld

#endif // CONSISTENT_HASHING_LLD_OBSERVERS_CONSOLERINGLOGGER_H

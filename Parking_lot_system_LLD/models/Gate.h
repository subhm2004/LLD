// ============================================================================
//  models/Gate.h — Entry/Exit gates (requirement #3: multiple entrances/exits)
// ----------------------------------------------------------------------------
//  Requirement #3: parking lot me MULTIPLE entry aur exit points hone chahiye
//  (vehicle flow smooth ho, congestion kam ho).
//
//  Gate = ek entry ya exit point. Type se pata chalta hai ENTRY hai ya EXIT.
//  ParkingLot inki list rakhta hai — vehicle kisi bhi entry gate se aa sakta,
//  kisi bhi exit gate se ja sakta. Ticket me entry gate id store hota
//  (kaunse gate se aaya, tracking ke liye).
//
//  Simple model — real me gate me barrier/sensor/camera hote, par LLD ke
//  liye id + type kaafi hai (multiple points ka concept dikhane ke liye).
// ============================================================================
#ifndef PARKING_LOT_SYSTEM_LLD_MODELS_GATE_H
#define PARKING_LOT_SYSTEM_LLD_MODELS_GATE_H

#include <bits/stdc++.h>

using namespace std;

namespace parking_lot_lld {

enum class GateType { ENTRY, EXIT };

class Gate {
public:
    Gate(int gateId, GateType type) : gateId_(gateId), type_(type) {}

    int getGateId() const { return gateId_; }
    GateType getType() const { return type_; }

    string toString() const {
        return string(type_ == GateType::ENTRY ? "ENTRY" : "EXIT") + "-Gate-" + to_string(gateId_);
    }

private:
    int gateId_;
    GateType type_;
};

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_MODELS_GATE_H

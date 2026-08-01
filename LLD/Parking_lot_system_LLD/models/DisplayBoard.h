// ============================================================================
//  models/DisplayBoard.h — DISPLAY BOARD (requirement #5/#7) [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #5: ek display board har spot type ki availability dikhaye.
//  Requirement #7: lot full hone pe prominently "FULL" notification dikhe.
//
//  ⭐ Ye ek OBSERVER hai (IParkingObserver implement karta) — ParkingLot ka
//  "subscriber". Jab bhi lot me kuch badalta (park/unpark), ParkingLot
//  onParkingUpdate() call karta, aur board APNE AAP latest availability
//  yaad rakh leta. Client kabhi bhi display() karke current state dekh sakta.
//
//  Observer pattern ka fayda yahan clearly dikhta: DisplayBoard ko har baar
//  manually update nahi karna padta — event-driven hai. Real parking lot ke
//  entrance pe jo LED board hota (kitne spots khaali) — bilkul waise.
//
//  FULL notification (requirement #6/#7): jab isFull true aata, board special
//  "⚠️ PARKING FULL" message dikhata — entrance pe customer ko turant pata.
// ============================================================================
#ifndef PARKING_LOT_SYSTEM_LLD_MODELS_DISPLAYBOARD_H
#define PARKING_LOT_SYSTEM_LLD_MODELS_DISPLAYBOARD_H

#include <bits/stdc++.h>

#include "../enums/SpotType.h"
#include "../observers/IParkingObserver.h"

using namespace std;

namespace parking_lot_lld {

class DisplayBoard : public IParkingObserver {
public:
    // OBSERVER callback — ParkingLot har update pe ye call karta hai.
    // Board bas latest state store kar leta (display() pe dikhega).
    void onParkingUpdate(const map<SpotType, int> &availability, bool isFull) override {
        availability_ = availability;
        isFull_ = isFull;

        // Full hone pe TURANT notification (requirement #7 — prominent)
        if (isFull_) {
            cout << "\n[DisplayBoard] *** PARKING FULL! No more vehicles can be admitted. ***\n";
        }
    }

    // Current availability screen — har spot type ka count (requirement #5)
    void display() const {
        cout << "\n========= DISPLAY BOARD =========\n";
        if (isFull_) {
            cout << "  STATUS: *** PARKING FULL ***\n";
        } else {
            cout << "  STATUS: OPEN\n";
        }
        cout << "  Available spots by type:\n";
        // map se har SpotType ka count print (map SpotType ordered rakhta hai)
        for (const auto &entry : availability_) {
            cout << "    " << spotTypeToString(entry.first) << ": " << entry.second << "\n";
        }
        cout << "=================================\n";
    }

private:
    map<SpotType, int> availability_;  // latest per-type available counts
    bool isFull_ = false;
};

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_MODELS_DISPLAYBOARD_H

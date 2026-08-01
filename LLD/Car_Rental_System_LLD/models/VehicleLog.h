// ============================================================================
//  models/VehicleLog.h — Har vehicle ki ACTIVITY LOG (requirement #7) [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #7: har vehicle ki saari activities ka detailed log rakho —
//  monitoring/audit ke liye. Kab reserve hua, kab pickup, kab return, kab
//  maintenance me gaya — sab timeline me.
//
//  Design: har log entry = timestamp + description string. Vehicle ke id se
//  mapped (CarRentalSystem me map<vehicleId, VehicleLog>). Koi bhi event pe
//  system addEntry() call karta.
//
//  Real fleet management me ye critical hota — kaunsi gaadi kab kahan thi,
//  kitni baar rent hui, kab service hui. Audit trail.
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_MODELS_VEHICLELOG_H
#define CAR_RENTAL_SYSTEM_LLD_MODELS_VEHICLELOG_H

#include <bits/stdc++.h>

using namespace std;

namespace car_rental_lld {

class VehicleLog {
public:
    // Ek activity record karo (day + kya hua)
    void addEntry(int day, const string &activity) {
        entries_.push_back("Day " + to_string(day) + ": " + activity);
    }

    void print(const string &vehicleId) const {
        cout << "  [Vehicle Log: " << vehicleId << "]\n";
        if (entries_.empty()) {
            cout << "    (no activity)\n";
            return;
        }
        for (const string &entry : entries_) {
            cout << "    - " << entry << "\n";
        }
    }

    const vector<string> &getEntries() const { return entries_; }

private:
    vector<string> entries_;  // chronological activity list
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_MODELS_VEHICLELOG_H

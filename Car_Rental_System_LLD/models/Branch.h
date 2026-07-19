// ============================================================================
//  models/Branch.h — Rental BRANCH + parking stalls (requirement #12/#13) [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #12: system multiple branches oversee kare (alag locations).
//  Requirement #13: har branch me parking STALLS hon vehicles ke liye.
//
//  Branch = id + naam + location + apni vehicle inventory + parking stalls.
//  Har branch apne vehicles ka MALIK hai (ownership — destructor me delete).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │   CarRentalSystem                                                       │
//  │        ├── Branch "Delhi"  -> vehicles[] + stalls (10)                  │
//  │        ├── Branch "Mumbai" -> vehicles[] + stalls (8)                   │
//  │        └── Branch "Pune"   -> vehicles[] + stalls (5)                   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  totalStalls_ = parking capacity (requirement #13). Vehicle add karte waqt
//  check hota ki stall available hai (stalls se zyada vehicles nahi).
//  ParkingStall alag class banane ki jagah simple count rakha hai (LLD ke
//  liye kaafi — "kitne stalls, kitne bhare" ka concept).
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_MODELS_BRANCH_H
#define CAR_RENTAL_SYSTEM_LLD_MODELS_BRANCH_H

#include <bits/stdc++.h>

#include "Vehicle.h"

using namespace std;

namespace car_rental_lld {

class Branch {
public:
    Branch(string branchId, string name, string location, int totalStalls)
        : branchId_(std::move(branchId)),
          name_(std::move(name)),
          location_(std::move(location)),
          totalStalls_(totalStalls) {}

    // Branch apne vehicles ka owner — cleanup destructor me
    ~Branch() {
        for (Vehicle *vehicle : vehicles_) {
            delete vehicle;
        }
    }

    const string &getId() const { return branchId_; }
    const string &getName() const { return name_; }
    const string &getLocation() const { return location_; }

    // Vehicle add karo — requirement #13: parking stall available hona chahiye
    void addVehicle(Vehicle *vehicle) {
        if (static_cast<int>(vehicles_.size()) >= totalStalls_) {
            throw runtime_error("Branch " + name_ + " has no free parking stalls");
        }
        vehicles_.push_back(vehicle);
    }

    const vector<Vehicle *> &getVehicles() const { return vehicles_; }

    // Parking availability (requirement #13)
    int getTotalStalls() const { return totalStalls_; }
    int getFreeStalls() const { return totalStalls_ - static_cast<int>(vehicles_.size()); }

    Vehicle *findVehicleById(const string &vehicleId) const {
        for (Vehicle *vehicle : vehicles_) {
            if (vehicle->getId() == vehicleId) {
                return vehicle;
            }
        }
        return nullptr;
    }

private:
    string branchId_;
    string name_;
    string location_;
    int totalStalls_;              // requirement #13 — parking capacity
    vector<Vehicle *> vehicles_;   // is branch ki inventory (owned)
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_MODELS_BRANCH_H

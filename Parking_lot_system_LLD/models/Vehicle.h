// ============================================================================
//  models/Vehicle.h — Ek vehicle ka data (number + type + handicapped permit)
// ----------------------------------------------------------------------------
//  Vehicle = number plate + type (car/truck/van/moto) + handicappedPermit flag.
//
//  ⭐ handicappedPermit_ NAYA field hai (requirement #2 ke HANDICAPPED spots
//  ke liye) — sirf permit wale vehicles hi handicapped spot use kar sakte.
//  ParkingSpot::canFitVehicle() isko check karta hai. Bina permit ke koi
//  handicapped spot me park nahi kar sakta (real life rule!).
//
//  Immutable data holder — banne ke baad badalta nahi (getters only).
// ============================================================================
#ifndef PARKING_LOT_SYSTEM_LLD_MODELS_VEHICLE_H
#define PARKING_LOT_SYSTEM_LLD_MODELS_VEHICLE_H

#include <bits/stdc++.h>

#include "../enums/VehicleType.h"

using namespace std;

namespace parking_lot_lld {

class Vehicle {
public:
    // handicappedPermit default false — normal vehicles ke liye pass mat karo
    Vehicle(string number, VehicleType type, bool handicappedPermit = false)
        : number_(std::move(number)), type_(type), handicappedPermit_(handicappedPermit) {}

    const string &getNumber() const { return number_; }
    VehicleType getType() const { return type_; }
    bool hasHandicappedPermit() const { return handicappedPermit_; }

private:
    string number_;
    VehicleType type_;
    bool handicappedPermit_;  // true = handicapped spot use kar sakta hai
};

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_MODELS_VEHICLE_H

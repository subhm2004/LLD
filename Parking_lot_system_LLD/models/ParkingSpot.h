// ============================================================================
//  models/ParkingSpot.h — Ek parking spot (SpotType + fit-mapping ka dil)
// ----------------------------------------------------------------------------
//  Ek spot ka data: id + SpotType (handicapped/compact/large/moto) + occupied.
//  Pehle ye VehicleType-based tha; ab SpotType-based hai (requirement #2).
//
//  ⭐ canFitVehicle() — SABSE IMPORTANT method (VehicleType <-> SpotType mapping):
//  "Kaunsi vehicle kaunse spot me park ho sakti hai?" ka pura rule yahan.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  FIT MAPPING (kaunsa vehicle kaunse spot me):                           │
//  │                                                                          │
//  │   Spot Type    | Kaun park kar sakta hai                                │
//  │   -------------+-----------------------------------------------------   │
//  │   HANDICAPPED  | SIRF handicapped-permit wale (koi bhi vehicle type)    │
//  │   MOTORCYCLE   | sirf MOTORCYCLE                                        │
//  │   COMPACT      | CAR ya MOTORCYCLE (chhote vehicles)                    │
//  │   LARGE        | koi bhi (CAR/TRUCK/VAN/MOTO — bada spot sab fit)       │
//  │                                                                          │
//  │  Real logic: bada vehicle chhote spot me nahi aata (TRUCK compact me   │
//  │  nahi), par chhota vehicle bade spot me aa sakta hai (MOTO large me).  │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef PARKING_LOT_SYSTEM_LLD_MODELS_PARKINGSPOT_H
#define PARKING_LOT_SYSTEM_LLD_MODELS_PARKINGSPOT_H

#include <bits/stdc++.h>

#include "../enums/SpotType.h"
#include "../enums/VehicleType.h"
#include "Vehicle.h"

using namespace std;

namespace parking_lot_lld {

class ParkingSpot {
public:
    ParkingSpot(int spotId, SpotType spotType)
        : spotId_(spotId), spotType_(spotType), occupied_(false) {}

    int getSpotId() const { return spotId_; }
    SpotType getSpotType() const { return spotType_; }
    bool isOccupied() const { return occupied_; }

    // >>> FIT MAPPING KA DIL <<< (upar wala table code me)
    bool canFitVehicle(const Vehicle &vehicle) const {
        if (occupied_) {
            return false;  // pehle se koi khada hai
        }
        // HANDICAPPED spot: sirf permit wale (vehicle type koi bhi ho)
        if (spotType_ == SpotType::HANDICAPPED) {
            return vehicle.hasHandicappedPermit();
        }
        switch (spotType_) {
            case SpotType::MOTORCYCLE:
                // Sirf bike fit hoti
                return vehicle.getType() == VehicleType::MOTORCYCLE;
            case SpotType::COMPACT:
                // Chhote vehicles: car ya bike
                return vehicle.getType() == VehicleType::CAR ||
                       vehicle.getType() == VehicleType::MOTORCYCLE;
            case SpotType::LARGE:
                // Bada spot — sab fit (truck/van/car/bike)
                return true;
            default:
                return false;
        }
    }

    void parkVehicle(const string &vehicleNumber) {
        if (occupied_) {
            throw runtime_error("Spot already occupied");
        }
        occupied_ = true;
        parkedVehicleNumber_ = vehicleNumber;
    }

    void removeVehicle() {
        occupied_ = false;
        parkedVehicleNumber_.clear();
    }

private:
    int spotId_;
    SpotType spotType_;
    bool occupied_;
    string parkedVehicleNumber_;
};

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_MODELS_PARKINGSPOT_H

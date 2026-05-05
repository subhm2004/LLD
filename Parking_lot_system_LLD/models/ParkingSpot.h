#ifndef PARKING_LOT_SYSTEM_LLD_MODELS_PARKINGSPOT_H
#define PARKING_LOT_SYSTEM_LLD_MODELS_PARKINGSPOT_H

#include <bits/stdc++.h>

#include "../enums/VehicleType.h"
#include "Vehicle.h"

using namespace std;

namespace parking_lot_lld {

class ParkingSpot {
public:
    ParkingSpot(int spotId, VehicleType supportedType)
        : spotId_(spotId), supportedType_(supportedType), occupied_(false) {}

    int getSpotId() const { return spotId_; }
    VehicleType getSupportedType() const { return supportedType_; }
    bool isOccupied() const { return occupied_; }

    bool canFitVehicle(const Vehicle &vehicle) const {
        return !occupied_ && vehicle.getType() == supportedType_;
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
    VehicleType supportedType_;
    bool occupied_;
    string parkedVehicleNumber_;
};

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_MODELS_PARKINGSPOT_H

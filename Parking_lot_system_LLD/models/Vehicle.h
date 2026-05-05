#ifndef PARKING_LOT_SYSTEM_LLD_MODELS_VEHICLE_H
#define PARKING_LOT_SYSTEM_LLD_MODELS_VEHICLE_H

#include <bits/stdc++.h>

#include "../enums/VehicleType.h"

using namespace std;

namespace parking_lot_lld {

class Vehicle {
public:
    Vehicle(string number, VehicleType type) : number_(std::move(number)), type_(type) {}

    const string &getNumber() const { return number_; }
    VehicleType getType() const { return type_; }

private:
    string number_;
    VehicleType type_;
};

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_MODELS_VEHICLE_H

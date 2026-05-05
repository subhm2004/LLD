#ifndef PARKING_LOT_SYSTEM_LLD_ENUMS_VEHICLETYPE_H
#define PARKING_LOT_SYSTEM_LLD_ENUMS_VEHICLETYPE_H

#include <bits/stdc++.h>

using namespace std;

namespace parking_lot_lld {

enum class VehicleType { BIKE, CAR, TRUCK };

inline string vehicleTypeToString(VehicleType type) {
    if (type == VehicleType::BIKE) {
        return "BIKE";
    }
    if (type == VehicleType::CAR) {
        return "CAR";
    }
    return "TRUCK";
}

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_ENUMS_VEHICLETYPE_H

// ============================================================================
//  enums/VehicleType.h — Kaunse VEHICLES park ho sakte hain (requirement #4)
// ----------------------------------------------------------------------------
//  Requirements ke hisaab se 4 vehicle types allowed hain:
//    CAR        -> standard passenger vehicles
//    TRUCK      -> bade/heavy vehicles
//    VAN        -> goods/people transport
//    MOTORCYCLE -> two-wheeler
//
//  ⚠️ DHYAN DO — VehicleType aur SpotType DO ALAG cheezein hain!
//    VehicleType = "kya park ho raha hai" (car/truck/van/bike)
//    SpotType    = "kahan park ho raha hai" (handicapped/compact/large/moto)
//  Ek CAR compact ya large spot me fit ho sakti hai — mapping ParkingSpot me.
//  (Pehle sirf BIKE/CAR/TRUCK tha; ab requirement ke hisaab se VAN add kiya
//   aur BIKE -> MOTORCYCLE rename — poore system me consistent.)
// ============================================================================
#ifndef PARKING_LOT_SYSTEM_LLD_ENUMS_VEHICLETYPE_H
#define PARKING_LOT_SYSTEM_LLD_ENUMS_VEHICLETYPE_H

#include <bits/stdc++.h>

using namespace std;

namespace parking_lot_lld {

enum class VehicleType { CAR, TRUCK, VAN, MOTORCYCLE };

// Enum ko readable string me badalne ka helper (logging/display ke liye)
inline string vehicleTypeToString(VehicleType type) {
    switch (type) {
        case VehicleType::CAR: return "CAR";
        case VehicleType::TRUCK: return "TRUCK";
        case VehicleType::VAN: return "VAN";
        case VehicleType::MOTORCYCLE: return "MOTORCYCLE";
    }
    return "UNKNOWN";
}

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_ENUMS_VEHICLETYPE_H

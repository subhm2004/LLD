// ============================================================================
//  enums/VehicleType.h — Vehicle ke 4 TYPES (requirement #2) [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #2: system me 4 vehicle types honge — car/truck/van/motorcycle.
//  Har type ke apne SUBTYPES bhi hain (requirement #3) — wo har concrete
//  vehicle class (Car/Van/Motorcycle/Truck) me define hain:
//    Car        -> economy / luxury / standard / compact
//    Van        -> passenger / cargo
//    Motorcycle -> cruiser / touring / sports
//    Truck      -> light / medium / high-duty
//
//  Ye enum "kaunsa broad type" batata hai; subtype "us type ke andar kaunsi
//  category". VehicleFactory dono ka use karke sahi vehicle banata hai.
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_ENUMS_VEHICLETYPE_H
#define CAR_RENTAL_SYSTEM_LLD_ENUMS_VEHICLETYPE_H

#include <bits/stdc++.h>

using namespace std;

namespace car_rental_lld {

enum class VehicleType { CAR, TRUCK, VAN, MOTORCYCLE };

inline string vehicleTypeToString(VehicleType type) {
    switch (type) {
        case VehicleType::CAR: return "CAR";
        case VehicleType::TRUCK: return "TRUCK";
        case VehicleType::VAN: return "VAN";
        case VehicleType::MOTORCYCLE: return "MOTORCYCLE";
    }
    return "UNKNOWN";
}

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_ENUMS_VEHICLETYPE_H

// ============================================================================
//  enums/VehicleStatus.h — Vehicle abhi kis state me hai
// ----------------------------------------------------------------------------
//  AVAILABLE   -> rent ke liye free
//  RESERVED    -> booking confirmed, par abhi pickup nahi (customer aayega)
//  RENTED      -> pickup ho gaya, customer ke paas hai
//  MAINTENANCE -> servicing/repair me (rent nahi ho sakta)
//
//  Ek chhota state machine: AVAILABLE -> RESERVED (booking) -> RENTED (pickup)
//  -> AVAILABLE (return). MAINTENANCE kabhi bhi set ho sakta (staff dwara).
//  CarRentalSystem in transitions ko manage karta hai.
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_ENUMS_VEHICLESTATUS_H
#define CAR_RENTAL_SYSTEM_LLD_ENUMS_VEHICLESTATUS_H

#include <bits/stdc++.h>

using namespace std;

namespace car_rental_lld {

enum class VehicleStatus { AVAILABLE, RESERVED, RENTED, MAINTENANCE };

inline string vehicleStatusToString(VehicleStatus status) {
    switch (status) {
        case VehicleStatus::AVAILABLE: return "AVAILABLE";
        case VehicleStatus::RESERVED: return "RESERVED";
        case VehicleStatus::RENTED: return "RENTED";
        case VehicleStatus::MAINTENANCE: return "MAINTENANCE";
    }
    return "UNKNOWN";
}

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_ENUMS_VEHICLESTATUS_H

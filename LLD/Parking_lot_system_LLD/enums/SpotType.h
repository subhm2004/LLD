// ============================================================================
//  enums/SpotType.h — 4 tarah ke PARKING SPOTS (requirement #2) [NAYA]
// ----------------------------------------------------------------------------
//  Requirements ke hisaab se 4 distinct spot categories:
//    HANDICAPPED -> sirf handicapped permit wale vehicles ke liye reserved
//    COMPACT     -> chhote spots (chhoti cars ke liye)
//    LARGE       -> bade spots (truck/van jaise bade vehicles ke liye)
//    MOTORCYCLE  -> sirf motorcycles ke liye
//
//  ⭐ YE VehicleType SE ALAG HAI (interview me ye distinction important):
//    VehicleType (car/truck/van/moto) = "kya" park ho raha hai
//    SpotType (handi/compact/large/moto) = "kahan" park ho sakta hai
//  Ek vehicle multiple spot types me fit ho sakti hai (jaise CAR compact YA
//  large me). Ye "kaunsi vehicle kaunse spot me fit hai" ka mapping
//  ParkingSpot::canFitVehicle() me hai. Real parking lots aise hi kaam karte —
//  spot ka size vehicle ke size se match hona chahiye.
// ============================================================================
#ifndef PARKING_LOT_SYSTEM_LLD_ENUMS_SPOTTYPE_H
#define PARKING_LOT_SYSTEM_LLD_ENUMS_SPOTTYPE_H

#include <bits/stdc++.h>

using namespace std;

namespace parking_lot_lld {

enum class SpotType { HANDICAPPED, COMPACT, LARGE, MOTORCYCLE };

inline string spotTypeToString(SpotType type) {
    switch (type) {
        case SpotType::HANDICAPPED: return "HANDICAPPED";
        case SpotType::COMPACT: return "COMPACT";
        case SpotType::LARGE: return "LARGE";
        case SpotType::MOTORCYCLE: return "MOTORCYCLE";
    }
    return "UNKNOWN";
}

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_ENUMS_SPOTTYPE_H

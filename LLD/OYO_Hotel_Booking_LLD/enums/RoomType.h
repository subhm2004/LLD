// ============================================================================
//  enums/RoomType.h  —  Room ki category (aur uski keemat ka aadhaar)
// ----------------------------------------------------------------------------
//     STANDARD -> aam room (sasta)
//     DELUXE   -> behtar (mehnga)
//     SUITE    -> sabse premium
//
//  📌 Is LLD me price seedha `Room::pricePerNight_` me store hota hai (har room
//     apna daam rakhta) — type sirf display/category ke liye hai. Kal ko chahe
//     to type ke hisaab se base-price/multiplier logic add ho sakta (jaise
//     SUITE = STANDARD x 3), par abhi har room ka apna explicit price hai.
//
//  💡 `enum class` (scoped) — `RoomType::SUITE` likhna padta, int me chupke se
//     convert nahi hota (type safety).
// ============================================================================
#ifndef OYO_HOTEL_BOOKING_LLD_ENUMS_ROOMTYPE_H
#define OYO_HOTEL_BOOKING_LLD_ENUMS_ROOMTYPE_H

#include <string>

namespace oyo_hotel_lld {

using namespace std;

enum class RoomType { STANDARD, DELUXE, SUITE };

inline string roomTypeToString(RoomType type) {
    switch (type) {
        case RoomType::STANDARD: return "STANDARD";
        case RoomType::DELUXE:   return "DELUXE";
        case RoomType::SUITE:    return "SUITE";
    }
    return "UNKNOWN";
}

}  // namespace oyo_hotel_lld

#endif

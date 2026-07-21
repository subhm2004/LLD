// ============================================================================
//  enums/RoomStatus.h  —  Ek room ki coarse state
// ----------------------------------------------------------------------------
//     AVAILABLE   -> khaali (coarse indicator)
//     BOOKED      -> abhi kisi booking me (coarse indicator)
//     MAINTENANCE -> repair/cleaning me — kabhi book nahi ho sakta
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ DHYAAN DO: AVAILABILITY ka ASLI source ye status NAHI hai            │
//  │                                                                          │
//  │  Kaunsa room kis DATE-RANGE pe free hai — ye AvailabilityService          │
//  │  date-overlap check se decide karta hai (bookings ke against), NA ki      │
//  │  is `status_` field se. Kyunki ek room days 10-12 pe booked ho sakta hai  │
//  │  par days 20-22 pe FREE (segment-based, hotel bhi seat/train jaisa).     │
//  │                                                                          │
//  │  To in me se availability ke liye SIRF `MAINTENANCE` matter karta hai —   │
//  │  wo room har date ke liye block. BOOKED/AVAILABLE bas ek "coarse flag"    │
//  │  hai (aakhri booking pe update hota), availability ka authority nahi.     │
//  │  (isRoomAvailableForStay me dekho: sirf MAINTENANCE check + date overlap.)│
//  │                                                                          │
//  │  ⚠ Isi wajah se BOOKED/AVAILABLE kabhi "asli" haalat se thoda alag ho     │
//  │  sakta (jaise ek future booking hote hue bhi AVAILABLE) — par ye harmless │
//  │  hai kyunki availability decision date-overlap se hota hai, is flag se nahi.│
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef OYO_HOTEL_BOOKING_LLD_ENUMS_ROOMSTATUS_H
#define OYO_HOTEL_BOOKING_LLD_ENUMS_ROOMSTATUS_H

#include <string>

namespace oyo_hotel_lld {

using namespace std;

enum class RoomStatus { AVAILABLE, BOOKED, MAINTENANCE };

inline string roomStatusToString(RoomStatus status) {
    switch (status) {
        case RoomStatus::AVAILABLE:   return "AVAILABLE";
        case RoomStatus::BOOKED:      return "BOOKED";
        case RoomStatus::MAINTENANCE: return "MAINTENANCE";
    }
    return "UNKNOWN";
}

}  // namespace oyo_hotel_lld

#endif

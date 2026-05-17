#ifndef OYO_HOTEL_BOOKING_LLD_ENUMS_ROOMSTATUS_H
#define OYO_HOTEL_BOOKING_LLD_ENUMS_ROOMSTATUS_H

#include <bits/stdc++.h>

namespace oyo_hotel_lld {

using namespace std;

enum class RoomStatus { AVAILABLE, BOOKED, MAINTENANCE };

inline string roomStatusToString(RoomStatus status) {
    switch (status) {
        case RoomStatus::AVAILABLE:
            return "AVAILABLE";
        case RoomStatus::BOOKED:
            return "BOOKED";
        case RoomStatus::MAINTENANCE:
            return "MAINTENANCE";
    }
    return "UNKNOWN";
}

}  // namespace oyo_hotel_lld

#endif

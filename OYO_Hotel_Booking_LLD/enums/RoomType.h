#ifndef OYO_HOTEL_BOOKING_LLD_ENUMS_ROOMTYPE_H
#define OYO_HOTEL_BOOKING_LLD_ENUMS_ROOMTYPE_H

#include <bits/stdc++.h>

namespace oyo_hotel_lld {

using namespace std;

enum class RoomType { STANDARD, DELUXE, SUITE };

inline string roomTypeToString(RoomType type) {
    switch (type) {
        case RoomType::STANDARD:
            return "STANDARD";
        case RoomType::DELUXE:
            return "DELUXE";
        case RoomType::SUITE:
            return "SUITE";
    }
    return "UNKNOWN";
}

}  // namespace oyo_hotel_lld

#endif

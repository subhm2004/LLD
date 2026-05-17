#ifndef OYO_HOTEL_BOOKING_LLD_ENUMS_BOOKINGSTATUS_H
#define OYO_HOTEL_BOOKING_LLD_ENUMS_BOOKINGSTATUS_H

#include <bits/stdc++.h>

namespace oyo_hotel_lld {

using namespace std;

enum class BookingStatus { PENDING, CONFIRMED, CANCELLED, CHECKED_IN, CHECKED_OUT };

inline string bookingStatusToString(BookingStatus status) {
    switch (status) {
        case BookingStatus::PENDING:
            return "PENDING";
        case BookingStatus::CONFIRMED:
            return "CONFIRMED";
        case BookingStatus::CANCELLED:
            return "CANCELLED";
        case BookingStatus::CHECKED_IN:
            return "CHECKED_IN";
        case BookingStatus::CHECKED_OUT:
            return "CHECKED_OUT";
    }
    return "UNKNOWN";
}

}  // namespace oyo_hotel_lld

#endif

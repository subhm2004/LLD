// ============================================================================
//  enums/BookingStatus.h — Booking abhi kis stage me hai
// ----------------------------------------------------------------------------
//  CREATED   -> abhi bana, confirm nahi
//  CONFIRMED -> pakka ho gaya, vehicle reserve (pickup ka wait)
//  ACTIVE    -> pickup ho gaya, rental chalu (vehicle customer ke paas)
//  COMPLETED -> return ho gaya, khatam
//  CANCELLED -> customer ne cancel kiya (requirement #6)
//
//  Booking lifecycle: CREATED -> CONFIRMED -> ACTIVE -> COMPLETED.
//  CONFIRMED se CANCELLED bhi ho sakta (requirement #6 — reservation cancel).
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_ENUMS_BOOKINGSTATUS_H
#define CAR_RENTAL_SYSTEM_LLD_ENUMS_BOOKINGSTATUS_H

#include <bits/stdc++.h>

using namespace std;

namespace car_rental_lld {

enum class BookingStatus { CREATED, CONFIRMED, ACTIVE, COMPLETED, CANCELLED };

inline string bookingStatusToString(BookingStatus status) {
    switch (status) {
        case BookingStatus::CREATED: return "CREATED";
        case BookingStatus::CONFIRMED: return "CONFIRMED";
        case BookingStatus::ACTIVE: return "ACTIVE";
        case BookingStatus::COMPLETED: return "COMPLETED";
        case BookingStatus::CANCELLED: return "CANCELLED";
    }
    return "UNKNOWN";
}

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_ENUMS_BOOKINGSTATUS_H

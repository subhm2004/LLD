// ============================================================================
//  enums/BookingStatus.h  —  Ek booking ki zindagi ke stages (state machine)
// ----------------------------------------------------------------------------
//  Ek booking ka safar:
//     PENDING   -> abhi bani (payment/confirm baaki) — is demo me turant CONFIRMED
//     CONFIRMED -> pakki booking (room reserved)
//     CHECKED_IN -> guest aa gaya (stay chal rahi hai)
//     CHECKED_OUT -> stay khatam (room fir se free)
//     CANCELLED  -> cancel ho gayi (room release)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ VALID TRANSITIONS (state machine — facade enforce karta hai)          │
//  │                                                                          │
//  │     CONFIRMED ── checkIn ──► CHECKED_IN ── checkOut ──► CHECKED_OUT       │
//  │         │                                                                │
//  │         └── cancel ──► CANCELLED   (SIRF check-in se PEHLE cancel ho sakti)│
//  │                                                                          │
//  │  Rules (OYOHotelBookingSystem me guarded):                               │
//  │    - checkIn: sirf CONFIRMED se                                           │
//  │    - checkOut: sirf CHECKED_IN se                                         │
//  │    - cancel: CHECKED_IN ke baad NAHI (guest aa chuka)                     │
//  │  Galat transition -> exception. Isse invalid workflow (jaise pehle        │
//  │  checkout phir checkin) rukta hai.                                        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ CANCELLED/CHECKED_OUT ka role AVAILABILITY me: AvailabilityService in do
//     statuses ki bookings ko IGNORE karta hai (wo room ab us range ke liye free).
//     Isi liye checkout/cancel hote hi wo room dobara book ho sakta hai.
// ============================================================================
#ifndef OYO_HOTEL_BOOKING_LLD_ENUMS_BOOKINGSTATUS_H
#define OYO_HOTEL_BOOKING_LLD_ENUMS_BOOKINGSTATUS_H

#include <string>

namespace oyo_hotel_lld {

using namespace std;

enum class BookingStatus { PENDING, CONFIRMED, CANCELLED, CHECKED_IN, CHECKED_OUT };

// Enum -> readable string (printing/logging ke liye).
inline string bookingStatusToString(BookingStatus status) {
    switch (status) {
        case BookingStatus::PENDING:     return "PENDING";
        case BookingStatus::CONFIRMED:   return "CONFIRMED";
        case BookingStatus::CANCELLED:   return "CANCELLED";
        case BookingStatus::CHECKED_IN:  return "CHECKED_IN";
        case BookingStatus::CHECKED_OUT: return "CHECKED_OUT";
    }
    return "UNKNOWN";
}

}  // namespace oyo_hotel_lld

#endif

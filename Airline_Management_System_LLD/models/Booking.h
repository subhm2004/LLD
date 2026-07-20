// ============================================================================
//  models/Booking.h  —  Ek booking (kaunse passenger ne kaunsi seats li)
// ----------------------------------------------------------------------------
//  Booking sab kuch ek jagah jodti hai: kaunsa passenger, kaunsi flight, kaunsi
//  seats (ek se zyada ho sakti hain), abhi kis status me hai, aur kul kitna paisa.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `seatIds` ek VECTOR hai — ek booking me kai seats                     │
//  │                                                                          │
//  │  Ek passenger ek hi booking me 2-3 seats le sakta hai (family/friends).  │
//  │  Isi liye single seatId nahi, poori list. `totalAmount` un sab seats ke  │
//  │  fare ka jod hota hai (har seat pe PricingStrategy lag ke).              │
//  │                                                                          │
//  │  📌 Booking ka `status` (BookingStatus) uski poori kahani batata hai:     │
//  │     PENDING (paise baaki) -> CONFIRMED (paid) -> CANCELLED/REFUNDED.      │
//  │     Payment ALAG model (Payment.h) me track hota hai — booking sirf apna  │
//  │     status rakhti hai, paise ki detail Payment sambhalta hai (SRP).      │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_BOOKING_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_BOOKING_H

#include <string>
#include <vector>

#include "../enums/BookingStatus.h"

namespace airline_mgmt {

struct Booking {
    std::string bookingId;              // "BK_1" — unique
    std::string passengerId;            // kiski booking
    std::string flightId;               // kaunsi flight
    std::vector<std::string> seatIds;   // kaunsi seats (ek se zyada ho sakti hain)
    BookingStatus status;               // PENDING / CONFIRMED / CANCELLED / REFUNDED
    double totalAmount;                 // sab seats ka fare (strategy ke baad)
};

} // namespace airline_mgmt

#endif

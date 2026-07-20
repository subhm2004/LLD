// ============================================================================
//  models/Seat.h  —  Ek flight ki ek seat (bookable unit)
// ----------------------------------------------------------------------------
//  Har seat KISI EK flight ki hoti hai (flightId se bandhi). Isme uski jagah
//  (row/column), class, abhi khaali hai ya nahi (status), aur base price hoti hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `basePrice` "asli" price nahi hai — sirf SHURUAATI daam               │
//  │                                                                          │
//  │  Booking ke waqt asli fare = PricingStrategy(seat). Economy ka basePrice  │
//  │  seedha lagta hai, par Business ka basePrice x 2.5 ho jaata hai. Yaani    │
//  │  seat sirf apna base daam rakhti hai; "final kitna" ka faisal strategy    │
//  │  karti hai. Isse pricing logic seat model se ALAG rehta hai (SRP).       │
//  │                                                                          │
//  │  📌 `seatId` = flightId + "-" + suffix (jaise "AI-101-E1"). Isliye seat   │
//  │     id se hi pata chal jaata hai wo kaunse flight ki hai — aur ye globally│
//  │     unique bhi rehti hai (do flights ki seat id kabhi takraati nahi).    │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_SEAT_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_SEAT_H

#include <string>

#include "../enums/SeatClass.h"
#include "../enums/SeatStatus.h"

namespace airline_mgmt {

struct Seat {
    std::string seatId;   // "AI-101-E1" (flightId + suffix) — globally unique
    std::string flightId; // ye seat kis flight ki hai
    int row;              // kaunsi row
    int column;           // kaunsa column
    SeatClass seatClass;  // ECONOMY / PREMIUM_ECONOMY / BUSINESS (pricing base)
    SeatStatus status;    // AVAILABLE / BOOKED / BLOCKED
    double basePrice;     // shuruaati daam (strategy isko multiply karti hai)
};

} // namespace airline_mgmt

#endif

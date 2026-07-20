// ============================================================================
//  enums/SeatStatus.h  —  Ek seat abhi khaali hai ya nahi
// ----------------------------------------------------------------------------
//     AVAILABLE -> khaali hai, book ho sakti hai
//     BOOKED    -> kisi ne le li (booking me lock ho gayi)
//     BLOCKED   -> temporary rok (maintenance/crew ke liye) — demo me mostly
//                  AVAILABLE/BOOKED hi use hote hain, BLOCKED future ke liye
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SEAT DOUBLE-BOOKING yahi enum + mutex se rukti hai                    │
//  │                                                                          │
//  │  Do log ek saath same seat book karne aayein — dono createBooking()       │
//  │  bulaate hain. Facade ek `bookingMutex_` se ise "ek-ek karke" chalata     │
//  │  hai: pehla seat ko BOOKED kar deta hai, doosra jab aata hai to seat      │
//  │  AVAILABLE nahi milti -> "Seat not available" error. Ek hi seat do logon  │
//  │  ko kabhi nahi milti. (Concurrency requirement R8.)                      │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_ENUMS_SEATSTATUS_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_ENUMS_SEATSTATUS_H

namespace airline_mgmt {

enum class SeatStatus { AVAILABLE, BOOKED, BLOCKED };

} // namespace airline_mgmt

#endif

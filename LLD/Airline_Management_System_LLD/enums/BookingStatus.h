// ============================================================================
//  enums/BookingStatus.h  —  Ek booking ki zindagi ke stages
// ----------------------------------------------------------------------------
//  Ek booking ka safar (chhota state machine):
//
//     PENDING   -> seat lock ho gayi, par PAISA abhi baaki hai (payment pending)
//     CONFIRMED -> payment ho gaya, ticket pakka
//     CANCELLED -> pending booking cancel hui (paisa tha hi nahi, sirf seat chhodi)
//     REFUNDED  -> CONFIRMED booking cancel hui -> seat chhodi + paisa wapas
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ CANCELLED vs REFUNDED — ye do alag kyun?                              │
//  │                                                                          │
//  │   Farak "paisa gaya tha ya nahi" pe hai:                                 │
//  │     PENDING booking cancel   -> CANCELLED  (payment hua hi nahi tha)      │
//  │     CONFIRMED booking cancel -> REFUNDED   (payment tha -> refund karo)   │
//  │                                                                          │
//  │   Facade ka cancelBooking() yahi decide karta hai: agar status CONFIRMED  │
//  │   tha to refund process karke REFUNDED, warna sidha CANCELLED.            │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_ENUMS_BOOKINGSTATUS_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_ENUMS_BOOKINGSTATUS_H

namespace airline_mgmt {

enum class BookingStatus { PENDING, CONFIRMED, CANCELLED, REFUNDED };

} // namespace airline_mgmt

#endif

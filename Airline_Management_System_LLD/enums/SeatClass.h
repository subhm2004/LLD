// ============================================================================
//  enums/SeatClass.h  —  Seat kis class ki hai (pricing ka aadhaar)
// ----------------------------------------------------------------------------
//     ECONOMY         -> sabse sasti (base price)
//     PREMIUM_ECONOMY -> beech wali (thodi mehngi)
//     BUSINESS        -> sabse mehngi (aage wali, zyada aaram)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Ye enum PRICING STRATEGY ka dil hai                                   │
//  │                                                                          │
//  │  SeatClassPricingStrategy is class ke hisaab se fare multiply karti hai:  │
//  │     ECONOMY         -> basePrice x 1.0                                    │
//  │     PREMIUM_ECONOMY -> basePrice x 1.4                                    │
//  │     BUSINESS        -> basePrice x 2.5                                    │
//  │                                                                          │
//  │  Kal ko naya class chahiye (FIRST_CLASS)? Yahan add karo aur strategy me  │
//  │  ek case — baaki system chhuo mat. (Strategy pattern ka fayda.)          │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_ENUMS_SEATCLASS_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_ENUMS_SEATCLASS_H

namespace airline_mgmt {

enum class SeatClass { ECONOMY, PREMIUM_ECONOMY, BUSINESS };

} // namespace airline_mgmt

#endif

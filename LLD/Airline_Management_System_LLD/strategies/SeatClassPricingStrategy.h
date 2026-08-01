// ============================================================================
//  strategies/SeatClassPricingStrategy.h  —  CONCRETE STRATEGY (class-wise fare)
// ----------------------------------------------------------------------------
//  IPricingStrategy ka ek asli implementation: fare seat ki CLASS ke hisaab se
//  nikaalta hai. Base price ko class ka multiplier laga deta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ MULTIPLIER TABLE                                                       │
//  │                                                                          │
//  │     ECONOMY         ->  basePrice x 1.0   (jaisa hai waisa)               │
//  │     PREMIUM_ECONOMY ->  basePrice x 1.4   (40% mehnga)                    │
//  │     BUSINESS        ->  basePrice x 2.5   (2.5 guna)                      │
//  │                                                                          │
//  │  Ye ek SIMPLE, predictable pricing hai — interview me samjhaane layak.   │
//  │  Real airline me pricing bahut complex hoti (demand, time-to-departure,  │
//  │  route, season). Wo sab ek NAYI strategy class me aa sakta hai bina isko  │
//  │  chhue — yahi Strategy pattern ka poora point hai.                       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  💡 `switch` ke saare cases cover hain (ECONOMY/PREMIUM/BUSINESS). Aakhir me
//     ek `return seat.basePrice` bhi hai — ye "safety net" hai agar kabhi naya
//     SeatClass add ho jaaye aur yahan case add karna bhool jaayein (crash ke
//     bajaye base price lauta dega). Defensive default.
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_STRATEGIES_SEATCLASSPRICINGSTRATEGY_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_STRATEGIES_SEATCLASSPRICINGSTRATEGY_H

#include "../enums/SeatClass.h"
#include "IPricingStrategy.h"

namespace airline_mgmt {

class SeatClassPricingStrategy : public IPricingStrategy {
public:
    // Interface ka method implement — seat ki class dekh ke multiplier lagao.
    double calculateSeatFare(const Seat &seat) const override {
        switch (seat.seatClass) {
        case SeatClass::ECONOMY:
            return seat.basePrice;          // x1.0
        case SeatClass::PREMIUM_ECONOMY:
            return seat.basePrice * 1.4;    // x1.4
        case SeatClass::BUSINESS:
            return seat.basePrice * 2.5;    // x2.5
        }
        return seat.basePrice; // defensive default (naya class add hua to crash na ho)
    }
};

} // namespace airline_mgmt

#endif

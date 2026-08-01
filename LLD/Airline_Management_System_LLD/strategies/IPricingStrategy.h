// ============================================================================
//  strategies/IPricingStrategy.h  —  STRATEGY PATTERN ka interface (pricing)
// ----------------------------------------------------------------------------
//  "Ek seat ka fare kaise nikaale?" — ye ek BADALNE WALA faisla hai. Aaj class
//  ke hisaab se, kal demand ke hisaab se (surge pricing), parso festival discount.
//  Isi liye pricing ko ek INTERFACE ke peeche rakh diya — Strategy Pattern.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ STRATEGY PATTERN — "algorithm ko plug-and-play banao"                 │
//  │                                                                          │
//  │  Facade ek `IPricingStrategy*` rakhta hai — usse pata nahi andar kaunsa   │
//  │  formula hai. Wo bas `calculateSeatFare(seat)` bulata hai. Kaunsa asli    │
//  │  formula chalega, wo inject ki gayi concrete strategy decide karti hai.  │
//  │                                                                          │
//  │  Fayda: naya pricing chahiye (DynamicPricingStrategy, FestiveDiscount...) │
//  │  to bas nayi class banao jo is interface ko implement kare, aur facade me │
//  │  inject kar do. Booking/seat/facade ka ek line bhi nahi badlega.          │
//  │  Yahi OPEN/CLOSED PRINCIPLE hai — extension ke liye khula, modification   │
//  │  ke liye band.                                                            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ VIRTUAL DESTRUCTOR zaroori hai: facade `shared_ptr<IPricingStrategy>` me
//     asli object (SeatClassPricingStrategy) rakhta hai base pointer ke through.
//     Bina virtual destructor ke, delete pe derived ka destructor chalta hi nahi
//     -> undefined behavior. Rule: koi bhi virtual function hai to destructor bhi
//     virtual. `= default` isliye kyunki isme khud kuch delete karne ko nahi hai.
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_STRATEGIES_IPRICINGSTRATEGY_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_STRATEGIES_IPRICINGSTRATEGY_H

#include "../models/Seat.h"

namespace airline_mgmt {

class IPricingStrategy {
public:
    virtual ~IPricingStrategy() = default;

    // Pure virtual (`= 0`) — har concrete strategy ko ye implement karna HI padega.
    // Ek seat lo, uska final fare lauta do.
    virtual double calculateSeatFare(const Seat &seat) const = 0;
};

} // namespace airline_mgmt

#endif

// ============================================================================
//  strategies/IPricingStrategy.h  —  STRATEGY PATTERN ka interface (pricing)
// ----------------------------------------------------------------------------
//  "Stay ka total kitna banega?" — ye ek BADALNE WALA faisla hai. Flat rate?
//  Weekend surcharge? Seasonal? Loyalty discount? Har pricing policy alag. Isliye
//  ise interface ke peeche rakh diya — Strategy Pattern.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ STRATEGY PATTERN — pricing algorithm plug-and-play                    │
//  │                                                                          │
//  │  PricingService ek `IPricingStrategy*` rakhti hai — usse pata nahi andar  │
//  │  kaunsa formula hai. Wo bas `calculateTotal(price, in, out)` bulati hai.  │
//  │  Aur ye RUNTIME pe badla ja sakta (useWeekendPricing) — bina baaki system │
//  │  ko chhue. Naya pricing (SeasonalStrategy)? Nayi class banao, inject karo.│
//  │  Baaki code untouched (Open/Closed Principle).                           │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ VIRTUAL DESTRUCTOR zaroori: PricingService `unique_ptr<IPricingStrategy>`
//     me derived object rakhti hai base pointer ke through — bina virtual
//     destructor ke cleanup adhoora (UB).
// ============================================================================
#ifndef OYO_HOTEL_BOOKING_LLD_STRATEGIES_IPRICINGSTRATEGY_H
#define OYO_HOTEL_BOOKING_LLD_STRATEGIES_IPRICINGSTRATEGY_H

namespace oyo_hotel_lld {

class IPricingStrategy {
public:
    virtual ~IPricingStrategy() = default;

    // Pure virtual — har concrete strategy ko implement karna HI padega.
    // Per-night price + check-in/out days lo, poore stay ka total lauta do.
    virtual double calculateTotal(double pricePerNight, int checkInDay, int checkOutDay) const = 0;
};

}  // namespace oyo_hotel_lld

#endif

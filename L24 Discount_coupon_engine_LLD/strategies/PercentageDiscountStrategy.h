// ============================================================================
//  strategies/PercentageDiscountStrategy.h — Concrete Strategy #2: PERCENT off
// ----------------------------------------------------------------------------
//  "10% off" ka plain formula: (percent/100) × base. Na cap, na koi twist —
//  Single Responsibility ka perfect example: EK class, EK formula.
//  Use hota hai: SeasonalOffer (category subtotal pe) aur LoyaltyDiscount
//  (cart total pe) — SAME strategy, alag base amount. Yahi reuse ka fayda!
// ============================================================================
#ifndef DISCOUNT_COUPON_LLD_STRATEGIES_PERCENTAGEDISCOUNTSTRATEGY_H
#define DISCOUNT_COUPON_LLD_STRATEGIES_PERCENTAGEDISCOUNTSTRATEGY_H

#include "IDiscountStrategy.h"

namespace discount_coupon_lld {

// -----------------------------------------------------------------------------
// PercentageDiscountStrategy
// Kya karta hai:
// - Base amount ka percentage discount return karta hai.
// - No cap logic yahan nahi hota; sirf plain percent formula.
// -----------------------------------------------------------------------------
class PercentageDiscountStrategy : public IDiscountStrategy {
    double percent;

public:
    explicit PercentageDiscountStrategy(double pct) : percent(pct) {}

    // Formula: discount = (percent / 100) * baseAmount
    double calculate(double baseAmount) override { return (percent / 100.0) * baseAmount; }
};

}

#endif

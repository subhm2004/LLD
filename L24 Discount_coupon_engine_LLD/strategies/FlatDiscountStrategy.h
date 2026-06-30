// strategies/FlatDiscountStrategy.h — Concrete: fixed flat amount off (jaise ₹100).
// IDiscountStrategy implement karta hai.
#ifndef DISCOUNT_COUPON_LLD_STRATEGIES_FLATDISCOUNTSTRATEGY_H
#define DISCOUNT_COUPON_LLD_STRATEGIES_FLATDISCOUNTSTRATEGY_H

#include <algorithm>

#include "IDiscountStrategy.h"

namespace discount_coupon_lld {

// -----------------------------------------------------------------------------
// FlatDiscountStrategy
// Kya karta hai:
// - Fixed rupee discount apply karta hai.
// - Safety: discount cart total se bada na ho, isliye min() use hota hai.
// -----------------------------------------------------------------------------
class FlatDiscountStrategy : public IDiscountStrategy {
    double amount;

public:
    explicit FlatDiscountStrategy(double amt) : amount(amt) {}

    // Example:
    // base=80, flat=100 -> return 80 (total negative nahi hone dena)
    // base=500, flat=100 -> return 100
    double calculate(double baseAmount) override { return std::min(amount, baseAmount); }
};

}

#endif

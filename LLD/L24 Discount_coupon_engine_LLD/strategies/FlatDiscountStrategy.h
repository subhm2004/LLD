// ============================================================================
//  strategies/FlatDiscountStrategy.h — Concrete Strategy #1: FLAT amount off
// ----------------------------------------------------------------------------
//  "Rs 100 off" type ka seedha discount. Ek safety detail ke saath:
//  min(flat, baseAmount) — taaki 80 Rs ke cart pe 100 ka discount laga ke
//  total NEGATIVE na ho jaye (dukaan wale ko ulta paise dene padte! 😄)
//  Use hota hai: BulkPurchaseDiscount me.
// ============================================================================
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

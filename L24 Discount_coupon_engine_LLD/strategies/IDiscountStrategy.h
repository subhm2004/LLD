// strategies/IDiscountStrategy.h — STRATEGY interface: "amount par kitna discount".
// Concrete: Flat / Percentage / PercentageWithCap. Coupons isi math ko reuse karte.
#ifndef DISCOUNT_COUPON_LLD_STRATEGIES_IDISCOUNTSTRATEGY_H
#define DISCOUNT_COUPON_LLD_STRATEGIES_IDISCOUNTSTRATEGY_H

namespace discount_coupon_lld {

// -----------------------------------------------------------------------------
// IDiscountStrategy (Strategy interface)
// Kya karta hai:
// - "discount amount ka calculation formula" abstract karta hai.
// - Coupons applicability decide karti hain, strategy sirf math karti hai.
// -----------------------------------------------------------------------------
class IDiscountStrategy {
public:
    virtual ~IDiscountStrategy() = default;

    // baseAmount input par discount value return karo.
    // Example:
    // - FLAT: min(flat, baseAmount)
    // - PERCENT: baseAmount * percent
    // - PERCENT_WITH_CAP: min(percentDiscount, cap)
    virtual double calculate(double baseAmount) = 0;
};

}

#endif

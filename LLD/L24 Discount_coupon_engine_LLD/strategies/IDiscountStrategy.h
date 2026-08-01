// ============================================================================
//  strategies/IDiscountStrategy.h — STRATEGY interface (L8 wala pattern, live!)
// ----------------------------------------------------------------------------
//  Discount ka MATH coupon se ALAG kyun kiya? Separation of concerns:
//    - COUPON decide karta hai: "main lagta hoon ya nahi" (eligibility)
//    - STRATEGY decide karti hai: "kitna discount banega" (math formula)
//  Fayda: SeasonalOffer aur LoyaltyDiscount dono PERCENT strategy REUSE
//  karte hain — formula ek jagah likha, do jagah use hua. Naya math aaye
//  (jaise "buy 1 get 1") to bas nayi strategy class — coupons untouched!
// ============================================================================
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

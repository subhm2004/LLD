// ============================================================================
//  strategies/PercentageDiscountStrategy.h  —  CONCRETE: X% off (jaise SAVE10)
// ----------------------------------------------------------------------------
//  Cart ke subtotal ka ek percentage discount:
//     discount = (percent / 100) * subtotal
//     jaise: 10% of ₹5097 = ₹509.7
//
//  ⭐ Yahan clamp ki zaroorat NAHI (FlatDiscount ke ulat) — kyunki percentage
//     hamesha base ka hi hissa hota hai. 100% se zyada percent na do to discount
//     kabhi subtotal se zyada ho hi nahi sakta. (Aur PricingService phir bhi
//     `min(discount, subtotal)` se double-guard karta hai.)
//
//  📌 Same interface (IDiscountStrategy), bilkul alag formula — Coupon ko farak
//     hi nahi padta ki andar flat hai ya percentage. Yahi Strategy ka poora point:
//     naya discount rule = nayi chhoti class, baaki system untouched.
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_PERCENTAGEDISCOUNTSTRATEGY_H
#define ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_PERCENTAGEDISCOUNTSTRATEGY_H

#include "../interfaces/IDiscountStrategy.h"

namespace ecommerce_checkout_lld {

class PercentageDiscountStrategy : public IDiscountStrategy {
public:
    explicit PercentageDiscountStrategy(double percent) : percent_(percent) {}

    double calculate(double baseAmount) const override { return (percent_ / 100.0) * baseAmount; }

private:
    double percent_{0.0}; // 10.0 = 10% off
};

}  // namespace ecommerce_checkout_lld

#endif

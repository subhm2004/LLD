// ============================================================================
//  strategies/FlatDiscountStrategy.h  —  CONCRETE: seedha ₹X off (jaise FLAT100)
// ----------------------------------------------------------------------------
//  Fixed amount ka discount. Par ek zaroori guard ke saath:
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ CLAMP: discount kabhi cart se ZYADA nahi ho sakta                     │
//  │                                                                          │
//  │     return flatAmount_ > baseAmount ? baseAmount : flatAmount_;          │
//  │                                                                          │
//  │  Agar ₹500 ka FLAT discount hai par cart sirf ₹300 ka hai, to discount    │
//  │  ₹300 hi hoga (₹500 nahi) — warna payable NEGATIVE ho jaata (shop user ko │
//  │  paisa deti! 😄). Ye "clamp to base" guard chhota par zaroori hai.        │
//  │                                                                          │
//  │  📌 Dhyaan do: PricingService bhi `min(discount, subtotal)` karta hai —   │
//  │     yaani ye guard DO jagah hai (defense in depth). Ek layer bhool bhi    │
//  │     jaye to doosri bacha leti.                                           │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_FLATDISCOUNTSTRATEGY_H
#define ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_FLATDISCOUNTSTRATEGY_H

#include "../interfaces/IDiscountStrategy.h"

namespace ecommerce_checkout_lld {

class FlatDiscountStrategy : public IDiscountStrategy {
public:
    explicit FlatDiscountStrategy(double flatAmount) : flatAmount_(flatAmount) {}

    double calculate(double baseAmount) const override {
        // ⭐ Cart se zyada discount nahi (warna payable negative — upar note).
        return flatAmount_ > baseAmount ? baseAmount : flatAmount_;
    }

private:
    double flatAmount_{0.0}; // kitna flat off (jaise 100)
};

}  // namespace ecommerce_checkout_lld

#endif

// ============================================================================
//  interfaces/IDiscountStrategy.h  —  STRATEGY interface #1: discount kaise lage
// ----------------------------------------------------------------------------
//  "Coupon se kitna discount banega?" — ye badalne wala faisla hai. Flat ₹100?
//  10% off? Buy-one-get-one? Har rule alag. Isliye interface ke peeche.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ IS SYSTEM ME DO ALAG STRATEGY FAMILIES HAIN                          │
//  │                                                                          │
//  │     IDiscountStrategy -> discount kaise nikle (Flat / Percentage)         │
//  │     IPaymentStrategy  -> paisa kaise le (UPI / Card / COD)                │
//  │                                                                          │
//  │  Dono ALAG interfaces hain kyunki dono ALAG cheezein badalti hain. Ek     │
//  │  hi interface me thoos dete to dono aapas me juda jaate (coupon badla to  │
//  │  payment code chhoona padta) — SRP toot jaata.                            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Coupon is strategy ko HOLD karta hai (`unique_ptr<IDiscountStrategy>`) —
//     yaani coupon ko pata nahi wo flat hai ya percentage, wo bas
//     `strategy_->calculate(subtotal)` bulata hai. Naya discount type chahiye
//     (BOGO / tiered)? Nayi class banao — Coupon/CouponService untouched.
//
//  ⭐ VIRTUAL DESTRUCTOR: Coupon base pointer (unique_ptr<IDiscountStrategy>) me
//     derived object rakhta hai — bina virtual destructor ke cleanup adhoora (UB).
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_INTERFACES_IDISCOUNTSTRATEGY_H
#define ECOMMERCE_CART_CHECKOUT_LLD_INTERFACES_IDISCOUNTSTRATEGY_H

namespace ecommerce_checkout_lld {

class IDiscountStrategy {
public:
    virtual ~IDiscountStrategy() = default;

    // Base amount (cart subtotal) lo -> kitna discount banega, wo lauta do.
    virtual double calculate(double baseAmount) const = 0;
};

}  // namespace ecommerce_checkout_lld

#endif

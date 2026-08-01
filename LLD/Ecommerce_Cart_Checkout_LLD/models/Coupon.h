// ============================================================================
//  models/Coupon.h  —  Ek discount coupon (Strategy ko HOLD karta hai)
// ----------------------------------------------------------------------------
//  Coupon me 4 cheezein: code, minimum cart value, DISCOUNT STRATEGY, aur expiry.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ COUPON = ELIGIBILITY RULES + DISCOUNT STRATEGY (do alag cheezein)     │
//  │                                                                          │
//  │     isValidFor()       -> "ye coupon LAG sakta hai kya?"                  │
//  │                           (expiry nahi hui + cart minimum se upar)        │
//  │     calculateDiscount() -> "kitna discount banega?"                       │
//  │                           (ye kaam STRATEGY ko delegate hota hai)         │
//  │                                                                          │
//  │  Coupon ko PATA NAHI ki andar flat hai ya percentage — wo bas             │
//  │  `strategy_->calculate(subtotal)` bulata hai. Isi liye naya discount type │
//  │  (BOGO, tiered) banana ho to sirf nayi IDiscountStrategy class chahiye,   │
//  │  Coupon/CouponService bilkul untouched. (Strategy Pattern ka fayda.)     │
//  │                                                                          │
//  │     SAVE10  = Percentage(10%) + minCart 500                               │
//  │     FLAT100 = Flat(₹100)      + minCart 1000                              │
//  │  Same Coupon class, alag strategy = alag behavior.                        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠ `unique_ptr` member -> Coupon MOVE-ONLY hai (copy nahi ho sakta)      │
//  │                                                                          │
//  │  Isi wajah se CouponService `registerCoupon(Coupon coupon)` by-value leta │
//  │  hai aur andar `std::move` karta hai — copy possible hi nahi. Aur         │
//  │  `unordered_map<string, Coupon>` bhi move se hi bharta hai.              │
//  │  📌 Ye ek chain-effect hai: ek unique_ptr member poori class ko           │
//  │     move-only bana deta hai. Design me iska dhyaan rakhna padta hai.     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `expiryDay` int hai (real date nahi) — simplified calendar. System ka
//     `currentDay_` isse compare hota hai (facade me setCurrentDay se set).
//     Null-strategy guard bhi hai (`if (!strategy_) return 0.0`) — defensive.
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_COUPON_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_COUPON_H

#include <memory>
#include <string>
#include <utility>

#include "../interfaces/IDiscountStrategy.h"

namespace ecommerce_checkout_lld {

using namespace std;

class Coupon {
public:
    Coupon(string code, double minCartValue, std::unique_ptr<IDiscountStrategy> strategy,
           int expiryDay)
        : code_(std::move(code)),
          minCartValue_(minCartValue),
          strategy_(std::move(strategy)), // ⭐ discount ka "kaise" injected
          expiryDay_(expiryDay) {}

    const string &getCode() const { return code_; }
    double getMinCartValue() const { return minCartValue_; }
    int getExpiryDay() const { return expiryDay_; }

    // ---- ELIGIBILITY: ye coupon lag sakta hai kya? ------------------------
    bool isValidFor(double cartSubtotal, int currentDay) const {
        return currentDay <= expiryDay_ && cartSubtotal >= minCartValue_;
    }

    // ---- AMOUNT: kitna discount? (strategy ko delegate) -------------------
    double calculateDiscount(double cartSubtotal) const {
        if (!strategy_) {
            return 0.0; // defensive: strategy hi nahi to koi discount nahi
        }
        return strategy_->calculate(cartSubtotal); // flat? percentage? — pata nahi, farak nahi
    }

private:
    string code_;                                // "SAVE10"
    double minCartValue_{0.0};                   // itne se upar cart ho tabhi lagega
    std::unique_ptr<IDiscountStrategy> strategy_; // ⭐ move-only bana deta hai (upar note)
    int expiryDay_{0};                            // simplified calendar day
};

}  // namespace ecommerce_checkout_lld

#endif

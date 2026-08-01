// ============================================================================
//  services/CouponService.h  —  Coupons ka registry + apply karne ka gate
// ----------------------------------------------------------------------------
//  Coupons register karo, aur checkout ke waqt "kitna discount milega" pucho.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `applyCouponOrThrow` ke 3 raaste                                      │
//  │                                                                          │
//  │   1. couponCode KHAALI  -> 0.0 discount (koi error nahi)                  │
//  │      📌 Bina coupon ke checkout bilkul normal hai — isliye caller ko      │
//  │         `if (coupon.empty())` likhna hi nahi padta. API clean rehti hai.  │
//  │   2. code galat / rule fail -> THROW                                      │
//  │      📌 User ne coupon DAALA hai par nahi lag raha — usko batana ZAROORI  │
//  │         hai. Chup-chaap 0 discount de dete to user ko lagta coupon laga   │
//  │         hai aur bill dekh ke shock lagta. 😱 Silent failure = worst UX.  │
//  │   3. sab theek -> Coupon ko discount poochho (wo Strategy ko delegate)    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `registerCoupon(Coupon coupon)` BY VALUE leta hai — kyunki Coupon MOVE-ONLY
//     hai (uske andar `unique_ptr<IDiscountStrategy>` hai). `const Coupon&` lete
//     to map me daalne ke liye COPY chahiye hoti = compile error. By-value + move
//     hi ek tareeka hai. 📌 Ye Coupon.h ke unique_ptr member ka chain-effect hai.
//
//  📌 Validation aur calculation ka batwara: CouponService "kaunsa coupon, valid
//     hai kya" dekhta hai; ASLI discount ka math Coupon->Strategy me hai.
//     Isliye naya discount type add karne pe ye file CHHUNI hi nahi padti.
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_COUPONSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_COUPONSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "../models/Coupon.h"

namespace ecommerce_checkout_lld {

using namespace std;

class CouponService {
public:
    // ⭐ by-value + move (Coupon copy nahi ho sakta — upar note).
    void registerCoupon(Coupon coupon) {
        coupons_.insert_or_assign(coupon.getCode(), std::move(coupon));
    }

    double applyCouponOrThrow(const string &couponCode, double cartSubtotal, int currentDay) const {
        if (couponCode.empty()) {
            return 0.0; // (1) coupon hi nahi diya -> normal checkout
        }
        auto it = coupons_.find(couponCode);
        if (it == coupons_.end()) {
            throw runtime_error("Invalid coupon code: " + couponCode); // (2) galat code
        }
        const Coupon &coupon = it->second;
        if (!coupon.isValidFor(cartSubtotal, currentDay)) {
            // (2) code sahi par rule fail (expire ho gaya / cart minimum se kam)
            throw runtime_error("Coupon not applicable: " + couponCode);
        }
        // (3) ⭐ math Coupon -> IDiscountStrategy ko delegate (flat? %? — farak nahi)
        return coupon.calculateDiscount(cartSubtotal);
    }

private:
    unordered_map<string, Coupon> coupons_; // code -> coupon (move-only values)
};

}  // namespace ecommerce_checkout_lld

#endif

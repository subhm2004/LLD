#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_COUPONSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_COUPONSERVICE_H

#include <bits/stdc++.h>

#include "../models/Coupon.h"

namespace ecommerce_checkout_lld {

using namespace std;

class CouponService {
public:
    void registerCoupon(Coupon coupon) {
        coupons_.insert_or_assign(coupon.getCode(), std::move(coupon));
    }

    double applyCouponOrThrow(const string &couponCode, double cartSubtotal, int currentDay) const {
        if (couponCode.empty()) {
            return 0.0;
        }
        auto it = coupons_.find(couponCode);
        if (it == coupons_.end()) {
            throw runtime_error("Invalid coupon code: " + couponCode);
        }
        const Coupon &coupon = it->second;
        if (!coupon.isValidFor(cartSubtotal, currentDay)) {
            throw runtime_error("Coupon not applicable: " + couponCode);
        }
        return coupon.calculateDiscount(cartSubtotal);
    }

private:
    unordered_map<string, Coupon> coupons_;
};

}  // namespace ecommerce_checkout_lld

#endif

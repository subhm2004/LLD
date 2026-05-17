#ifndef DISCOUNT_COUPON_LLD_CORE_COUPONMANAGER_H
#define DISCOUNT_COUPON_LLD_CORE_COUPONMANAGER_H

#include <mutex>
#include <string>
#include <vector>

#include "../coupons/Coupon.h"
#include "../models/Cart.h"

namespace discount_coupon_lld {

class CouponManager {
    Coupon *head = nullptr;
    mutable std::mutex mtx;

    CouponManager() = default;

public:
    static CouponManager &getInstance() {
        static CouponManager instance;
        return instance;
    }

    void registerCoupon(Coupon *coupon) {
        std::lock_guard<std::mutex> lock(mtx);
        if (!head) {
            head = coupon;
            return;
        }
        Coupon *cur = head;
        while (cur->getNext()) {
            cur = cur->getNext();
        }
        cur->setNext(coupon);
    }

    std::vector<std::string> getApplicable(Cart *cart) const {
        std::lock_guard<std::mutex> lock(mtx);
        std::vector<std::string> result;
        Coupon *cur = head;
        while (cur) {
            if (cur->isApplicable(cart)) {
                result.push_back(cur->name());
            }
            cur = cur->getNext();
        }
        return result;
    }

    double applyAll(Cart *cart) {
        std::lock_guard<std::mutex> lock(mtx);
        if (head) {
            head->applyDiscount(cart);
        }
        return cart->getCurrentTotal();
    }
};

}

#endif

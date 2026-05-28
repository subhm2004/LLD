#ifndef DISCOUNT_COUPON_LLD_CORE_COUPONMANAGER_H
#define DISCOUNT_COUPON_LLD_CORE_COUPONMANAGER_H

#include <mutex>
#include <string>
#include <vector>

#include "../coupons/Coupon.h"
#include "../models/Cart.h"

namespace discount_coupon_lld {

// -----------------------------------------------------------------------------
// CouponManager (Singleton + Chain Registry)
// Kya karta hai:
// - Runtime me coupons register karta hai (linked-chain form me).
// - Cart ke liye applicable coupon names list karta hai.
// - Chain apply karke final total nikalta hai.
// - Thread-safe operations with mutex.
// -----------------------------------------------------------------------------
class CouponManager {
    Coupon *head = nullptr;
    // mutable: const methods (getApplicable) me bhi lock lag sake.
    mutable std::mutex mtx;

    CouponManager() = default;

public:
    // Meyers singleton style.
    static CouponManager &getInstance() {
        static CouponManager instance;
        return instance;
    }

    // Coupon chain ke tail pe new coupon add karo.
    // Registration order = application order.
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

    // Dry-run style lookup:
    // kaunse coupons currently cart par applicable hain (without applying discount).
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

    // Full coupon chain apply karo aur final current total return karo.
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

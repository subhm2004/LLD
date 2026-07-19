// ============================================================================
//  core/CouponManager.h — SINGLETON + chain registry (system ka orchestrator)
// ----------------------------------------------------------------------------
//  Ye class 2 patterns ka combo hai:
//    1. SINGLETON — poore program me coupon registry EK hi honi chahiye
//       (do registries hoti to kisi me Seasonal, kisi me Banking — chaos!)
//       Meyers style: getInstance() ke andar static local — C++11 se
//       thread-safe initialization ki GUARANTEE hai, bina lock likhe.
//    2. CHAIN REGISTRY — Chain of Responsibility ka "assembler". Coupons
//       ko tail pe jodta hai: REGISTRATION ORDER = APPLICATION ORDER.
//       (Isi liye main.cpp me exclusive BankingCoupon LAST register hota hai!)
//
//  Teen operations deta hai:
//    registerCoupon() -> chain ke tail pe naya coupon jodo
//    getApplicable()  -> DRY-RUN: kaunse lagenge, bina lagaye batao
//    applyAll()       -> poori chain chalao, final total do
//  Sab thread-safe hain (mutex) — multiple threads coupons padh/likh sakein.
// ============================================================================
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

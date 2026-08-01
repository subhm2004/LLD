// ============================================================================
//  coupons/BulkPurchaseDiscount.h — Concrete Coupon #3: bulk buy pe FLAT off
// ----------------------------------------------------------------------------
//  "1000 se upar ki shopping? Rs 100 flat off." Ek SUBTLE design decision
//  yahan chhupi hai (interview me bolne layak!):
//    - Eligibility ORIGINAL total pe check hoti hai (pehle wale coupons
//      discount laga ke cart ko threshold ke NEECHE gira dein, to bhi ye
//      coupon unfairly disqualify na ho!)
//    - Par discount CURRENT total pe lagta hai (stacking ka normal rule)
//  Math ke liye FLAT strategy — jo min(flat, base) se total negative hone
//  se bhi bachati hai.
// ============================================================================
#ifndef DISCOUNT_COUPON_LLD_COUPONS_BULKPURCHASEDISCOUNT_H
#define DISCOUNT_COUPON_LLD_COUPONS_BULKPURCHASEDISCOUNT_H

#include "../enums/StrategyType.h"
#include "../factories/DiscountStrategyManager.h"
#include "Coupon.h"

namespace discount_coupon_lld {

// -----------------------------------------------------------------------------
// BulkPurchaseDiscount coupon
// Kya karta hai:
// - Minimum cart threshold cross hone par fixed flat discount deta hai.
// -----------------------------------------------------------------------------
class BulkPurchaseDiscount : public Coupon {
    double threshold;
    double flatOff;
    IDiscountStrategy *strategy = nullptr;

public:
    BulkPurchaseDiscount(double thr, double off) : threshold(thr), flatOff(off) {
        // Bulk discount math = flat strategy
        strategy = DiscountStrategyManager::getInstance().createStrategy(StrategyType::FLAT, flatOff);
    }

    ~BulkPurchaseDiscount() override { delete strategy; }

    // Eligibility original total par check hoti hai,
    // taaki earlier discounts threshold ko unfairly impact na karein.
    bool isApplicable(Cart *cart) override { return cart->getOriginalTotal() >= threshold; }

    // Discount current total par apply hota hai.
    double getDiscount(Cart *cart) override { return strategy->calculate(cart->getCurrentTotal()); }

    std::string name() override {
        return "Bulk Purchase Rs " + std::to_string(static_cast<int>(flatOff)) + " off over " +
               std::to_string(static_cast<int>(threshold));
    }
};

}

#endif

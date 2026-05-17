#ifndef DISCOUNT_COUPON_LLD_COUPONS_BULKPURCHASEDISCOUNT_H
#define DISCOUNT_COUPON_LLD_COUPONS_BULKPURCHASEDISCOUNT_H

#include "../enums/StrategyType.h"
#include "../factories/DiscountStrategyManager.h"
#include "Coupon.h"

namespace discount_coupon_lld {

class BulkPurchaseDiscount : public Coupon {
    double threshold;
    double flatOff;
    IDiscountStrategy *strategy = nullptr;

public:
    BulkPurchaseDiscount(double thr, double off) : threshold(thr), flatOff(off) {
        strategy = DiscountStrategyManager::getInstance().createStrategy(StrategyType::FLAT, flatOff);
    }

    ~BulkPurchaseDiscount() override { delete strategy; }

    bool isApplicable(Cart *cart) override { return cart->getOriginalTotal() >= threshold; }

    double getDiscount(Cart *cart) override { return strategy->calculate(cart->getCurrentTotal()); }

    std::string name() override {
        return "Bulk Purchase Rs " + std::to_string(static_cast<int>(flatOff)) + " off over " +
               std::to_string(static_cast<int>(threshold));
    }
};

}

#endif

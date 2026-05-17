#ifndef DISCOUNT_COUPON_LLD_COUPONS_LOYALTYDISCOUNT_H
#define DISCOUNT_COUPON_LLD_COUPONS_LOYALTYDISCOUNT_H

#include "../enums/StrategyType.h"
#include "../factories/DiscountStrategyManager.h"
#include "Coupon.h"

namespace discount_coupon_lld {

class LoyaltyDiscount : public Coupon {
    double percent;
    IDiscountStrategy *strategy = nullptr;

public:
    explicit LoyaltyDiscount(double pct) : percent(pct) {
        strategy = DiscountStrategyManager::getInstance().createStrategy(StrategyType::PERCENT, percent);
    }

    ~LoyaltyDiscount() override { delete strategy; }

    bool isApplicable(Cart *cart) override { return cart->isLoyaltyMember(); }

    double getDiscount(Cart *cart) override { return strategy->calculate(cart->getCurrentTotal()); }

    std::string name() override {
        return "Loyalty Discount " + std::to_string(static_cast<int>(percent)) + "% off";
    }
};

}

#endif

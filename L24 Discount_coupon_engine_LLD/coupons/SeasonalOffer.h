#ifndef DISCOUNT_COUPON_LLD_COUPONS_SEASONALOFFER_H
#define DISCOUNT_COUPON_LLD_COUPONS_SEASONALOFFER_H

#include "../enums/StrategyType.h"
#include "../factories/DiscountStrategyManager.h"
#include "Coupon.h"

namespace discount_coupon_lld {

class SeasonalOffer : public Coupon {
    double percent;
    std::string category;
    IDiscountStrategy *strategy = nullptr;

public:
    SeasonalOffer(double pct, std::string cat)
        : percent(pct), category(std::move(cat)) {
        strategy = DiscountStrategyManager::getInstance().createStrategy(StrategyType::PERCENT, percent);
    }

    ~SeasonalOffer() override { delete strategy; }

    bool isApplicable(Cart *cart) override {
        for (CartItem *item : cart->getItems()) {
            if (item->getProduct()->getCategory() == category) {
                return true;
            }
        }
        return false;
    }

    double getDiscount(Cart *cart) override {
        double subtotal = 0.0;
        for (CartItem *item : cart->getItems()) {
            if (item->getProduct()->getCategory() == category) {
                subtotal += item->itemTotal();
            }
        }
        return strategy->calculate(subtotal);
    }

    std::string name() override {
        return "Seasonal Offer " + std::to_string(static_cast<int>(percent)) + "% off " + category;
    }
};

}

#endif

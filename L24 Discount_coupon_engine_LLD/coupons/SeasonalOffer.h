#ifndef DISCOUNT_COUPON_LLD_COUPONS_SEASONALOFFER_H
#define DISCOUNT_COUPON_LLD_COUPONS_SEASONALOFFER_H

#include "../enums/StrategyType.h"
#include "../factories/DiscountStrategyManager.h"
#include "Coupon.h"

namespace discount_coupon_lld {

// -----------------------------------------------------------------------------
// SeasonalOffer coupon
// Kya karta hai:
// - Category-level percentage discount deta hai.
// - Example: "Clothing par 10% off"
// -----------------------------------------------------------------------------
class SeasonalOffer : public Coupon {
    double percent;
    std::string category;
    IDiscountStrategy *strategy = nullptr;

public:
    SeasonalOffer(double pct, std::string cat)
        : percent(pct), category(std::move(cat)) {
        // Seasonal discount math = percentage strategy
        strategy = DiscountStrategyManager::getInstance().createStrategy(StrategyType::PERCENT, percent);
    }

    ~SeasonalOffer() override { delete strategy; }

    // Applicable tab hai jab cart me target category ka kam se kam ek item ho.
    bool isApplicable(Cart *cart) override {
        for (CartItem *item : cart->getItems()) {
            if (item->getProduct()->getCategory() == category) {
                return true;
            }
        }
        return false;
    }

    // Discount base = sirf matching category items ka subtotal
    // (poori cart par नहीं, category slice par percent)
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

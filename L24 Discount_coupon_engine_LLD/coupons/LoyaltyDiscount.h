#ifndef DISCOUNT_COUPON_LLD_COUPONS_LOYALTYDISCOUNT_H
#define DISCOUNT_COUPON_LLD_COUPONS_LOYALTYDISCOUNT_H

#include "../enums/StrategyType.h"
#include "../factories/DiscountStrategyManager.h"
#include "Coupon.h"

namespace discount_coupon_lld {

// -----------------------------------------------------------------------------
// LoyaltyDiscount coupon
// Kya karta hai:
// - Loyalty users ko cart-level percentage discount deta hai.
// -----------------------------------------------------------------------------
class LoyaltyDiscount : public Coupon {
    double percent;
    IDiscountStrategy *strategy = nullptr;

public:
    explicit LoyaltyDiscount(double pct) : percent(pct) {
        // Loyalty discount math = percentage strategy
        strategy = DiscountStrategyManager::getInstance().createStrategy(StrategyType::PERCENT, percent);
    }

    ~LoyaltyDiscount() override { delete strategy; }

    // Applicable only for loyalty member carts.
    bool isApplicable(Cart *cart) override { return cart->isLoyaltyMember(); }

    // Cart-level discount: currentTotal par apply hota hai.
    // Isse pehle lage coupons ka effect consider ho jata hai (stacking behavior).
    double getDiscount(Cart *cart) override { return strategy->calculate(cart->getCurrentTotal()); }

    std::string name() override {
        return "Loyalty Discount " + std::to_string(static_cast<int>(percent)) + "% off";
    }
};

}

#endif

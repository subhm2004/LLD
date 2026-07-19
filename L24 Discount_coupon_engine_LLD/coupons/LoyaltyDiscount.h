// ============================================================================
//  coupons/LoyaltyDiscount.h — Concrete Coupon #2: loyalty members ka % off
// ----------------------------------------------------------------------------
//  "Loyalty member ho? 5% off poore cart pe." Do khaas baatein:
//    1. CART-level hai — discount CURRENT total pe lagta hai, matlab isse
//       PEHLE lage coupons ka asar isme count hota hai (stacking behavior:
//       Seasonal ne 300 kam kiya to Loyalty ka 5% us kam hue total pe lagega).
//    2. Math ke liye PERCENT strategy (factory se).
//  Eligibility ekdum simple: cart.isLoyaltyMember() true ho bas.
// ============================================================================
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

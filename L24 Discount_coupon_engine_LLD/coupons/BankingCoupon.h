#ifndef DISCOUNT_COUPON_LLD_COUPONS_BANKINGCOUPON_H
#define DISCOUNT_COUPON_LLD_COUPONS_BANKINGCOUPON_H

#include "../enums/StrategyType.h"
#include "../factories/DiscountStrategyManager.h"
#include "Coupon.h"

namespace discount_coupon_lld {

// -----------------------------------------------------------------------------
// BankingCoupon coupon
// Kya karta hai:
// - Specific bank + minimum spend rule par based percent discount deta hai.
// - Discount capped hota hai (max limit).
// - Exclusive behavior: apply hone ke baad chain stop.
// -----------------------------------------------------------------------------
class BankingCoupon : public Coupon {
    std::string bank;
    double minSpend;
    double percent;
    double offCap;
    IDiscountStrategy *strategy = nullptr;

public:
    BankingCoupon(const std::string &bankName, double minSpendAmount, double discountPercent, double discountCap)
        : bank(bankName), minSpend(minSpendAmount), percent(discountPercent), offCap(discountCap) {
        // Banking coupon math = percent with cap
        strategy = DiscountStrategyManager::getInstance().createStrategy(StrategyType::PERCENT_WITH_CAP, percent,
                                                                         offCap);
    }

    ~BankingCoupon() override { delete strategy; }

    // Eligibility:
    // 1) selected payment bank match ho
    // 2) original total minimum spend se bada/equal ho
    bool isApplicable(Cart *cart) override {
        return cart->getPaymentBank() == bank && cart->getOriginalTotal() >= minSpend;
    }

    // Discount current total par compute hota hai.
    double getDiscount(Cart *cart) override { return strategy->calculate(cart->getCurrentTotal()); }

    // Exclusive coupon:
    // true stacking ke bajay chain yahin stop karni hai.
    bool isCombinable() override { return false; }

    std::string name() override {
        return bank + " Bank " + std::to_string(static_cast<int>(percent)) + "% off upto " +
               std::to_string(static_cast<int>(offCap)) + " (exclusive)";
    }
};

}

#endif

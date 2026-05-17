#ifndef DISCOUNT_COUPON_LLD_COUPONS_BANKINGCOUPON_H
#define DISCOUNT_COUPON_LLD_COUPONS_BANKINGCOUPON_H

#include "../enums/StrategyType.h"
#include "../factories/DiscountStrategyManager.h"
#include "Coupon.h"

namespace discount_coupon_lld {

class BankingCoupon : public Coupon {
    std::string bank;
    double minSpend;
    double percent;
    double offCap;
    IDiscountStrategy *strategy = nullptr;

public:
    BankingCoupon(const std::string &bankName, double minSpendAmount, double discountPercent, double discountCap)
        : bank(bankName), minSpend(minSpendAmount), percent(discountPercent), offCap(discountCap) {
        strategy = DiscountStrategyManager::getInstance().createStrategy(StrategyType::PERCENT_WITH_CAP, percent,
                                                                         offCap);
    }

    ~BankingCoupon() override { delete strategy; }

    bool isApplicable(Cart *cart) override {
        return cart->getPaymentBank() == bank && cart->getOriginalTotal() >= minSpend;
    }

    double getDiscount(Cart *cart) override { return strategy->calculate(cart->getCurrentTotal()); }

    std::string name() override {
        return bank + " Bank " + std::to_string(static_cast<int>(percent)) + "% off upto " +
               std::to_string(static_cast<int>(offCap));
    }
};

}

#endif

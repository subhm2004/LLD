#ifndef DISCOUNT_COUPON_LLD_STRATEGIES_FLATDISCOUNTSTRATEGY_H
#define DISCOUNT_COUPON_LLD_STRATEGIES_FLATDISCOUNTSTRATEGY_H

#include <algorithm>

#include "IDiscountStrategy.h"

namespace discount_coupon_lld {

class FlatDiscountStrategy : public IDiscountStrategy {
    double amount;

public:
    explicit FlatDiscountStrategy(double amt) : amount(amt) {}

    double calculate(double baseAmount) override { return std::min(amount, baseAmount); }
};

}

#endif

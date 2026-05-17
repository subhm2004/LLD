#ifndef DISCOUNT_COUPON_LLD_STRATEGIES_PERCENTAGEDISCOUNTSTRATEGY_H
#define DISCOUNT_COUPON_LLD_STRATEGIES_PERCENTAGEDISCOUNTSTRATEGY_H

#include "IDiscountStrategy.h"

namespace discount_coupon_lld {

class PercentageDiscountStrategy : public IDiscountStrategy {
    double percent;

public:
    explicit PercentageDiscountStrategy(double pct) : percent(pct) {}

    double calculate(double baseAmount) override { return (percent / 100.0) * baseAmount; }
};

}

#endif

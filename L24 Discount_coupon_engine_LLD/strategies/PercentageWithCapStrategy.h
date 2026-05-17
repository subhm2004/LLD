#ifndef DISCOUNT_COUPON_LLD_STRATEGIES_PERCENTAGEWITHCAPSTRATEGY_H
#define DISCOUNT_COUPON_LLD_STRATEGIES_PERCENTAGEWITHCAPSTRATEGY_H

#include <algorithm>

#include "IDiscountStrategy.h"

namespace discount_coupon_lld {

class PercentageWithCapStrategy : public IDiscountStrategy {
    double percent;
    double cap;

public:
    PercentageWithCapStrategy(double pct, double capVal) : percent(pct), cap(capVal) {}

    double calculate(double baseAmount) override {
        double disc = (percent / 100.0) * baseAmount;
        return std::min(disc, cap);
    }
};

}

#endif

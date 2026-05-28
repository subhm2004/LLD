#ifndef DISCOUNT_COUPON_LLD_STRATEGIES_PERCENTAGEWITHCAPSTRATEGY_H
#define DISCOUNT_COUPON_LLD_STRATEGIES_PERCENTAGEWITHCAPSTRATEGY_H

#include <algorithm>

#include "IDiscountStrategy.h"

namespace discount_coupon_lld {

// -----------------------------------------------------------------------------
// PercentageWithCapStrategy
// Kya karta hai:
// - Percentage discount calculate karta hai.
// - Max discount cap enforce karta hai (banking coupon use-case).
// -----------------------------------------------------------------------------
class PercentageWithCapStrategy : public IDiscountStrategy {
    double percent;
    double cap;

public:
    PercentageWithCapStrategy(double pct, double capVal) : percent(pct), cap(capVal) {}

    // disc = percent of base
    // return min(disc, cap)
    double calculate(double baseAmount) override {
        double disc = (percent / 100.0) * baseAmount;
        return std::min(disc, cap);
    }
};

}

#endif

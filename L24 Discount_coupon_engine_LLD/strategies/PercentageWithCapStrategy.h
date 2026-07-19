// ============================================================================
//  strategies/PercentageWithCapStrategy.h — Concrete Strategy #3: % + MAX CAP
// ----------------------------------------------------------------------------
//  "15% off, MAX Rs 500" — bank offers ka classic formula:
//  min((percent/100) × base, cap). 50,000 ke cart pe 15% = 7500 banta,
//  par cap 500 hai to 500 hi milega — bank apna nuksan limit karta hai!
//  Use hota hai: BankingCoupon me. Dhyan do: ye PercentageDiscountStrategy
//  ko inherit NAHI karti — chhoti si duplication (percent formula) accept
//  ki hai taaki hierarchy flat aur simple rahe.
// ============================================================================
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

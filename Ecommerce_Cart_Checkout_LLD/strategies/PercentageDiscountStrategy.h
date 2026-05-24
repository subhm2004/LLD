#ifndef ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_PERCENTAGEDISCOUNTSTRATEGY_H
#define ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_PERCENTAGEDISCOUNTSTRATEGY_H

#include "../interfaces/IDiscountStrategy.h"

namespace ecommerce_checkout_lld {

class PercentageDiscountStrategy : public IDiscountStrategy {
public:
    explicit PercentageDiscountStrategy(double percent) : percent_(percent) {}

    double calculate(double baseAmount) const override { return (percent_ / 100.0) * baseAmount; }

private:
    double percent_{0.0};
};

}  // namespace ecommerce_checkout_lld

#endif

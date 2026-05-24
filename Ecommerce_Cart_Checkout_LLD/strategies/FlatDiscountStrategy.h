#ifndef ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_FLATDISCOUNTSTRATEGY_H
#define ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_FLATDISCOUNTSTRATEGY_H

#include "../interfaces/IDiscountStrategy.h"

namespace ecommerce_checkout_lld {

class FlatDiscountStrategy : public IDiscountStrategy {
public:
    explicit FlatDiscountStrategy(double flatAmount) : flatAmount_(flatAmount) {}

    double calculate(double baseAmount) const override {
        return flatAmount_ > baseAmount ? baseAmount : flatAmount_;
    }

private:
    double flatAmount_{0.0};
};

}  // namespace ecommerce_checkout_lld

#endif

#ifndef DISCOUNT_COUPON_LLD_STRATEGIES_IDISCOUNTSTRATEGY_H
#define DISCOUNT_COUPON_LLD_STRATEGIES_IDISCOUNTSTRATEGY_H

namespace discount_coupon_lld {

class IDiscountStrategy {
public:
    virtual ~IDiscountStrategy() = default;
    virtual double calculate(double baseAmount) = 0;
};

}

#endif

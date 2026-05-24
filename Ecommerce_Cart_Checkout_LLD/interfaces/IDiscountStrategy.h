#ifndef ECOMMERCE_CART_CHECKOUT_LLD_INTERFACES_IDISCOUNTSTRATEGY_H
#define ECOMMERCE_CART_CHECKOUT_LLD_INTERFACES_IDISCOUNTSTRATEGY_H

namespace ecommerce_checkout_lld {

class IDiscountStrategy {
public:
    virtual ~IDiscountStrategy() = default;
    virtual double calculate(double baseAmount) const = 0;
};

}  // namespace ecommerce_checkout_lld

#endif

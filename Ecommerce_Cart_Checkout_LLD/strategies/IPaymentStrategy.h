#ifndef ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_IPAYMENTSTRATEGY_H
#define ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_IPAYMENTSTRATEGY_H

#include <string>

#include "../models/PaymentResult.h"

namespace ecommerce_checkout_lld {

class IPaymentStrategy {
public:
    virtual ~IPaymentStrategy() = default;
    virtual PaymentResult pay(double amount, const std::string &paymentInstrument) const = 0;
};

}  // namespace ecommerce_checkout_lld

#endif

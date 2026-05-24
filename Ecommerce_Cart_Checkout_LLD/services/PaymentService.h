#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_PAYMENTSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_PAYMENTSERVICE_H

#include <bits/stdc++.h>

#include "../enums/PaymentMethod.h"
#include "../factories/PaymentStrategyFactory.h"
#include "../models/PaymentResult.h"

namespace ecommerce_checkout_lld {

using namespace std;

class PaymentService {
public:
    PaymentResult processPayment(PaymentMethod method,
                                 double amount,
                                 const string &paymentInstrument) const {
        auto strategy = PaymentStrategyFactory::create(method);
        return strategy->pay(amount, paymentInstrument);
    }
};

}  // namespace ecommerce_checkout_lld

#endif

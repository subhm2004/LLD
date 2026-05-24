#ifndef ECOMMERCE_CART_CHECKOUT_LLD_FACTORIES_PAYMENTSTRATEGYFACTORY_H
#define ECOMMERCE_CART_CHECKOUT_LLD_FACTORIES_PAYMENTSTRATEGYFACTORY_H

#include <bits/stdc++.h>
#include <memory>

#include "../enums/PaymentMethod.h"
#include "../strategies/CardPaymentStrategy.h"
#include "../strategies/CodPaymentStrategy.h"
#include "../strategies/IPaymentStrategy.h"
#include "../strategies/UpiPaymentStrategy.h"

namespace ecommerce_checkout_lld {

using namespace std;

class PaymentStrategyFactory {
public:
    static unique_ptr<IPaymentStrategy> create(PaymentMethod method) {
        switch (method) {
            case PaymentMethod::UPI:
                return make_unique<UpiPaymentStrategy>();
            case PaymentMethod::CREDIT_CARD:
                return make_unique<CardPaymentStrategy>();
            case PaymentMethod::COD:
                return make_unique<CodPaymentStrategy>();
        }
        throw invalid_argument("Unsupported payment method");
    }
};

}  // namespace ecommerce_checkout_lld

#endif

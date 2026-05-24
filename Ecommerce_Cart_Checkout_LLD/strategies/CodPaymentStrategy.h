#ifndef ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_CODPAYMENTSTRATEGY_H
#define ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_CODPAYMENTSTRATEGY_H

#include <bits/stdc++.h>

#include "IPaymentStrategy.h"

namespace ecommerce_checkout_lld {

using namespace std;

class CodPaymentStrategy : public IPaymentStrategy {
public:
    PaymentResult pay(double amount, const string &/*paymentInstrument*/) const override {
        if (amount <= 0) {
            return PaymentResult(PaymentStatus::FAILED, "", "Invalid amount");
        }
        const string txnId = "COD_" + to_string(static_cast<long long>(amount * 100));
        return PaymentResult(PaymentStatus::SUCCESS, txnId, "COD order accepted — pay on delivery");
    }
};

}  // namespace ecommerce_checkout_lld

#endif

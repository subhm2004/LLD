#ifndef ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_UPIPAYMENTSTRATEGY_H
#define ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_UPIPAYMENTSTRATEGY_H

#include <bits/stdc++.h>

#include "IPaymentStrategy.h"

namespace ecommerce_checkout_lld {

using namespace std;

class UpiPaymentStrategy : public IPaymentStrategy {
public:
    PaymentResult pay(double amount, const string &paymentInstrument) const override {
        if (amount <= 0) {
            return PaymentResult(PaymentStatus::FAILED, "", "Invalid amount");
        }
        if (paymentInstrument.empty()) {
            return PaymentResult(PaymentStatus::FAILED, "", "UPI ID required");
        }
        const string txnId = "UPI_TXN_" + to_string(static_cast<long long>(amount * 100));
        return PaymentResult(PaymentStatus::SUCCESS, txnId, "Paid via UPI");
    }
};

}  // namespace ecommerce_checkout_lld

#endif

#ifndef ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_CARDPAYMENTSTRATEGY_H
#define ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_CARDPAYMENTSTRATEGY_H

#include <bits/stdc++.h>

#include "IPaymentStrategy.h"

namespace ecommerce_checkout_lld {

using namespace std;

class CardPaymentStrategy : public IPaymentStrategy {
public:
    PaymentResult pay(double amount, const string &paymentInstrument) const override {
        if (amount <= 0) {
            return PaymentResult(PaymentStatus::FAILED, "", "Invalid amount");
        }
        if (paymentInstrument.size() < 4) {
            return PaymentResult(PaymentStatus::FAILED, "", "Invalid card number");
        }
        if (paymentInstrument.rfind("0000") == paymentInstrument.size() - 4) {
            return PaymentResult(PaymentStatus::FAILED, "", "Card declined by bank");
        }
        const string txnId = "CARD_TXN_" + to_string(static_cast<long long>(amount * 100));
        return PaymentResult(PaymentStatus::SUCCESS, txnId, "Paid via credit card");
    }
};

}  // namespace ecommerce_checkout_lld

#endif

#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_PAYMENTRESULT_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_PAYMENTRESULT_H

#include <bits/stdc++.h>

#include "../enums/PaymentStatus.h"

namespace ecommerce_checkout_lld {

using namespace std;

class PaymentResult {
public:
    PaymentResult(PaymentStatus status, string transactionId, string message)
        : status_(status),
          transactionId_(std::move(transactionId)),
          message_(std::move(message)) {}

    PaymentStatus getStatus() const { return status_; }
    const string &getTransactionId() const { return transactionId_; }
    const string &getMessage() const { return message_; }
    bool isSuccess() const { return status_ == PaymentStatus::SUCCESS; }

private:
    PaymentStatus status_{PaymentStatus::FAILED};
    string transactionId_;
    string message_;
};

}  // namespace ecommerce_checkout_lld

#endif

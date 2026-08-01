#ifndef UBER_LLD_MODELS_PAYMENT_H
#define UBER_LLD_MODELS_PAYMENT_H

#include <bits/stdc++.h>

#include "../enums/PaymentMethod.h"
#include "../enums/PaymentStatus.h"

using namespace std;

namespace uber_lld {

class Payment {
public:
    Payment(string paymentId, string rideId, double amount, PaymentMethod method)
        : paymentId_(std::move(paymentId)),
          rideId_(std::move(rideId)),
          amount_(amount),
          method_(method),
          status_(PaymentStatus::INITIATED) {}

    const string &getPaymentId() const { return paymentId_; }
    const string &getRideId() const { return rideId_; }
    double getAmount() const { return amount_; }
    PaymentMethod getMethod() const { return method_; }
    PaymentStatus getStatus() const { return status_; }

    void markSuccess() { status_ = PaymentStatus::SUCCESS; }
    void markFailed() { status_ = PaymentStatus::FAILED; }

private:
    string paymentId_;
    string rideId_;
    double amount_;
    PaymentMethod method_;
    PaymentStatus status_;
};

} // namespace uber_lld

#endif // UBER_LLD_MODELS_PAYMENT_H

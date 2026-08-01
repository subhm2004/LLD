#ifndef UBER_LLD_SERVICES_PAYMENTSERVICE_H
#define UBER_LLD_SERVICES_PAYMENTSERVICE_H

#include <bits/stdc++.h>

#include "../models/Payment.h"

using namespace std;

namespace uber_lld {

class PaymentService {
public:
    Payment *createPayment(const string &rideId, double amount, PaymentMethod method) {
        const string paymentId = "PAY_" + to_string(++paymentCounter_);
        Payment *payment = new Payment(paymentId, rideId, amount, method);
        payments_[paymentId] = payment;
        return payment;
    }

    bool processPayment(Payment *payment) const {
        if (payment->getMethod() == PaymentMethod::CASH) {
            payment->markSuccess();
            return true;
        }

        if (payment->getAmount() <= 0.0) {
            payment->markFailed();
            return false;
        }
        payment->markSuccess();
        return true;
    }

    ~PaymentService() {
        for (auto &entry : payments_) {
            delete entry.second;
        }
    }

private:
    unordered_map<string, Payment *> payments_;
    int paymentCounter_ = 0;
};

} // namespace uber_lld

#endif // UBER_LLD_SERVICES_PAYMENTSERVICE_H

// ============================================================================
//  UpiPaymentStrategy.h — Concrete Strategy #1: UPI se payment
// ----------------------------------------------------------------------------
//  PaymentStrategy ka ek implementation — mobile number lekar UPI pay
//  karta hai. Order ko iske andar ki detail (mobile) se koi matlab nahi —
//  wo bas pay(amount) call karta hai, ye apna kaam kar deta hai.
// ============================================================================
#ifndef UPI_PAYMENT_STRATEGY_H
#define UPI_PAYMENT_STRATEGY_H

#include "PaymentStrategy.h"
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

class UpiPaymentStrategy : public PaymentStrategy {
private:
    string mobile;
public:
    UpiPaymentStrategy(const string& mob) {
        mobile = mob;
    }

    void pay(double amount) override {
        cout << "Paid ₹" << amount << " using UPI (" << mobile << ")" << endl;
    }
};

#endif // UPI_PAYMENT_STRATEGY_H

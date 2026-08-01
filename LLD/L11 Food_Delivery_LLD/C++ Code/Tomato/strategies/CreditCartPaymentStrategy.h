// ============================================================================
//  CreditCartPaymentStrategy.h — Concrete Strategy #2: Credit Card se payment
// ----------------------------------------------------------------------------
//  PaymentStrategy ka doosra implementation — card number lekar pay karta hai.
//  UPI wali strategy ke saath INTERCHANGEABLE hai: Order dono ko ek jaisa
//  treat karta hai (PaymentStrategy*), sirf pay() ka output alag.
//  ⚠️ FILENAME me typo hai: "Cart" (asal me "Card" hona chahiye) — par
//  class ka naam CreditCardPaymentStrategy sahi hai, isliye code chalta hai.
// ============================================================================
#ifndef CREDIT_CARD_PAYMENT_STRATEGY_H
#define CREDIT_CARD_PAYMENT_STRATEGY_H

#include "PaymentStrategy.h"
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

class CreditCardPaymentStrategy : public PaymentStrategy {
private:
    string cardNumber;
public:
    CreditCardPaymentStrategy(const string& card) {
        cardNumber = card;
    }

    void pay(double amount) override {
        cout << "Paid ₹" << amount << " using Credit Card (" << cardNumber << ")" << endl;
    }
};

#endif // CREDIT_CARD_PAYMENT_STRATEGY_H

// ============================================================================
//  PaymentStrategy.h — STRATEGY interface: payment ka "kaise" (L8 pattern!)
// ----------------------------------------------------------------------------
//  Payment ka TAREEKA (UPI/Card/Wallet...) alag-alag ho sakta hai, par
//  Order ko sirf itna pata: "koi pay(amount) karega". Concrete strategy
//  (UpiPaymentStrategy, CreditCardPaymentStrategy) runtime pe inject hoti
//  hai — Order ka koi code nahi badalta.
//  Client (main.cpp) decide karta hai: checkoutNow(..., new UpiPaymentStrategy())
//  Naya mode (PayPal, COD) chahiye? -> nayi class banao, bas. OCP ✅
// ============================================================================
#ifndef PAYMENT_STRATEGY_H
#define PAYMENT_STRATEGY_H

#include <iostream>
#include <string>
using namespace std;

class PaymentStrategy {
public:
    virtual void pay(double amount) = 0;
    virtual ~PaymentStrategy() {}
};

#endif // PAYMENT_STRATEGY_H

// PaymentStrategy.h — Payment ka STRATEGY interface (pay() method). Concrete
// strategies (UPI, CreditCard) ise implement karte hain -> payment mode runtime
// par swap ho sakta hai bina order logic chhede. (Strategy pattern)
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

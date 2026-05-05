#ifndef PAYMENT_GATEWAY_LLD_BANKING_BANKINGSYSTEM_H
#define PAYMENT_GATEWAY_LLD_BANKING_BANKINGSYSTEM_H

#include <bits/stdc++.h>
using namespace std;

namespace payment_gateway_lld {
class BankingSystem {
public:
    virtual bool processPayment(double amount) = 0;
    virtual ~BankingSystem() {}
};

class PaytmBankingSystem : public BankingSystem {
public:
    bool processPayment(double) override {
        int r = rand() % 100;
        return r < 80;
    }
};

class RazorpayBankingSystem : public BankingSystem {
public:
    bool processPayment(double amount) override {
        cout << "[BankingSystem-Razorpay] Processing payment of " << amount << "...\n";
        int r = rand() % 100;
        return r < 90;
    }
};
}

#endif

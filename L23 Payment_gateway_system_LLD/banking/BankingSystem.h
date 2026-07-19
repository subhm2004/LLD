// ============================================================================
//  banking/BankingSystem.h — STRATEGY (L8): actual money-processing backend 🏦
// ----------------------------------------------------------------------------
//  Har gateway ke andar ek BankingSystem hota hai jo asli paisa process
//  karta hai (bank/wallet integration). Ye Strategy pattern hai — gateway
//  ke andar composition se inject hota, swap ho sakta.
//
//  Teeno implementations RANDOM success dete hain (demo ke liye — real me
//  bank API call hoti):
//    PaytmBankingSystem    -> 80% success (r < 80)
//    RazorpayBankingSystem -> 90% success (sabse reliable)
//    PayPalBankingSystem   -> 85% success (international)
//
//  ⭐ Ye random failure hi RETRY ko meaningful banata hai — kabhi pehli
//  attempt fail hoti, proxy retry karta, dusri me pass. Isi liye demo har
//  baar thoda alag output de sakta hai (rand() se).
// ============================================================================
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
    cout << "[BankingSystem-Razorpay] Processing payment of " << amount
         << "...\n";
    int r = rand() % 100;
    return r < 90;
  }
};

class PayPalBankingSystem : public BankingSystem {
public:
  bool processPayment(double amount) override {
    cout << "[BankingSystem-PayPal] Processing international payment of "
         << amount << "...\n";
    int r = rand() % 100;
    return r < 85;
  }
};
} // namespace payment_gateway_lld

#endif

// ============================================================================
//  models/BankAccount.h  —  Bank account (jahan asli paisa pada hai)
// ----------------------------------------------------------------------------
//  Account ke paas ek number aur ek balance hota hai. Iska sabse ahem kaam:
//  `debit()` — balance se paisa surakshit tarike se ghatana.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `debit()` KHUD APNI SURAKSHA karta hai — ye zaroori design hai        │
//  │                                                                          │
//  │  debit() do cheez check karta hai PEHLE, tabhi paisa ghatata:            │
//  │     1. amount > 0        (0 ya negative debit ka matlab nahi)            │
//  │     2. amount <= balance (balance se zyada nikal nahi sakte)             │
//  │  Fail hone par `false` lauta ta hai aur balance ko HAATH tak nahi lagata. │
//  │                                                                          │
//  │  ⭐ Balance kabhi NEGATIVE nahi ho sakta — ye "invariant" (hamesha sach   │
//  │  rehne wala niyam) account khud enforce karta hai. Bahar wala koi galti   │
//  │  se over-debit na kar de, iski guarantee model ke andar hai. Yahi        │
//  │  "encapsulation" hai — data apni safety khud sambhaale.                  │
//  │                                                                          │
//  │  💡 ATMSystem phir bhi debit se PEHLE balance check karta hai (double     │
//  │     safety) — taaki proper error message de sake ("insufficient balance")│
//  │     bina silent false ke. Do jagah check = defense in depth.             │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef ATM_LLD_MODELS_BANKACCOUNT_H
#define ATM_LLD_MODELS_BANKACCOUNT_H

#include <string>
#include <utility>

using namespace std;

namespace atm_lld {

class BankAccount {
public:
    BankAccount(string accountNumber, double balance)
        : accountNumber_(std::move(accountNumber)), balance_(balance) {}

    const string &getAccountNumber() const { return accountNumber_; }
    double getBalance() const { return balance_; }

    // Paisa ghatao — par pehle safety check. Fail to false, balance untouched.
    bool debit(double amount) {
        if (amount <= 0.0 || amount > balance_) {
            return false; // invalid amount ya balance kam -> reject
        }
        balance_ -= amount;
        return true;
    }

private:
    string accountNumber_; // unique account id
    double balance_;       // account me abhi kitna paisa (kabhi negative nahi)
};

} // namespace atm_lld

#endif // ATM_LLD_MODELS_BANKACCOUNT_H

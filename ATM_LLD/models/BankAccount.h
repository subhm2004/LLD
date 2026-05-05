#ifndef ATM_LLD_MODELS_BANKACCOUNT_H
#define ATM_LLD_MODELS_BANKACCOUNT_H

#include <bits/stdc++.h>

using namespace std;

namespace atm_lld {

class BankAccount {
public:
    BankAccount(string accountNumber, double balance)
        : accountNumber_(std::move(accountNumber)), balance_(balance) {}

    const string &getAccountNumber() const { return accountNumber_; }
    double getBalance() const { return balance_; }

    bool debit(double amount) {
        if (amount <= 0.0 || amount > balance_) {
            return false;
        }
        balance_ -= amount;
        return true;
    }

private:
    string accountNumber_;
    double balance_;
};

} // namespace atm_lld

#endif // ATM_LLD_MODELS_BANKACCOUNT_H

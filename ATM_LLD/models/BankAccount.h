#ifndef ATM_LLD_MODELS_BANKACCOUNT_H
#define ATM_LLD_MODELS_BANKACCOUNT_H

#include <bits/stdc++.h>

using namespace std;

namespace atm_lld {

/**
 * @class BankAccount
 * @brief User ke bank account ko represent karti hai.
 * 
 * Isme account number, current balance aur amount debit karne ka logic detailed comment kiya gaya hai.
 */
class BankAccount {
public:
    // Account details initialize karne ke liye constructor.
    BankAccount(string accountNumber, double balance)
        : accountNumber_(std::move(accountNumber)), balance_(balance) {}

    // Account Number fetch karne ke liye getter.
    const string &getAccountNumber() const { return accountNumber_; }
    
    // Current balance check karne ke liye getter.
    double getBalance() const { return balance_; }

    // Account se cash deduct (debit) karne ka core implementation.
    bool debit(double amount) {
        // Balance negative nahi ho sakta aur zero se kam amount debit nahi kar sakte.
        if (amount <= 0.0 || amount > balance_) {
            return false; // Debit request reject.
        }
        balance_ -= amount; // Balance deduct kiya.
        return true; // Debit success.
    }

private:
    string accountNumber_; // Unique account identifier
    double balance_;        // Account main save balance double format me
};

} // namespace atm_lld

#endif // ATM_LLD_MODELS_BANKACCOUNT_H

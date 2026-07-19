#include <bits/stdc++.h>
using namespace std;

// Demo 3: Custom exception — LLD me domain-specific/business errors kaise banate hain

/**
 * @class InsufficientBalanceException
 * @brief Bank account me balance kam hone par throw kiya jane wala custom exception class.
 * 
 * LLD me business limits check karne ke liye custom exceptions banana aachhi practice hai.
 * Hum standard class `runtime_error` se inherit kar rahe hain taaki standard polymorph catch updates support hon.
 */
class InsufficientBalanceException : public runtime_error {
public:
    // Constructor: parent runtime_error class ko generic warning message bhejta hai aur detailed variables save karta hai.
    InsufficientBalanceException(string accountId, double balance, double withdrawAmount)
        : runtime_error("Insufficient balance in account!"),
          accountId_(std::move(accountId)),
          balance_(balance),
          withdrawAmount_(withdrawAmount) {}

    // Getters taaki catch block error states check kar sake.
    const string &getAccountId() const { return accountId_; }
    double getBalance() const { return balance_; }
    double getWithdrawAmount() const { return withdrawAmount_; }

    // Business details generate karne ke liye helper method.
    string details() const {
        return "Account " + accountId_ + " | balance=" + to_string(balance_) +
               " | requested=" + to_string(withdrawAmount_);
    }

private:
    string accountId_;          // Account jisme limit issues aayi
    double balance_;            // Available balance
    double withdrawAmount_;     // Amount jo withdraw karne ki koshish ki gayi
};

/**
 * @class Account
 * @brief Bank Account ki structure ko mock karne wali simple class.
 */
class Account {
public:
    Account(string id, double balance) : id_(std::move(id)), balance_(balance) {}

    // Cash withdraw method.
    void withdraw(double amount) {
        if (amount <= 0) {
            // Validation error: invalid arguments block
            throw invalid_argument("withdraw amount must be positive");
        }
        if (amount > balance_) {
            // Business rule validation failed: Custom business error trigger.
            throw InsufficientBalanceException(id_, balance_, amount);
        }
        balance_ -= amount;
        cout << "Withdraw OK. New balance: " << balance_ << "\n";
    }

private:
    string id_;
    double balance_;
};

int main() {
    cout << "=== 03 Custom exception ===\n";

    Account acc("ACC_101", 500.0);

    try {
        acc.withdraw(200); // 200 withdraw ho jayega, balance = 300 bachega.
        acc.withdraw(400); // 400 withdraw karne par custom exception throw hoga.
    } 
    // Pehle specific child / custom exception handle karenge.
    catch (const InsufficientBalanceException &ex) {
        cout << "Business error: " << ex.what() << "\n";
        cout << "Details: " << ex.details() << "\n";
    } 
    // Generic validation errors ko doosre block me catch karenge.
    catch (const invalid_argument &ex) {
        cout << "Validation error: " << ex.what() << "\n";
    }

    return 0;
}

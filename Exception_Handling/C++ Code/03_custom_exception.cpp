#include <bits/stdc++.h>
using namespace std;

// Demo 3: Custom exception — LLD me domain-specific errors

class InsufficientBalanceException : public runtime_error {
public:
    InsufficientBalanceException(string accountId, double balance, double withdrawAmount)
        : runtime_error("Insufficient balance"),
          accountId_(std::move(accountId)),
          balance_(balance),
          withdrawAmount_(withdrawAmount) {}

    const string &getAccountId() const { return accountId_; }
    double getBalance() const { return balance_; }
    double getWithdrawAmount() const { return withdrawAmount_; }

    string details() const {
        return "Account " + accountId_ + " | balance=" + to_string(balance_) +
               " | requested=" + to_string(withdrawAmount_);
    }

private:
    string accountId_;
    double balance_;
    double withdrawAmount_;
};

class Account {
public:
    Account(string id, double balance) : id_(std::move(id)), balance_(balance) {}

    void withdraw(double amount) {
        if (amount <= 0) {
            throw invalid_argument("withdraw amount must be positive");
        }
        if (amount > balance_) {
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
        acc.withdraw(200);
        acc.withdraw(400); // throws InsufficientBalanceException
    } catch (const InsufficientBalanceException &ex) {
        cout << "Business error: " << ex.what() << "\n";
        cout << ex.details() << "\n";
    } catch (const invalid_argument &ex) {
        cout << "Validation error: " << ex.what() << "\n";
    }

    return 0;
}

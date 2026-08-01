// ============================================================================
//  03_custom_exception.cpp  —  Apna khud ka exception banao (domain errors)
// ----------------------------------------------------------------------------
//  Build: g++ -std=c++17 -Wall -Wextra "C++ Code/03_custom_exception.cpp" -o bin/03_custom_exception
//
//  Standard exceptions (runtime_error, invalid_argument) generic hain. LLD me
//  aksar hume BUSINESS-specific errors chahiye jo extra data bhi carry karein
//  (jaise "insufficient balance" — kaunsa account, kitna balance, kitna maanga).
//  Iske liye apna custom exception banate hain.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ CUSTOM EXCEPTION KO STANDARD SE INHERIT KARO — ye zaroori hai         │
//  │                                                                          │
//  │  `class InsufficientBalanceException : public runtime_error`             │
//  │                                                                          │
//  │  Kyun runtime_error se? Taaki jo code `catch (const std::exception&)` ya  │
//  │  `catch (const runtime_error&)` likhta hai, wo hamare custom exception ko │
//  │  bhi AAP HI pakad le (polymorphism). Agar hum kisi cheez se inherit na    │
//  │  karte, to sirf hamara exact type hi ise catch kar paata — flexibility gayi.│
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ EXTRA DATA carry karna — custom exception ka asli fayda:
//     Base ko ek generic message dete hain (`runtime_error("...")` -> what()),
//     aur apne EXTRA fields (accountId, balance, amount) khud store karte hain
//     + getters dete hain. Catch block in structured details ko padh ke smart
//     decision le sakta hai (sirf string message se zyada).
//
//  📌 Catch ORDER: specific/custom exception PEHLE, generic BAAD me (file 04
//     me iski poori wajah hai). Isi liye main me InsufficientBalance pehle,
//     invalid_argument baad me.
// ============================================================================
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

// ---- Custom business exception (runtime_error se inherit) -------------------
class InsufficientBalanceException : public runtime_error {
public:
    // Base ko generic message + apne extra fields save karo.
    InsufficientBalanceException(string accountId, double balance, double withdrawAmount)
        : runtime_error("Insufficient balance in account!"), // what() isi ko dega
          accountId_(std::move(accountId)),
          balance_(balance),
          withdrawAmount_(withdrawAmount) {}

    // ⭐ Getters — catch block structured error data padh sake (sirf message nahi).
    const string &getAccountId() const { return accountId_; }
    double getBalance() const { return balance_; }
    double getWithdrawAmount() const { return withdrawAmount_; }

    // Ek readable detail line banane wala helper.
    string details() const {
        return "Account " + accountId_ + " | balance=" + to_string(balance_) +
               " | requested=" + to_string(withdrawAmount_);
    }

private:
    string accountId_;      // kis account me problem
    double balance_;        // available balance
    double withdrawAmount_; // jitna nikalne ki koshish ki
};

// ---- Simple bank account (custom exception ka user) ------------------------
class Account {
public:
    Account(string id, double balance) : id_(std::move(id)), balance_(balance) {}

    void withdraw(double amount) {
        if (amount <= 0) {
            // Generic validation error -> standard invalid_argument kaafi hai.
            throw invalid_argument("withdraw amount must be positive");
        }
        if (amount > balance_) {
            // ⭐ Business rule toota -> custom exception (extra data ke saath).
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
        acc.withdraw(200); // OK -> balance 300
        acc.withdraw(400); // 400 > 300 -> custom exception throw
    }
    // ⭐ Specific/custom PEHLE — taaki iske extra details (getBalance/details) use kar sakein.
    catch (const InsufficientBalanceException &ex) {
        cout << "Business error: " << ex.what() << "\n";
        cout << "Details: " << ex.details() << "\n"; // structured data ka fayda
    }
    // Generic validation BAAD me.
    catch (const invalid_argument &ex) {
        cout << "Validation error: " << ex.what() << "\n";
    }

    return 0;
}

// Expected output:
//   Withdraw OK. New balance: 300
//   Business error: Insufficient balance in account!
//   Details: Account ACC_101 | balance=300.000000 | requested=400.000000

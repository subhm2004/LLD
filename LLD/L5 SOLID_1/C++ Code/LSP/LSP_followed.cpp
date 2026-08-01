// ============================================================================
//  LSP_followed.cpp  —  Liskov Substitution Principle (LSP) ka SAHI tareeka
// ----------------------------------------------------------------------------
//  Fix: ek hi moti `Account` (deposit + withdraw) ki jagah hierarchy ko
//  CAPABILITY ke hisaab se tod diya:
//     DepositOnlyAccount      -> sirf deposit
//        └─ WithdrawableAccount -> deposit + withdraw
//  Ab FixedTermAccount sirf DepositOnlyAccount hai (withdraw ka jhootha
//  promise hi nahi). Koi bhi subtype apne base ki jagah safely use ho sakta
//  hai -> LSP follow hota hai. Koi exception-throw wala hack nahi.
// ============================================================================

#include <iostream>
#include <vector>
#include <typeinfo>
#include <stdexcept>

using namespace std;

// Sabse upar wali abstraction — har account kam se kam deposit to karta hi hai.
class DepositOnlyAccount {
public:
    virtual void deposit(double amount) = 0;
};

// Jo accounts withdraw bhi karte hain — woh deposit-only ko extend karte hain.
class WithdrawableAccount : public DepositOnlyAccount {
public:
    virtual void withdraw(double amount) = 0;
};

// SavingAccount = withdrawable (deposit + withdraw dono valid hai).
class SavingAccount : public WithdrawableAccount {
private:
    double balance;

public:
    SavingAccount() {
        balance = 0;
    }

    void deposit(double amount) {
        balance += amount;
        cout << "Deposited: " << amount << " in Savings Account. New Balance: " << balance << endl;
    }

    void withdraw(double amount) {
        if (balance >= amount) {
            balance -= amount;
            cout << "Withdrawn: " << amount << " from Savings Account. New Balance: " << balance << endl;
        } else {
            cout << "Insufficient funds in Savings Account!\n";
        }
    }
};

// CurrentAccount = withdrawable bhi.
class CurrentAccount : public WithdrawableAccount {
    private:
        double balance;

    public:
        CurrentAccount() {
            balance = 0;
        }

        void deposit(double amount) {
            balance += amount;
            cout << "Deposited: " << amount << " in Current Account. New Balance: " << balance << endl;
        }

        void withdraw(double amount) {
            if (balance >= amount) {
                balance -= amount;
                cout << "Withdrawn: " << amount << " from Current Account. New Balance: " << balance << endl;
            } else {
                cout << "Insufficient funds in Current Account!\n";
            }
        }
    };

// ✅ FixedTermAccount sirf DepositOnlyAccount hai -> withdraw() ka existence hi
//    nahi, isliye koi jhootha promise ya exception nahi. Yahi sahi modeling hai.
class FixedTermAccount : public DepositOnlyAccount {
private:
    double balance;

public:
    FixedTermAccount() {
        balance = 0;
    }

    void deposit(double amount) {
        balance += amount;
        cout << "Deposited: " << amount << " in Fixed Term Account. New Balance: " << balance << endl;
    }
};

// Client ab capability ke hisaab se do alag lists rakhta hai — type-safe.
class BankClient {
private:
    vector<WithdrawableAccount*> withdrawableAccounts; // inpe withdraw safe hai
    vector<DepositOnlyAccount*> depositOnlyAccounts;   // inpe sirf deposit

public:
    BankClient( vector<WithdrawableAccount*> withdrawableAccounts,
        vector<DepositOnlyAccount*> depositOnlyAccounts) {
        this->withdrawableAccounts = withdrawableAccounts;
        this->depositOnlyAccounts = depositOnlyAccounts;
    }

    void processTransactions() {
        // Sirf withdrawable accounts pe withdraw call hota hai -> kabhi crash nahi.
        for (WithdrawableAccount* acc : withdrawableAccounts) {
            acc->deposit(1000);
            acc->withdraw(500);
        }
        // Deposit-only accounts pe sirf deposit.
        for (DepositOnlyAccount* acc : depositOnlyAccounts) {
            acc->deposit(5000);
        }
    }
};

int main() {
    vector<WithdrawableAccount*> withdrawableAccounts;
    withdrawableAccounts.push_back(new SavingAccount());
    withdrawableAccounts.push_back(new CurrentAccount());

    vector<DepositOnlyAccount*> depositOnlyAccounts;
    depositOnlyAccounts.push_back(new FixedTermAccount());

    BankClient* client = new BankClient (withdrawableAccounts, depositOnlyAccounts);
    client->processTransactions();

    return 0;
}

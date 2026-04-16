#include <bits/stdc++.h>

using namespace std;

// Class Invariant of a parent class Object should not be broken by child class Object.
// Hence child class can either maintain or strengthen the invariant but never narrows it down.

// Invariant : Balance cannot be negative
class BankAccount
{
protected:
    double balance;

public:
    BankAccount(double b)
    {
        if (b < 0)
            throw invalid_argument("Balance can't be negative");
        balance = b;
    }
    virtual void withdraw(double amount)
    {
        if (balance - amount < 0)
            throw runtime_error("Insufficient funds");
        balance -= amount;
        cout << "Amount withdrawn. Remaining balance is " << balance << endl;
    }
};

// Brakes invariant : Should not be allowed.
class CheatAccount : public BankAccount
{
public:
    CheatAccount(double b) : BankAccount(b) {}

    void withdraw(double amount) override
    {
        balance -= amount; // LSP break! Negative balance allowed
        cout << "Amount withdrawn. Remaining balance is " << balance << endl;
    }
};

int main()
{
    BankAccount *bankAccount = new BankAccount(100);
    bankAccount->withdraw(100);

    CheatAccount *cheat_account = new CheatAccount(100);
    cheat_account->withdraw(200); // property break ho gyi hai
}

/*
├── std::logic_error        <-- For logical errors detected before runtime
│   ├── std::invalid_argument   <-- Invalid function argument
│   ├── std::domain_error       <-- Function argument domain error
│   ├── std::length_error       <-- Exceeding valid length limits
│   ├── std::out_of_range       <-- Array or container index out of bounds
│
├── std::runtime_error      <-- For errors that occur at runtime
│   ├── std::range_error        <-- Numeric result out of range
│   ├── std::overflow_error     <-- Arithmetic overflow
│   ├── std::underflow_error
*/

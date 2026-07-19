/*
=====================================================================
OVERUSE OF GETTER/SETTERS ANTI-PATTERN
=====================================================================
THEORY:
Encapsulation ka poora point hota hai data ko protect karna aur
sirf controlled tarike se access dena. Lekin agar hum har private
field ke liye bina soche-samjhe getter/setter bana dete hain
(bina kisi validation ke), to encapsulation ka fayda hi khatam
ho jata hai kyunki field practically public jaisa ho jata hai.

PROBLEM:
- Data integrity maintain nahi hoti (koi bhi kuch bhi value set
  kar sakta hai, jaise negative balance!)
- Business rules bypass ho jate hain
- Object apne aap ki state protect nahi kar pata

SOLUTION APPROACH:
- Setter me proper validation logic daalo
- Jahan zaroorat na ho, wahan setter hi mat banao (read-only rakho)
- Behavior-driven methods banao (jaise deposit, withdraw) instead
  of raw setBalance()
=====================================================================
*/

#include <iostream>
using namespace std;

// ---------------------------------------------------------------
// ANTI-PATTERN VERSION: bina validation ke direct setter
// ---------------------------------------------------------------
class BankAccount
{
private:
    double balance;

public:
    BankAccount(double initialBalance) : balance(initialBalance) {}

    // Anti-Pattern: koi validation nahi, seedha private member ko
    // set karne de rahe hain - koi bhi negative balance set kar sakta hai
    void setBalance(double b)
    {
        balance = b;
    }

    double getBalance()
    {
        return balance;
    }
};

// ---------------------------------------------------------------
// FIXED VERSION: behavior-driven methods with validation
// ---------------------------------------------------------------
class BankAccountFixed
{
private:
    double balance;

public:
    BankAccountFixed(double initialBalance)
    {
        // Constructor me bhi validation daalna zaroori hai
        balance = (initialBalance >= 0) ? initialBalance : 0;
    }

    // deposit karte waqt amount positive honi chahiye
    void deposit(double amount)
    {
        if (amount <= 0)
        {
            cout << "Invalid deposit amount!" << endl;
            return;
        }
        balance += amount;
        cout << "Deposited Rs. " << amount << ", New Balance: " << balance << endl;
    }

    // withdraw karte waqt balance check karna zaroori hai
    void withdraw(double amount)
    {
        if (amount <= 0)
        {
            cout << "Invalid withdraw amount!" << endl;
            return;
        }
        if (amount > balance)
        {
            cout << "Insufficient balance!" << endl;
            return;
        }
        balance -= amount;
        cout << "Withdrew Rs. " << amount << ", New Balance: " << balance << endl;
    }

    // getter rakhna theek hai, lekin setter nahi - balance sirf
    // deposit/withdraw ke through hi change honi chahiye
    double getBalance()
    {
        return balance;
    }
};

int main()
{
    cout << "--- Getter/Setter Overuse (Anti-Pattern) Demo ---" << endl;
    BankAccount account(1000);
    account.setBalance(-500); // ye galat hai, lekin allow ho raha hai!
    cout << "Balance after invalid set: " << account.getBalance() << endl;

    cout << "\n--- Fixed Version (Validated Behavior Methods) Demo ---" << endl;
    BankAccountFixed accountFixed(1000);
    accountFixed.deposit(500);
    accountFixed.withdraw(2000); // insufficient balance, reject ho jayega
    accountFixed.withdraw(300);

    return 0;
}
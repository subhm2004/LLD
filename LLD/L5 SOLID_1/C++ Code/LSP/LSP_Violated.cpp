// ============================================================================
//  LSP_Violated.cpp  —  Liskov Substitution Principle (LSP) ka VIOLATION
// ----------------------------------------------------------------------------
//  LSP: subtype ko base type ki jagah bina kuch toote use kar paana chahiye.
//  Yahan base `Account` me withdraw() declare hai, par FixedTermAccess withdraw
//  support hi nahi karta -> woh exception throw karta hai. Matlab jo code
//  `Account*` ke through withdraw karega woh FixedTermAccount par CRASH karega.
//  Yeh substitution toot gaya -> LSP break. Fix: LSP_followed.cpp.
// ============================================================================

#include <bits/stdc++.h>
using namespace std;

// Base abstraction — maan liya gaya ki HAR account deposit + withdraw dono karega.
class Account
{
public:
    virtual void deposit(double amount) = 0;
    virtual void withdraw(double amount) = 0; // <- yahi galat assumption hai
};

// SavingAccount: deposit + withdraw dono sahi se support karta hai.
class SavingAccount : public Account
{
private:
    double balance;

public:
    SavingAccount()
    {
        balance = 0;
    }

    void deposit(double amount)
    {
        balance += amount;
        cout << "Deposited: " << amount << " in Savings Account. New Balance: " << balance << endl;
    }

    void withdraw(double amount)
    {
        if (balance >= amount)
        {
            balance -= amount;
            cout << "Withdrawn: " << amount << " from Savings Account. New Balance: " << balance << endl;
        }
        else
        {
            cout << "Insufficient funds in Savings Account!\n";
        }
    }
};

// CurrentAccount: bhi deposit + withdraw dono support karta hai.
class CurrentAccount : public Account
{
private:
    double balance;

public:
    CurrentAccount()
    {
        balance = 0;
    }

    void deposit(double amount)
    {
        balance += amount;
        cout << "Deposited: " << amount << " in Current Account. New Balance: " << balance << endl;
    }

    void withdraw(double amount)
    {
        if (balance >= amount)
        {
            balance -= amount;
            cout << "Withdrawn: " << amount << " from Current Account. New Balance: " << balance << endl;
        }
        else
        {
            cout << "Insufficient funds in Current Account!\n";
        }
    }
};

// ❌ FixedTermAccount: ismein withdrawal allowed hi NAHI hai.
//    Par base ki wajah se withdraw() override karna majboori hai -> exception
//    throw karta hai. Yahi LSP violation ka asli point hai.
class FixedTermAccount : public Account
{
private:
    double balance;

public:
    FixedTermAccount()
    {
        balance = 0;
    }

    void deposit(double amount)
    {
        balance += amount;
        cout << "Deposited: " << amount << " in Fixed Term Account. New Balance: " << balance << endl;
    }

    void withdraw(double amount)
    {
        // Base ne promise kiya tha "withdraw chalega", par yahan hum tod rahe hain.
        throw logic_error("Withdrawal not allowed in Fixed Term Account!");
    }
};

// Client jo sirf base `Account` jaanta hai aur sabpe withdraw maan ke chalta hai.
class BankClient
{
private:
    vector<Account *> accounts;

public:
    BankClient(vector<Account *> accounts)
    {
        this->accounts = accounts;
    }

    void processTransactions()
    {
        for (Account *acc : accounts)
        {
            acc->deposit(1000); // deposit sab pe chalta hai (yahan tak theek)

            // Maan liya sab withdraw support karte hain -> FixedTermAccount par exception.
            try
            {
                acc->withdraw(500);
            }
            catch (const logic_error &e)
            {
                cout << "Exception: " << e.what() << endl;
            }
        }
    }
};

int main()
{
    vector<Account *> accounts;
    accounts.push_back(new SavingAccount());
    accounts.push_back(new CurrentAccount());
    accounts.push_back(new FixedTermAccount());

    BankClient *client = new BankClient(accounts);
    client->processTransactions(); // FixedTermAccount pe withdraw karte hi exception aata hai

    return 0;
}

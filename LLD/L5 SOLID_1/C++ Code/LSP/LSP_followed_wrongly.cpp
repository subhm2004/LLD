// ============================================================================
//  LSP_followed_wrongly.cpp  —  LSP "fix" jo asal me GALAT hai (subtle trap)
// ----------------------------------------------------------------------------
//  Yeh code dikhta hai ki LSP problem solve ho gaya, par actually NAHI hua.
//  Hierarchy abhi bhi wahi galat hai (FixedTermAccount.withdraw() throw karta
//  hai). Bas client ne `typeid` se runtime pe TYPE CHECK laga diya:
//      "agar FixedTermAccount hai to withdraw skip kar do".
//  Yeh anti-pattern hai kyunki:
//    1) Client ko har subtype ka pata hona chahiye (tight coupling).
//    2) Naya deposit-only account aaya to client ka if-else phir badlega.
//    3) Polymorphism ka faida khatam.
//  Sahi fix LSP_followed.cpp wala hai (capability-based interfaces).
// ============================================================================

#include <bits/stdc++.h>

using namespace std;

// Base abstraction abhi bhi galat — deposit + withdraw dono maan raha hai.
class Account
{
public:
    virtual void deposit(double amount) = 0;
    virtual void withdraw(double amount) = 0;
};

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

// FixedTermAccount abhi bhi withdraw() pe exception throw karta hai (root problem unsolved).
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
        throw logic_error("Withdrawal not allowed in Fixed Term Account!");
    }
};

// Client class — yahi par galti chhupi hai.
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
            acc->deposit(1000);

            // ❌ Yahan client ko concrete type check karna pad raha hai.
            //    Yeh "if subtype is X then special-case" -> LSP ka asli ulta.
            if (typeid(*acc) == typeid(FixedTermAccount))
            {
                cout << "Skipping withdrawal for Fixed Term Account.\n";
            }
            else
            {
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
    }
};

int main()
{
    vector<Account *> accounts;
    accounts.push_back(new SavingAccount());
    accounts.push_back(new CurrentAccount());
    accounts.push_back(new FixedTermAccount());

    BankClient *client = new BankClient(accounts);
    client->processTransactions();

    return 0;
}

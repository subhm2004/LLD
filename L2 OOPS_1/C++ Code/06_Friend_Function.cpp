/**
 * L2 — friend function / friend class: breaks encapsulation selectively
 * NOT part of class; can access private members
 */
#include <iostream>
using namespace std;

class BankAccount {
    double balance;

    friend void printBalance(const BankAccount& acc);  // friend declaration

public:
    BankAccount(double b) : balance(b) {}
};

// Friend function — not a member
void printBalance(const BankAccount& acc) {
    cout << "Friend can read private balance: " << acc.balance << "\n";
}

class SecretHolder {
    int secret = 42;
    friend class Auditor;
};

class Auditor {
public:
    void inspect(const SecretHolder& h) {
        cout << "Friend class sees secret: " << h.secret << "\n";
    }
};

int main() {
    BankAccount acc(5000);
    printBalance(acc);

    SecretHolder h;
    Auditor a;
    a.inspect(h);
    return 0;
}

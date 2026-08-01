// ============================================================================
//  06_Friend_Function.cpp  —  Friend function / friend class
// ----------------------------------------------------------------------------
//  `friend` = ek class kisi BAHARI function/class ko apne PRIVATE members tak
//  access dene ki permission deti hai. Friend class ka member NAHI hota (na uske
//  paas `this` hota), par private data padh/likh sakta hai. Encapsulation ko
//  SELECTIVELY ttodta hai — soch-samajh ke use karo (jaise operator<< ya tightly
//  related helper classes ke liye).
// ============================================================================
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

#include <bits/stdc++.h>

#include "core/ATMSystem.h"

using namespace std;
using namespace atm_lld;

/**
 * @file main.cpp
 * @brief ATM System LLD execution flow test runner.
 * 
 * Hinglish comments:
 * - Is script me hum basic transactions verify kar rahe hain. 
 *   Pehle user register karenge, session trigger karenge (login), balance inquiry run karenge aur 
 *   CashDispenser check validation withdrawal execute karenge.
 */

int main() {
    cout << "====== ATM LLD System Demo ======\n\n";

    // ATM System Facade instanciate kiya
    ATMSystem *atm = new ATMSystem();

    // Accounts aur corresponding cards configuration map settings set
    atm->addAccount(BankAccount("ACC_1001", 25000.0));
    atm->addCard(Card("CARD_001", "1234", "ACC_1001"));

    // Case 1: Active session validation login trigger verification.
    if (!atm->login("CARD_001", "1234")) {
        cout << "Authentication failed: Invalid credentials / PIN match errors.\n";
        delete atm;
        return 0;
    }

    cout << "Login success: Active card session initialized.\n";
    
    // Case 2: Balance enquiry.
    cout << "Current Account Balance: " << atm->checkBalance() << " INR\n";

    // Case 3: Cash withdrawal (2300 INR request).
    // ATM rules greedy split logic: 500, 200 aur 100 notes inventory optimization.
    try {
        map<int, int> dispensedNotes = atm->withdrawCash(2300);
        cout << "\nWithdraw successful. Notes dispensed:\n";
        for (const auto &entry : dispensedNotes) {
            cout << "  " << entry.first << " INR Note x " << entry.second << "\n";
        }
    } 
    catch (const exception &ex) {
        cout << "Withdraw failed: " << ex.what() << "\n";
    }

    // Case 4: Balance query check post deductions.
    cout << "\nBalance after withdrawal: " << atm->checkBalance() << " INR\n";
    
    // Active session logout cleanup
    atm->logout();
    delete atm;
    
    cout << "\nDemo complete.\n";
    return 0;
}

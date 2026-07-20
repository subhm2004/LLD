// ============================================================================
//  main.cpp  —  ATM System ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  Ye ek chhota flow chala ke dikhata hai: account+card banao, login karo,
//  balance dekho, cash nikaalo, phir balance dobara dekho, logout.
//
//  ⭐ CLIENT KITNA KAM JAANTA HAI (Facade ka fayda):
//     Poore system me AuthenticationService, CashDispenser, BankAccount, Card,
//     Transaction sab hain — par yahan sirf `ATMSystem` dikhta hai. Client ko
//     PIN-check ya notes-algorithm ka kuch pata nahi; wo bas 4 method bulata hai:
//     login, checkBalance, withdrawCash, logout.
//
//  ============================================================================
//   DEMO PLAN — kaunsa case kaunsi requirement dikhata hai
//  ----------------------------------------------------------------------------
//     Setup -> account + card configure
//     Case 1 -> #1  Login (card + PIN authentication)
//     Case 2 -> #2  Balance inquiry
//     Case 3 -> #3,#5 Cash withdrawal + notes by denomination
//     Case 4 -> #2  Withdrawal ke baad balance (deduction verify)
//  ============================================================================
#include <exception>
#include <iostream>
#include <map>

#include "core/ATMSystem.h"

using namespace std;
using namespace atm_lld;

int main() {
    cout << "====== ATM LLD System Demo ======\n\n";

    // Facade banaya (heap pe — end me delete karenge).
    ATMSystem *atm = new ATMSystem();

    // ---- Setup: ek account + us par ek card ---------------------------------
    atm->addAccount(BankAccount("ACC_1001", 25000.0));       // 25,000 INR balance
    atm->addCard(Card("CARD_001", "1234", "ACC_1001"));      // card -> PIN 1234 -> account

    // ---- Case 1: LOGIN (R1) — galat credentials pe yahin ruk jao ------------
    if (!atm->login("CARD_001", "1234")) {
        cout << "Authentication failed: galat card/PIN.\n";
        delete atm;
        return 0;
    }
    cout << "Login success: active card session shuru.\n";

    // ---- Case 2: BALANCE INQUIRY (R2) ---------------------------------------
    cout << "Current Account Balance: " << atm->checkBalance() << " INR\n";

    // ---- Case 3: CASH WITHDRAWAL (R3, R5) -----------------------------------
    // 2300 maanga. Backtracking dispenser sahi notes chunega (500x4 + 200x1 + 100x1).
    try {
        map<int, int> dispensedNotes = atm->withdrawCash(2300);
        cout << "\nWithdraw successful. Notes dispensed:\n";
        // `map<int,int>` ascending order me print hota hai (100, 200, 500).
        for (const auto &entry : dispensedNotes) {
            cout << "  " << entry.first << " INR Note x " << entry.second << "\n";
        }
    } catch (const exception &ex) {
        cout << "Withdraw failed: " << ex.what() << "\n";
    }

    // ---- Case 4: BALANCE post-withdrawal (R2) — deduction verify ------------
    cout << "\nBalance after withdrawal: " << atm->checkBalance() << " INR\n";

    // ---- Session cleanup ----------------------------------------------------
    atm->logout();
    delete atm; // heap object saaf (koi leak na rahe)

    cout << "\nDemo complete.\n";
    return 0;
}

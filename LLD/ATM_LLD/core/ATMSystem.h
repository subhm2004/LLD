// ============================================================================
//  core/ATMSystem.h  —  Poore ATM ka FACADE (single entry point)
// ----------------------------------------------------------------------------
//  Ye class hi client (main.cpp / ATM screen) ke liye "ATM" hai. Andar
//  AuthenticationService, CashDispenser, BankAccount, Card, Transaction — sab
//  ka tamasha chalta hai, par bahar se client ko sirf 4 saaf method dikhte hain:
//  login, checkBalance, withdrawCash, logout.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FACADE PATTERN — subsystem ki complexity client se chhupao           │
//  │                                                                          │
//  │  Client ko ye NAHI pata:                                                 │
//  │    - PIN kaise verify hoti hai       (AuthenticationService)             │
//  │    - notes kaise chune jaate hain    (CashDispenser — backtracking)      │
//  │    - balance kaise ghatta hai        (BankAccount.debit)                 │
//  │    - transaction kaise log hoti hai  (Transaction)                       │
//  │  Wo bas `atm->withdrawCash(2300)` bulata hai; facade andar sahi service   │
//  │  ko sahi order me chalata hai. Facade = orchestrator (conductor).        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SESSION STATE — `currentCardNumber_` (simplified State idea)          │
//  │                                                                          │
//  │  ATM do haalat me hota hai: "card daala nahi" (koi session nahi) ya       │
//  │  "card daala hai" (active session). Ye ek string variable se track hota:  │
//  │     login()  -> currentCardNumber_ set  (session ON)                     │
//  │     logout() -> currentCardNumber_ clear (session OFF)                    │
//  │  Har balance/withdraw se pehle getCurrentAccountOrThrow() check karta hai │
//  │  ki session ON hai ya nahi — bina card daale kuch nahi ho sakta.         │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ FACADE khud saara data OWN karta hai (accounts, cards, transactions) aur
//     services stateless helpers hain. Single source of truth ek jagah (yahan).
//
//  📌 AUDIT: har action (success ho ya fail) ek Transaction log karta hai —
//     createTransaction() se. Fail bhi log hota hai (TransactionStatus.h note dekho).
// ============================================================================
#ifndef ATM_LLD_CORE_ATMSYSTEM_H
#define ATM_LLD_CORE_ATMSYSTEM_H

#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/BankAccount.h"
#include "../models/Card.h"
#include "../models/Transaction.h"
#include "../services/AuthenticationService.h"
#include "../services/CashDispenser.h"

using namespace std;

namespace atm_lld {

class ATMSystem {
public:
    // ---- Setup: account aur card register (insert_or_assign se dobara set safe) ----
    void addAccount(const BankAccount &account) {
        auto it = accounts_.find(account.getAccountNumber());
        if (it != accounts_.end()) {
            it->second = account; // pehle se hai -> update
        } else {
            accounts_.emplace(account.getAccountNumber(), account);
        }
    }

    void addCard(const Card &card) {
        auto it = cards_.find(card.getCardNumber());
        if (it != cards_.end()) {
            it->second = card;
        } else {
            cards_.emplace(card.getCardNumber(), card);
        }
    }

    // ========================================================================
    //  LOGIN — card + PIN se session shuru (Requirement #1)
    // ========================================================================
    bool login(const string &cardNumber, const string &pin) {
        Card *card = getCard(cardNumber);
        if (card == nullptr) {
            return false; // aisa card register hi nahi hai
        }
        // ⭐ PIN check khud nahi karte — AuthenticationService ko DELEGATE.
        if (!authService_.authenticate(*card, pin)) {
            return false; // galat PIN
        }
        currentCardNumber_ = cardNumber; // ⭐ session ON
        return true;
    }

    // Session band — card nikaal liya.
    void logout() {
        currentCardNumber_.clear(); // session OFF
    }

    // ========================================================================
    //  BALANCE INQUIRY (Requirement #2)
    // ========================================================================
    double checkBalance() {
        BankAccount *account = getCurrentAccountOrThrow(); // session valid? account milा?
        createTransaction(TransactionType::BALANCE_INQUIRY, 0.0, true); // audit log (amount 0)
        return account->getBalance();
    }

    // ========================================================================
    //  ⭐ CASH WITHDRAWAL (Requirement #3, #4, #5) — is file ka main flow
    // ========================================================================
    //  Do teen "gates" ek-ek karke paar karo, tabhi paisa milega. Har fail par
    //  ek FAILED transaction bhi log hoti hai (audit).
    map<int, int> withdrawCash(int amount) {
        // Gate 0: amount valid ho
        if (amount <= 0) {
            createTransaction(TransactionType::CASH_WITHDRAWAL, amount, false);
            throw runtime_error("Withdraw amount invalid/zero nahi ho sakti.");
        }

        BankAccount *account = getCurrentAccountOrThrow(); // session check

        // Gate 1: account me itna balance hai? (#4 balance validation)
        if (account->getBalance() < amount) {
            createTransaction(TransactionType::CASH_WITHDRAWAL, amount, false);
            throw runtime_error("Account me insufficient balance hai.");
        }

        // Gate 2: ATM ke paas in notes ka combination hai? (#4 notes validation)
        // (Ye backtracking check hai — greedy wali jhooti "na" nahi. CashDispenser.h dekho.)
        if (!cashDispenser_.canDispense(amount)) {
            createTransaction(TransactionType::CASH_WITHDRAWAL, amount, false);
            throw runtime_error("ATM dispenser me requested notes ka combination available nahi hai.");
        }

        // ⭐ ORDER MAAYNE RAKHTA HAI: pehle account debit, phir notes dispense.
        // (Dono checks upar paas ho chuke, to yahan dono safely honge. Balance
        //  ghata, phir physical notes nikale — #5 dispense by denomination.)
        account->debit(amount);
        map<int, int> notes = cashDispenser_.dispense(amount);

        createTransaction(TransactionType::CASH_WITHDRAWAL, amount, true); // success log
        return notes;
    }

private:
    unordered_map<string, BankAccount> accounts_; // accountNumber -> account
    unordered_map<string, Card> cards_;           // cardNumber -> card
    vector<Transaction> transactions_;            // saare transactions ka audit log
    AuthenticationService authService_;           // PIN verify helper
    CashDispenser cashDispenser_;                 // notes vault + dispense algo
    string currentCardNumber_;                    // ⭐ active session (khaali = koi session nahi)
    int transactionCounter_ = 0;                  // unique transaction id ke liye

    // Card dhoondho — na mile to nullptr (login me handle hota hai).
    Card *getCard(const string &cardNumber) {
        auto it = cards_.find(cardNumber);
        if (it == cards_.end()) {
            return nullptr;
        }
        return &it->second;
    }

    // ========================================================================
    //  ⭐ SESSION GUARD — har protected action ka "darwaaza"
    // ========================================================================
    //  Ye teen cheez check karta hai, phir hi active account deta hai:
    //    1. session ON hai? (currentCardNumber_ khaali to nahi)
    //    2. us card ka record milta hai?
    //    3. us card ka linked account milta hai?
    //  Kisi bhi step pe gadbad -> saaf error throw (silent nullptr nahi).
    BankAccount *getCurrentAccountOrThrow() {
        if (currentCardNumber_.empty()) {
            throw runtime_error("Error: Koi active session nahi. Pehle card daalo (login).");
        }
        Card *card = getCard(currentCardNumber_);
        if (card == nullptr) {
            throw runtime_error("Error: Active session ka card record nahi mila.");
        }
        auto accountIt = accounts_.find(card->getAccountNumber());
        if (accountIt == accounts_.end()) {
            throw runtime_error("Error: Card se linked bank account nahi mila.");
        }
        return &accountIt->second;
    }

    // Har action ka audit record banao (success/fail dono log hote hain).
    void createTransaction(TransactionType type, double amount, bool success) {
        const string transactionId = "TXN_" + to_string(++transactionCounter_);
        Transaction transaction(transactionId, type, amount);
        if (success) {
            transaction.markSuccess();
        } else {
            transaction.markFailed();
        }
        transactions_.push_back(transaction);
    }
};

} // namespace atm_lld

#endif // ATM_LLD_CORE_ATMSYSTEM_H

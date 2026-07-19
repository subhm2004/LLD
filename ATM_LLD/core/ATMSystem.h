#ifndef ATM_LLD_CORE_ATMSYSTEM_H
#define ATM_LLD_CORE_ATMSYSTEM_H

#include <bits/stdc++.h>

#include "../models/BankAccount.h"
#include "../models/Card.h"
#include "../models/Transaction.h"
#include "../services/AuthenticationService.h"
#include "../services/CashDispenser.h"

using namespace std;

namespace atm_lld {

/**
 * @class ATMSystem
 * @brief ATM System ka main interface (Facade Design Pattern).
 * 
 * Client codes (jaise display screens, bank runner tests) direct is facade class ke APIs consume karte hain.
 * Complex internal processes (jaise auth check, cash dispense calculation, account debits) isme hidden hain.
 */
class ATMSystem {
public:
    // Bank system configuration mapping ke liye new Account register interface.
    void addAccount(const BankAccount &account) {
        auto it = accounts_.find(account.getAccountNumber());
        if (it != accounts_.end()) {
            it->second = account;
        } else {
            accounts_.emplace(account.getAccountNumber(), account);
        }
    }

    // Card mappings initialize karne ke liye helper method.
    void addCard(const Card &card) {
        auto it = cards_.find(card.getCardNumber());
        if (it != cards_.end()) {
            it->second = card;
        } else {
            cards_.emplace(card.getCardNumber(), card);
        }
    }

    // User authentication session initiation logic.
    bool login(const string &cardNumber, const string &pin) {
        Card *card = getCard(cardNumber);
        if (card == nullptr) {
            return false; // Card register nahi mila.
        }
        // Authentication check single verification rules verify call.
        if (!authService_.authenticate(*card, pin)) {
            return false; // Invalid PIN
        }
        currentCardNumber_ = cardNumber; // Session lock active
        return true;
    }

    // Active card user session close/clear logic.
    void logout() { 
        currentCardNumber_.clear(); 
    }

    // Balance query API checks.
    double checkBalance() {
        BankAccount *account = getCurrentAccountOrThrow();
        // Log transaction history.
        createTransaction(TransactionType::BALANCE_INQUIRY, 0.0, true);
        return account->getBalance();
    }

    // Cash withdrawal execution handler.
    map<int, int> withdrawCash(int amount) {
        if (amount <= 0) {
            createTransaction(TransactionType::CASH_WITHDRAWAL, amount, false);
            throw runtime_error("Withdraw amount invalid/zero nahi ho sakti.");
        }

        BankAccount *account = getCurrentAccountOrThrow();
        
        // Validation check 1: balance limits verification.
        if (account->getBalance() < amount) {
            createTransaction(TransactionType::CASH_WITHDRAWAL, amount, false);
            throw runtime_error("Account me insufficient balance hai.");
        }
        
        // Validation check 2: ATM cash vault bounds check.
        if (!cashDispenser_.canDispense(amount)) {
            createTransaction(TransactionType::CASH_WITHDRAWAL, amount, false);
            throw runtime_error("ATM dispenser me requested notes options available nahi hain.");
        }

        // Logic check complete: Account balance debits and physical cash notes allocation.
        account->debit(amount);
        map<int, int> notes = cashDispenser_.dispense(amount);
        
        // Log transaction success metrics.
        createTransaction(TransactionType::CASH_WITHDRAWAL, amount, true);
        return notes;
    }

private:
    unordered_map<string, BankAccount> accounts_; // DB mock lookup index of BankAccounts
    unordered_map<string, Card> cards_;           // DB mock lookup index of Cards
    vector<Transaction> transactions_;             // History audits logger
    AuthenticationService authService_;            // Card pin verification single logic service
    CashDispenser cashDispenser_;                  // Cash vault notes manager service
    string currentCardNumber_;                    // Active session state tracking token
    int transactionCounter_ = 0;                  // Unique auto-increment tracking index

    // Card reference lookup internal helper method.
    Card *getCard(const string &cardNumber) {
        auto it = cards_.find(cardNumber);
        if (it == cards_.end()) {
            return nullptr;
        }
        return &it->second;
    }

    // Active session profile bank account verify rules check.
    BankAccount *getCurrentAccountOrThrow() {
        if (currentCardNumber_.empty()) {
            throw runtime_error("Error: Koi active session lock nahi mila. Please insert card first.");
        }
        Card *card = getCard(currentCardNumber_);
        if (card == nullptr) {
            throw runtime_error("Error: Active session lock validation error.");
        }

        auto accountIt = accounts_.find(card->getAccountNumber());
        if (accountIt == accounts_.end()) {
            throw runtime_error("Error: Linked bank account not found registry.");
        }
        return &accountIt->second;
    }

    // Audit logs transaction entries creation wrapper helper.
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

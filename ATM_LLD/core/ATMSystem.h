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

class ATMSystem {
public:
    void addAccount(const BankAccount &account) {
        auto it = accounts_.find(account.getAccountNumber());
        if (it != accounts_.end()) {
            it->second = account;
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

    bool login(const string &cardNumber, const string &pin) {
        Card *card = getCard(cardNumber);
        if (card == nullptr) {
            return false;
        }
        if (!authService_.authenticate(*card, pin)) {
            return false;
        }
        currentCardNumber_ = cardNumber;
        return true;
    }

    void logout() { currentCardNumber_.clear(); }

    double checkBalance() {
        BankAccount *account = getCurrentAccountOrThrow();
        createTransaction(TransactionType::BALANCE_INQUIRY, 0.0, true);
        return account->getBalance();
    }

    map<int, int> withdrawCash(int amount) {
        if (amount <= 0) {
            createTransaction(TransactionType::CASH_WITHDRAWAL, amount, false);
            throw runtime_error("Invalid amount");
        }

        BankAccount *account = getCurrentAccountOrThrow();
        if (account->getBalance() < amount) {
            createTransaction(TransactionType::CASH_WITHDRAWAL, amount, false);
            throw runtime_error("Insufficient account balance");
        }
        if (!cashDispenser_.canDispense(amount)) {
            createTransaction(TransactionType::CASH_WITHDRAWAL, amount, false);
            throw runtime_error("ATM has insufficient notes");
        }

        account->debit(amount);
        map<int, int> notes = cashDispenser_.dispense(amount);
        createTransaction(TransactionType::CASH_WITHDRAWAL, amount, true);
        return notes;
    }

private:
    unordered_map<string, BankAccount> accounts_;
    unordered_map<string, Card> cards_;
    vector<Transaction> transactions_;
    AuthenticationService authService_;
    CashDispenser cashDispenser_;
    string currentCardNumber_;
    int transactionCounter_ = 0;

    Card *getCard(const string &cardNumber) {
        auto it = cards_.find(cardNumber);
        if (it == cards_.end()) {
            return nullptr;
        }
        return &it->second;
    }

    BankAccount *getCurrentAccountOrThrow() {
        if (currentCardNumber_.empty()) {
            throw runtime_error("No active card session");
        }
        Card *card = getCard(currentCardNumber_);
        if (card == nullptr) {
            throw runtime_error("Active card not found");
        }

        auto accountIt = accounts_.find(card->getAccountNumber());
        if (accountIt == accounts_.end()) {
            throw runtime_error("Linked account not found");
        }
        return &accountIt->second;
    }

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

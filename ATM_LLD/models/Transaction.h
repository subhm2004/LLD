#ifndef ATM_LLD_MODELS_TRANSACTION_H
#define ATM_LLD_MODELS_TRANSACTION_H

#include <bits/stdc++.h>

#include "../enums/TransactionStatus.h"
#include "../enums/TransactionType.h"

using namespace std;

namespace atm_lld {

/**
 * @class Transaction
 * @brief ATM me hone wale har ek activity/transaction ki details hold karti hai.
 * 
 * Isme dynamic status transitions (markSuccess/markFailed) aur details auditing fields mapped hain.
 */
class Transaction {
public:
    // Nayi transaction details initialize karne ke liye constructor. Initial status INITIATED set hota hai.
    Transaction(string transactionId, TransactionType type, double amount)
        : transactionId_(std::move(transactionId)),
          type_(type),
          amount_(amount),
          status_(TransactionStatus::INITIATED),
          timestamp_(time(nullptr)) {}

    // Transaction ID get karne ka getter.
    const string &getTransactionId() const { return transactionId_; }
    
    // Type of transaction (BALANCE_INQUIRY / CASH_WITHDRAWAL) fetch karne ka getter.
    TransactionType getType() const { return type_; }
    
    // Transacted amount retrieve karne ka getter.
    double getAmount() const { return amount_; }
    
    // Transaction current status fetch karne ka getter.
    TransactionStatus getStatus() const { return status_; }
    
    // Stored transaction epoch timestamp check karne ka helper.
    long long getTimestamp() const { return timestamp_; }

    // Transaction complete/success hone par status update method.
    void markSuccess() { 
        status_ = TransactionStatus::SUCCESS; 
    }
    
    // Transaction cancel/fail hone par status update method.
    void markFailed() { 
        status_ = TransactionStatus::FAILED; 
    }

private:
    string transactionId_;       // Unique transaction tracking identifier
    TransactionType type_;       // Type of activity enum
    double amount_;              // Cash transaction amount bounds
    TransactionStatus status_;   // Current status state
    long long timestamp_;        // Date-time logger timestamp in epoch format
};

} // namespace atm_lld

#endif // ATM_LLD_MODELS_TRANSACTION_H

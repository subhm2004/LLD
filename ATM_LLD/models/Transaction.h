#ifndef ATM_LLD_MODELS_TRANSACTION_H
#define ATM_LLD_MODELS_TRANSACTION_H

#include <bits/stdc++.h>

#include "../enums/TransactionStatus.h"
#include "../enums/TransactionType.h"

using namespace std;

namespace atm_lld {

class Transaction {
public:
    Transaction(string transactionId, TransactionType type, double amount)
        : transactionId_(std::move(transactionId)),
          type_(type),
          amount_(amount),
          status_(TransactionStatus::INITIATED),
          timestamp_(time(nullptr)) {}

    const string &getTransactionId() const { return transactionId_; }
    TransactionType getType() const { return type_; }
    double getAmount() const { return amount_; }
    TransactionStatus getStatus() const { return status_; }
    long long getTimestamp() const { return timestamp_; }

    void markSuccess() { status_ = TransactionStatus::SUCCESS; }
    void markFailed() { status_ = TransactionStatus::FAILED; }

private:
    string transactionId_;
    TransactionType type_;
    double amount_;
    TransactionStatus status_;
    long long timestamp_;
};

} // namespace atm_lld

#endif // ATM_LLD_MODELS_TRANSACTION_H

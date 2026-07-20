// ============================================================================
//  models/Transaction.h  —  ATM ki ek activity ka record (audit log)
// ----------------------------------------------------------------------------
//  Har balance-inquiry ya withdrawal ka ek Transaction record banta hai: uska
//  id, type, amount, status, aur timestamp. Ye "kab kya hua" ka poora hisaab.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ TRANSACTION ek chhota STATE MACHINE hai                               │
//  │                                                                          │
//  │     INITIATED  ── markSuccess() ──►  SUCCESS                              │
//  │              └── markFailed()  ──►  FAILED                                │
//  │                                                                          │
//  │  Har transaction constructor me INITIATED se shuru hoti hai. Phir kaam    │
//  │  ke natije ke hisaab se SUCCESS ya FAILED me chali jaati hai. Ek baar     │
//  │  final state me pahunchne ke baad wo record freeze ho jaata hai (audit).  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `timestamp_` epoch (seconds since 1970) me hai — `time(nullptr)` se. Isse
//     transactions ko time se sort/filter kiya ja sakta hai (mini-statement ke
//     kaam aata). BALANCE_INQUIRY ka amount 0 hota hai (paisa nahi hilta).
// ============================================================================
#ifndef ATM_LLD_MODELS_TRANSACTION_H
#define ATM_LLD_MODELS_TRANSACTION_H

#include <ctime>
#include <string>
#include <utility>

#include "../enums/TransactionStatus.h"
#include "../enums/TransactionType.h"

using namespace std;

namespace atm_lld {

class Transaction {
public:
    // Nayi transaction — status hamesha INITIATED se shuru, timestamp abhi ka.
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

    // Status transitions (INITIATED -> final)
    void markSuccess() { status_ = TransactionStatus::SUCCESS; }
    void markFailed() { status_ = TransactionStatus::FAILED; }

private:
    string transactionId_;     // unique id (jaise "TXN_1")
    TransactionType type_;     // BALANCE_INQUIRY / CASH_WITHDRAWAL
    double amount_;            // kitna paisa (inquiry me 0)
    TransactionStatus status_; // INITIATED / SUCCESS / FAILED
    long long timestamp_;      // kab hui (epoch seconds)
};

} // namespace atm_lld

#endif // ATM_LLD_MODELS_TRANSACTION_H

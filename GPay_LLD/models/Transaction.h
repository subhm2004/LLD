#ifndef GPAY_LLD_MODELS_TRANSACTION_H
#define GPAY_LLD_MODELS_TRANSACTION_H

#include <string>

#include "../enums/PaymentRail.h"
#include "../enums/TransactionStatus.h"
#include "../enums/TransactionType.h"

namespace gpay_lld {

class Transaction {
public:
    Transaction() = default;

    Transaction(std::string txnId, std::string payerUserId, std::string payeeUserId,
                std::string payerUpi, std::string payeeUpi, double amount,
                TransactionType type, TransactionStatus status, PaymentRail rail,
                std::string note = "")
        : txnId_(std::move(txnId)),
          payerUserId_(std::move(payerUserId)),
          payeeUserId_(std::move(payeeUserId)),
          payerUpi_(std::move(payerUpi)),
          payeeUpi_(std::move(payeeUpi)),
          amount_(amount),
          type_(type),
          status_(status),
          rail_(rail),
          note_(std::move(note)) {}

    const std::string& getTxnId() const { return txnId_; }
    const std::string& getPayerUserId() const { return payerUserId_; }
    const std::string& getPayeeUserId() const { return payeeUserId_; }
    const std::string& getPayerUpi() const { return payerUpi_; }
    const std::string& getPayeeUpi() const { return payeeUpi_; }
    double getAmount() const { return amount_; }
    TransactionType getType() const { return type_; }
    TransactionStatus getStatus() const { return status_; }
    PaymentRail getRail() const { return rail_; }
    const std::string& getNote() const { return note_; }

    void markSuccess() { status_ = TransactionStatus::SUCCESS; }
    void markFailed() { status_ = TransactionStatus::FAILED; }

private:
    std::string txnId_;
    std::string payerUserId_;
    std::string payeeUserId_;
    std::string payerUpi_;
    std::string payeeUpi_;
    double amount_{0};
    TransactionType type_{TransactionType::P2P_SEND};
    TransactionStatus status_{TransactionStatus::INITIATED};
    PaymentRail rail_{PaymentRail::LINKED_BANK};
    std::string note_;
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_MODELS_TRANSACTION_H

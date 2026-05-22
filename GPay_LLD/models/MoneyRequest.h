#ifndef GPAY_LLD_MODELS_MONEYREQUEST_H
#define GPAY_LLD_MODELS_MONEYREQUEST_H

#include <string>

#include "../enums/TransactionStatus.h"

namespace gpay_lld {

class MoneyRequest {
public:
    MoneyRequest() = default;

    MoneyRequest(std::string requestId, std::string requesterUserId, std::string payerUserId,
                 double amount, std::string note)
        : requestId_(std::move(requestId)),
          requesterUserId_(std::move(requesterUserId)),
          payerUserId_(std::move(payerUserId)),
          amount_(amount),
          note_(std::move(note)) {}

    const std::string& getRequestId() const { return requestId_; }
    const std::string& getRequesterUserId() const { return requesterUserId_; }
    const std::string& getPayerUserId() const { return payerUserId_; }
    double getAmount() const { return amount_; }
    const std::string& getNote() const { return note_; }
    TransactionStatus getStatus() const { return status_; }

    void markFulfilled() { status_ = TransactionStatus::SUCCESS; }
    void markCancelled() { status_ = TransactionStatus::FAILED; }
    bool isPending() const { return status_ == TransactionStatus::PENDING; }

private:
    std::string requestId_;
    std::string requesterUserId_;
    std::string payerUserId_;
    double amount_{0};
    std::string note_;
    TransactionStatus status_{TransactionStatus::PENDING};
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_MODELS_MONEYREQUEST_H

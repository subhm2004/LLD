#ifndef GPAY_LLD_FACTORIES_TRANSACTIONFACTORY_H
#define GPAY_LLD_FACTORIES_TRANSACTIONFACTORY_H

#include "../enums/PaymentRail.h"
#include "../enums/TransactionStatus.h"
#include "../enums/TransactionType.h"
#include "../models/Transaction.h"

namespace gpay_lld {

class TransactionFactory {
public:
    static Transaction create(const std::string& txnId, const std::string& payerUserId,
                              const std::string& payeeUserId, const std::string& payerUpi,
                              const std::string& payeeUpi, double amount, TransactionType type,
                              PaymentRail rail, const std::string& note = "") {
        return Transaction(txnId, payerUserId, payeeUserId, payerUpi, payeeUpi, amount, type,
                           TransactionStatus::INITIATED, rail, note);
    }
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_FACTORIES_TRANSACTIONFACTORY_H

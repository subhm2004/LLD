#ifndef GPAY_LLD_SERVICES_TRANSACTIONLEDGERSERVICE_H
#define GPAY_LLD_SERVICES_TRANSACTIONLEDGERSERVICE_H

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/Transaction.h"

namespace gpay_lld {

class TransactionLedgerService {
public:
    void save(Transaction txn) { transactions_[txn.getTxnId()] = std::move(txn); }

    Transaction* findById(const std::string& txnId) {
        auto it = transactions_.find(txnId);
        if (it == transactions_.end()) {
            return nullptr;
        }
        return &it->second;
    }

    std::vector<Transaction> listForUser(const std::string& userId, size_t limit = 50) const {
        std::vector<Transaction> result;
        for (const auto& entry : transactions_) {
            const Transaction& t = entry.second;
            if (t.getPayerUserId() == userId || t.getPayeeUserId() == userId) {
                result.push_back(t);
            }
        }
        std::sort(result.begin(), result.end(),
                  [](const Transaction& a, const Transaction& b) {
                      return a.getTxnId() > b.getTxnId();
                  });
        if (result.size() > limit) {
            result.resize(limit);
        }
        return result;
    }

private:
    std::unordered_map<std::string, Transaction> transactions_;
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_SERVICES_TRANSACTIONLEDGERSERVICE_H

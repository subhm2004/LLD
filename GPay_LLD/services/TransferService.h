#ifndef GPAY_LLD_SERVICES_TRANSFERSERVICE_H
#define GPAY_LLD_SERVICES_TRANSFERSERVICE_H

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "../enums/PaymentRail.h"
#include "../enums/TransactionType.h"
#include "../factories/PaymentRailFactory.h"
#include "../factories/TransactionFactory.h"
#include "../models/BankAccount.h"
#include "../models/Transaction.h"
#include "../models/User.h"
#include "../models/Wallet.h"
#include "../strategies/IPaymentRailStrategy.h"
#include "DailyLimitService.h"
#include "TransactionLedgerService.h"

namespace gpay_lld {

class TransferService {
public:
    TransferService(TransactionLedgerService& ledger, DailyLimitService& limitService)
        : ledger_(ledger), limitService_(limitService) {}

    Transaction execute(const User& payer, const User& payee, double amount, PaymentRail rail,
                        TransactionType type, const std::string& note,
                        std::unordered_map<std::string, BankAccount>& bankAccounts,
                        std::unordered_map<std::string, Wallet>& wallets,
                        std::unordered_set<std::string>& processedClientIds,
                        const std::string& clientRequestId, int& txnCounter) {
        if (payer.getUserId() == payee.getUserId()) {
            throw std::runtime_error("cannot transfer to self");
        }
        if (amount <= 0) {
            throw std::invalid_argument("amount must be positive");
        }
        if (!clientRequestId.empty()) {
            if (processedClientIds.count(clientRequestId) != 0) {
                throw std::runtime_error("duplicate clientRequestId");
            }
            processedClientIds.insert(clientRequestId);
        }

        limitService_.recordSpend(payer.getUserId(), amount);

        const std::string txnId = "TXN_" + std::to_string(++txnCounter);
        Transaction txn = TransactionFactory::create(
            txnId, payer.getUserId(), payee.getUserId(), payer.getUpiId(), payee.getUpiId(),
            amount, type, rail, note);

        std::shared_ptr<IPaymentRailStrategy> strategy = PaymentRailFactory::create(rail);
        try {
            strategy->debit(payer.getUserId(), amount, bankAccounts, wallets);
            strategy->credit(payee.getUserId(), amount, bankAccounts, wallets);
            txn.markSuccess();
        } catch (...) {
            txn.markFailed();
            ledger_.save(txn);
            throw;
        }

        ledger_.save(txn);
        return txn;
    }

private:
    TransactionLedgerService& ledger_;
    DailyLimitService& limitService_;
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_SERVICES_TRANSFERSERVICE_H

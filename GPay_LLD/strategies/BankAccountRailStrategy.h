#ifndef GPAY_LLD_STRATEGIES_BANKACCOUNTRAILSTRATEGY_H
#define GPAY_LLD_STRATEGIES_BANKACCOUNTRAILSTRATEGY_H

#include <stdexcept>
#include <string>

#include "IPaymentRailStrategy.h"

namespace gpay_lld {

class BankAccountRailStrategy : public IPaymentRailStrategy {
public:
    explicit BankAccountRailStrategy(std::string defaultAccountId)
        : defaultAccountId_(std::move(defaultAccountId)) {}

    PaymentRail railType() const override { return PaymentRail::LINKED_BANK; }

    void debit(const std::string& userId, double amount,
               std::unordered_map<std::string, BankAccount>& bankAccounts,
               std::unordered_map<std::string, Wallet>&) override {
        BankAccount& account = getPrimaryAccount(userId, bankAccounts);
        account.debit(amount);
    }

    void credit(const std::string& userId, double amount,
                std::unordered_map<std::string, BankAccount>& bankAccounts,
                std::unordered_map<std::string, Wallet>&) override {
        BankAccount& account = getPrimaryAccount(userId, bankAccounts);
        account.credit(amount);
    }

private:
    std::string defaultAccountId_;

    BankAccount& getPrimaryAccount(
        const std::string& userId,
        std::unordered_map<std::string, BankAccount>& bankAccounts) const {
        for (auto& entry : bankAccounts) {
            if (entry.second.getUserId() == userId) {
                return entry.second;
            }
        }
        throw std::runtime_error("no linked bank account for user: " + userId);
    }
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_STRATEGIES_BANKACCOUNTRAILSTRATEGY_H

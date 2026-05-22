#ifndef GPAY_LLD_STRATEGIES_WALLETRAILSTRATEGY_H
#define GPAY_LLD_STRATEGIES_WALLETRAILSTRATEGY_H

#include <stdexcept>

#include "IPaymentRailStrategy.h"

namespace gpay_lld {

class WalletRailStrategy : public IPaymentRailStrategy {
public:
    PaymentRail railType() const override { return PaymentRail::GPAY_WALLET; }

    void debit(const std::string& userId, double amount,
               std::unordered_map<std::string, BankAccount>&,
               std::unordered_map<std::string, Wallet>& wallets) override {
        auto it = wallets.find(userId);
        if (it == wallets.end()) {
            throw std::runtime_error("wallet not found for user: " + userId);
        }
        it->second.debit(amount);
    }

    void credit(const std::string& userId, double amount,
                std::unordered_map<std::string, BankAccount>&,
                std::unordered_map<std::string, Wallet>& wallets) override {
        auto it = wallets.find(userId);
        if (it == wallets.end()) {
            wallets.emplace(userId, Wallet(userId, 0));
            it = wallets.find(userId);
        }
        it->second.credit(amount);
    }
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_STRATEGIES_WALLETRAILSTRATEGY_H

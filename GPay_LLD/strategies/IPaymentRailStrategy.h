#ifndef GPAY_LLD_STRATEGIES_IPAYMENTRAILSTRATEGY_H
#define GPAY_LLD_STRATEGIES_IPAYMENTRAILSTRATEGY_H

#include <string>
#include <unordered_map>

#include "../enums/PaymentRail.h"
#include "../models/BankAccount.h"
#include "../models/Wallet.h"

namespace gpay_lld {

class IPaymentRailStrategy {
public:
    virtual ~IPaymentRailStrategy() = default;
    virtual PaymentRail railType() const = 0;
    virtual void debit(const std::string& userId, double amount,
                       std::unordered_map<std::string, BankAccount>& bankAccounts,
                       std::unordered_map<std::string, Wallet>& wallets) = 0;
    virtual void credit(const std::string& userId, double amount,
                      std::unordered_map<std::string, BankAccount>& bankAccounts,
                      std::unordered_map<std::string, Wallet>& wallets) = 0;
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_STRATEGIES_IPAYMENTRAILSTRATEGY_H

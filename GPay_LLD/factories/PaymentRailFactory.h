#ifndef GPAY_LLD_FACTORIES_PAYMENTRAILFACTORY_H
#define GPAY_LLD_FACTORIES_PAYMENTRAILFACTORY_H

#include <memory>

#include "../enums/PaymentRail.h"
#include "../strategies/BankAccountRailStrategy.h"
#include "../strategies/IPaymentRailStrategy.h"
#include "../strategies/WalletRailStrategy.h"

namespace gpay_lld {

class PaymentRailFactory {
public:
    static std::shared_ptr<IPaymentRailStrategy> create(PaymentRail rail) {
        switch (rail) {
            case PaymentRail::LINKED_BANK:
                return std::make_shared<BankAccountRailStrategy>("");
            case PaymentRail::GPAY_WALLET:
                return std::make_shared<WalletRailStrategy>();
        }
        return std::make_shared<BankAccountRailStrategy>("");
    }
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_FACTORIES_PAYMENTRAILFACTORY_H

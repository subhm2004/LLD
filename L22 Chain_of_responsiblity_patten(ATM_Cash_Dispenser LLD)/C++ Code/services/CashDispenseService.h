#ifndef COR_ATM_SERVICES_CASH_DISPENSE_SERVICE_H
#define COR_ATM_SERVICES_CASH_DISPENSE_SERVICE_H

#include <iostream>

#include "../enums/DispenseStatus.h"
#include "../managers/CashDispenseChainManager.h"
#include "../models/WithdrawalRequest.h"

namespace cor_atm {

class CashDispenseService {
public:
    void initialize(const ATMCashInventory &inventory) {
        inventory_ = inventory;
        chainManager_.buildChain(inventory_);
    }

    DispenseStatus withdraw(const WithdrawalRequest &request) {
        const int amount = request.getAmount();

        if (amount <= 0 || amount % 100 != 0) {
            std::cout << "[CashDispenseService] Invalid withdrawal amount: ₹" << amount << "\n";
            return DispenseStatus::INVALID_AMOUNT;
        }

        MoneyHandler *chainHead = chainManager_.getChainHead();
        if (chainHead == nullptr) {
            std::cout << "[CashDispenseService] Dispense chain not initialized.\n";
            return DispenseStatus::INSUFFICIENT_ATM_CASH;
        }

        std::cout << "\nDispensing amount: ₹" << amount << std::endl;
        chainHead->dispense(amount);
        return DispenseStatus::SUCCESS;
    }

private:
    ATMCashInventory inventory_;
    CashDispenseChainManager chainManager_;
};

} // namespace cor_atm

#endif // COR_ATM_SERVICES_CASH_DISPENSE_SERVICE_H

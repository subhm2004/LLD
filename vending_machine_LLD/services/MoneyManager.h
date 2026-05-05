#ifndef VENDING_MACHINE_LLD_SERVICES_MONEYMANAGER_H
#define VENDING_MACHINE_LLD_SERVICES_MONEYMANAGER_H

#include <bits/stdc++.h>
#include <stdexcept>

using namespace std;

namespace vending_machine_lld {

class MoneyManager {
public:
    void insertMoney(double amount) {
        if (amount <= 0.0) {
            throw std::runtime_error("Inserted amount should be positive");
        }
        currentBalance_ += amount;
    }

    double getCurrentBalance() const { return currentBalance_; }

    void resetBalance() { currentBalance_ = 0.0; }

private:
    double currentBalance_ = 0.0;
};

} // namespace vending_machine_lld

#endif // VENDING_MACHINE_LLD_SERVICES_MONEYMANAGER_H

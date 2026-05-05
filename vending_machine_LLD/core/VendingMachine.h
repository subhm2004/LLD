#ifndef VENDING_MACHINE_LLD_CORE_VENDINGMACHINE_H
#define VENDING_MACHINE_LLD_CORE_VENDINGMACHINE_H

#include <bits/stdc++.h>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../enums/ItemType.h"
#include "../models/Item.h"
#include "../services/Inventory.h"
#include "../services/MoneyManager.h"

using namespace std;

namespace vending_machine_lld {

class VendingMachine {
public:
    VendingMachine() : selectedItem_(ItemType::WATER), itemSelected_(false) {}

    void loadItem(ItemType type, const Item &item, int quantity) {
        inventory_.addItem(type, item, quantity);
    }

    void selectItem(ItemType type) {
        const Item &item = inventory_.getItem(type);
        if (!inventory_.hasStock(type)) {
            throw std::runtime_error(item.getName() + " is out of stock");
        }
        selectedItem_ = type;
        itemSelected_ = true;
        std::cout << "Selected: " << item.getName() << " | Price: " << std::fixed
                  << std::setprecision(2) << item.getPrice() << "\n";
    }

    void insertMoney(double amount) {
        moneyManager_.insertMoney(amount);
        std::cout << "Inserted: " << std::fixed << std::setprecision(2) << amount
                  << " | Current Balance: " << moneyManager_.getCurrentBalance() << "\n";
    }

    void dispense() {
        if (!itemSelected_) {
            throw std::runtime_error("Please select an item first");
        }

        const Item &item = inventory_.getItem(selectedItem_);
        const double price = item.getPrice();
        const double paid = moneyManager_.getCurrentBalance();

        if (paid < price) {
            throw std::runtime_error("Insufficient amount. Need: " + std::to_string(price - paid));
        }

        inventory_.reduceStock(selectedItem_);
        const double change = paid - price;

        std::cout << "Dispensed: " << item.getName() << "\n";
        std::cout << "Change Returned: " << std::fixed << std::setprecision(2) << change << "\n";

        itemSelected_ = false;
        moneyManager_.resetBalance();
    }

    void cancelAndRefund() {
        const double refund = moneyManager_.getCurrentBalance();
        std::cout << "Transaction cancelled. Refund: " << std::fixed << std::setprecision(2)
                  << refund << "\n";
        itemSelected_ = false;
        moneyManager_.resetBalance();
    }

    void showInventory() const {
        inventory_.printInventory();
    }

private:
    Inventory inventory_;
    MoneyManager moneyManager_;
    ItemType selectedItem_;
    bool itemSelected_;
};

} // namespace vending_machine_lld

#endif // VENDING_MACHINE_LLD_CORE_VENDINGMACHINE_H

#ifndef VENDING_MACHINE_LLD_SERVICES_INVENTORY_H
#define VENDING_MACHINE_LLD_SERVICES_INVENTORY_H

#include <bits/stdc++.h>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

#include "../enums/ItemType.h"
#include "../models/Item.h"

using namespace std;

namespace vending_machine_lld {

class Inventory {
public:
    void addItem(ItemType type, const Item &item, int quantity) {
        if (quantity <= 0) {
            throw std::runtime_error("Quantity must be positive");
        }
        items_[type] = item;
        stock_[type] += quantity;
    }

    bool hasStock(ItemType type) const {
        auto it = stock_.find(type);
        return it != stock_.end() && it->second > 0;
    }

    void reduceStock(ItemType type) {
        if (!hasStock(type)) {
            throw std::runtime_error("Item out of stock");
        }
        --stock_[type];
    }

    const Item &getItem(ItemType type) const {
        auto it = items_.find(type);
        if (it == items_.end()) {
            throw std::runtime_error("Item not configured in machine");
        }
        return it->second;
    }

    void printInventory() const {
        std::cout << "\n=== Inventory ===\n";
        for (const auto &entry : stock_) {
            const ItemType type = entry.first;
            const int qty = entry.second;
            const Item &item = items_.at(type);
            std::cout << item.getName() << " (" << itemTypeToString(type) << ")"
                      << " | Price: " << std::fixed << std::setprecision(2) << item.getPrice()
                      << " | Qty: " << qty << "\n";
        }
    }

private:
    std::unordered_map<ItemType, Item> items_;
    std::unordered_map<ItemType, int> stock_;
};

} // namespace vending_machine_lld

#endif // VENDING_MACHINE_LLD_SERVICES_INVENTORY_H

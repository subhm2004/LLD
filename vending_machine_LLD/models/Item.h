#ifndef VENDING_MACHINE_LLD_MODELS_ITEM_H
#define VENDING_MACHINE_LLD_MODELS_ITEM_H

#include <bits/stdc++.h>
#include <string>
#include <utility>

#include "../enums/ItemType.h"

using namespace std;

namespace vending_machine_lld {

class Item {
public:
    Item() : name_(""), price_(0.0), type_(ItemType::WATER) {}

    Item(std::string name, double price, ItemType type)
        : name_(std::move(name)), price_(price), type_(type) {}

    const std::string &getName() const { return name_; }
    double getPrice() const { return price_; }
    ItemType getType() const { return type_; }

private:
    std::string name_;
    double price_;
    ItemType type_;
};

} // namespace vending_machine_lld

#endif // VENDING_MACHINE_LLD_MODELS_ITEM_H

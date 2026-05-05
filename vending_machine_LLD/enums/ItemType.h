#ifndef VENDING_MACHINE_LLD_ENUMS_ITEMTYPE_H
#define VENDING_MACHINE_LLD_ENUMS_ITEMTYPE_H

#include <bits/stdc++.h>
#include <string>

using namespace std;

namespace vending_machine_lld {

enum class ItemType { COKE, PEPSI, WATER, JUICE };

inline std::string itemTypeToString(ItemType type) {
    if (type == ItemType::COKE) {
        return "COKE";
    }
    if (type == ItemType::PEPSI) {
        return "PEPSI";
    }
    if (type == ItemType::WATER) {
        return "WATER";
    }
    return "JUICE";
}

} // namespace vending_machine_lld

#endif // VENDING_MACHINE_LLD_ENUMS_ITEMTYPE_H

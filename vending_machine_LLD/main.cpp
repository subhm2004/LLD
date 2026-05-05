#include <bits/stdc++.h>
#include "core/VendingMachine.h"

using namespace std;
using namespace vending_machine_lld;

int main() {
    VendingMachine machine;

    machine.loadItem(ItemType::COKE, Item("Coke Can", 40.0, ItemType::COKE), 3);
    machine.loadItem(ItemType::PEPSI, Item("Pepsi Can", 35.0, ItemType::PEPSI), 2);
    machine.loadItem(ItemType::WATER, Item("Mineral Water", 20.0, ItemType::WATER), 5);
    machine.loadItem(ItemType::JUICE, Item("Orange Juice", 50.0, ItemType::JUICE), 2);

    machine.showInventory();
    machine.selectItem(ItemType::WATER);
    machine.insertMoney(50.0);
    machine.dispense();
    machine.showInventory();

    return 0;
}

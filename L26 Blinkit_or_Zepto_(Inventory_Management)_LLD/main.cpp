#include <bits/stdc++.h>

#include "core/BlinkitSystem.h"

using namespace std;
using namespace blinkit_lld;

int main() {
    DarkStore *storeA = new DarkStore("DarkStoreA", 0.0, 0.0);
    storeA->addStock(101, 5);
    storeA->addStock(102, 3);
    storeA->addStock(103, 7);

    DarkStore *storeB = new DarkStore("DarkStoreB", 3.0, 1.0);
    storeB->addStock(101, 2);
    storeB->addStock(201, 4);

    DarkStoreManager::getInstance()->registerDarkStore(storeA);
    DarkStoreManager::getInstance()->registerDarkStore(storeB);

    User user("Aditya", 1.0, 1.0);
    user.getCart()->addItem(101, 2);
    user.getCart()->addItem(103, 1);
    user.getCart()->addItem(201, 1);

    int orderId = OrderManager::getInstance()->placeOrder(&user);
    if (orderId != -1) {
        OrderManager::getInstance()->updateOrderStatus(orderId, OrderStatus::CONFIRMED);
        OrderManager::getInstance()->updateOrderStatus(orderId, OrderStatus::PACKING);
        OrderManager::getInstance()->updateOrderStatus(orderId, OrderStatus::OUT_FOR_DELIVERY);
        OrderManager::getInstance()->updateOrderStatus(orderId, OrderStatus::DELIVERED);
    }
    return 0;
}

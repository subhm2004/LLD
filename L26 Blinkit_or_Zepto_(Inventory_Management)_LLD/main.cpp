// ============================================================================
//  main.cpp  —  Blinkit/Zepto quick-commerce inventory system ka demo driver
// ----------------------------------------------------------------------------
//  Dark stores onboard karo, inventory + replenishment set karo, 5km catalog
//  dikhao, cart se order place karo — agar ek store full cart fulfill na kare
//  to order SPLIT across nearby stores (alag delivery partners). Patterns:
//  Strategy (replenish) + Factory (product/store) + Facade (BlinkitSystem).
// ============================================================================
#include <bits/stdc++.h>

#include "core/BlinkitSystem.h"

using namespace std;
using namespace blinkit_lld;

static void setupDarkStores() {
    DarkStore *storeA = new DarkStore("DarkStoreA", 0.0, 0.0, InventoryStoreType::DB);
    storeA->setReplenishStrategy(new ThresholdReplenishStrategy(3));
    storeA->addStock(101, 5);
    storeA->addStock(102, 2);

    DarkStore *storeB = new DarkStore("DarkStoreB", 4.0, 1.0, InventoryStoreType::DB);
    storeB->setReplenishStrategy(new ThresholdReplenishStrategy(3));
    storeB->addStock(101, 3);
    storeB->addStock(103, 10);

    DarkStore *storeC = new DarkStore("DarkStoreC", 2.0, 3.0, InventoryStoreType::IN_MEMORY);
    storeC->setReplenishStrategy(new ThresholdReplenishStrategy(3));
    storeC->addStock(102, 5);
    storeC->addStock(201, 7);

    DarkStoreManager::getInstance()->registerDarkStore(storeA);
    DarkStoreManager::getInstance()->registerDarkStore(storeB);
    DarkStoreManager::getInstance()->registerDarkStore(storeC);

    // Replenishment API (Strategy pattern) — optional per store:
    // map<int,int> plan{{102, 4}}; storeA->runReplenishment(plan);
}

int main() {
    setupDarkStores();

    User user("Hardik", 1.0, 1.0);
    DarkStoreManager::getInstance()->showCatalogWithinRadius(user.getX(), user.getY(), kServiceRadiusKm, user.getName());

    cout << "\nAdding items to cart\n";
    user.getCart()->addItem(101, 4);
    user.getCart()->addItem(102, 3);
    user.getCart()->addItem(103, 2);

    const int orderId = OrderManager::getInstance()->placeOrder(&user);
    if (orderId != -1) {
        OrderManager::getInstance()->updateOrderStatus(orderId, OrderStatus::CONFIRMED);
        OrderManager::getInstance()->updateOrderStatus(orderId, OrderStatus::PACKING);
        OrderManager::getInstance()->updateOrderStatus(orderId, OrderStatus::OUT_FOR_DELIVERY);
        OrderManager::getInstance()->updateOrderStatus(orderId, OrderStatus::DELIVERED);
    }

    return 0;
}

// ============================================================================
//  main.cpp  —  BLINKIT/ZEPTO quick-commerce inventory system (L26 LLD project)
// ----------------------------------------------------------------------------
//  System kya karta hai: dark stores (chhote local warehouses) onboard karo,
//  har store me inventory + replenishment strategy set karo, user ko 5km ke
//  andar ka catalog dikhao, cart se order place karo. AGAR ek store poora
//  cart fulfill na kar sake to order NEARBY stores me SPLIT ho jaata hai
//  (alag delivery partners)! Blinkit/Zepto ka mini LLD.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ORDER SPLITTING — is system ka sabse smart feature:                    │
//  │                                                                          │
//  │   User cart: SKU101 x4, SKU102 x3, SKU103 x2                            │
//  │   StoreA (paas): 101 x5, 102 x2   -> 101 poora + 102 me se 2 de sakta   │
//  │   StoreB (door): 101 x3, 103 x10  -> baaki 102 nahi, 103 de sakta       │
//  │                                                                          │
//  │   Result: order SPLIT — StoreA se kuch (Partner1), StoreB se kuch       │
//  │   (Partner2). Ek cart, multiple stores, multiple deliveries — bilkul    │
//  │   asli Blinkit jaisa jab ek store me sab available nahi hota!           │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  IS PROJECT ME PATTERNS:
//    Strategy  -> ReplenishStrategy (Threshold/Weekly) + InventoryStore backend
//    Factory   -> ProductFactory + InventoryStoreFactory
//    Facade    -> BlinkitSystem (DarkStoreManager + OrderManager coordinate)
//    Singleton -> DarkStoreManager + OrderManager
//    State machine -> OrderStatus transitions (PLACED->CONFIRMED->...->DELIVERED)
//  Detail: design_patterns_used.md padho!
//
//  DEMO FLOW: setup stores -> user ka catalog dekho -> cart bharo -> order
//  place (split hoga!) -> order status ko step-by-step aage badhao.
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

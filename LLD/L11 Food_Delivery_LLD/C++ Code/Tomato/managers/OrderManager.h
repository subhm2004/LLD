// ============================================================================
//  OrderManager.h — SINGLETON: saare orders ka central tracker
// ----------------------------------------------------------------------------
//  App ke saare orders ek jagah — add + list. RestaurantManager ki tarah
//  ye bhi SINGLETON hai (poore system me ek order-book).
//  ⚠️ NOTE: ye SIMPLE lazy singleton hai (bas null-check, koi lock nahi) —
//  RestaurantManager wale DCL se kam safe hai (multi-thread me race ho
//  sakta hai). Ek hi project me do alag singleton styles — L10 me inka
//  farq detail me hai. Consistency ke liye dono ko Meyers karna better hota.
// ============================================================================
#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include <vector>
#include <iostream>
#include "../models/Order.h"
using namespace std;

class OrderManager {
private:
    vector<Order*> orders;
    static OrderManager* instance;

    OrderManager() {
        // Private Constructor
    }

public:
    static OrderManager* getInstance() {
        if (!instance) {
            instance = new OrderManager();
        }
        return instance;
    }

    void addOrder(Order* order) {
        orders.push_back(order);
    }

    void listOrders() {
        cout << "\n--- All Orders ---" << endl;
        for (auto order : orders) {
            cout << order->getType() << " order for " << order->getUser()->getName()
                    << " | Total: ₹" << order->getTotal()
                    << " | At: " << order->getScheduled() << endl;
        }
    }
};

OrderManager* OrderManager::instance = nullptr;

#endif // ORDER_MANAGER_H

#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_ORDERSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_ORDERSERVICE_H

#include <bits/stdc++.h>

#include "../models/Order.h"

namespace ecommerce_checkout_lld {

using namespace std;

class OrderService {
public:
    void saveOrder(Order order) { orders_.insert_or_assign(order.getOrderId(), std::move(order)); }

    Order &getOrderOrThrow(const string &orderId) {
        auto it = orders_.find(orderId);
        if (it == orders_.end()) {
            throw runtime_error("Order not found: " + orderId);
        }
        return it->second;
    }

    const Order &getOrderOrThrow(const string &orderId) const {
        auto it = orders_.find(orderId);
        if (it == orders_.end()) {
            throw runtime_error("Order not found: " + orderId);
        }
        return it->second;
    }

    const Order *findByClientRequestId(const string &clientRequestId) const {
        if (clientRequestId.empty()) {
            return nullptr;
        }
        for (const auto &entry : orders_) {
            if (entry.second.getClientRequestId() == clientRequestId) {
                return &entry.second;
            }
        }
        return nullptr;
    }

    vector<const Order *> getOrdersForUser(const string &userId) const {
        vector<const Order *> result;
        for (const auto &entry : orders_) {
            if (entry.second.getUserId() == userId) {
                result.push_back(&entry.second);
            }
        }
        sort(result.begin(), result.end(), [](const Order *a, const Order *b) {
            return a->getOrderId() < b->getOrderId();
        });
        return result;
    }

private:
    unordered_map<string, Order> orders_;
};

}  // namespace ecommerce_checkout_lld

#endif

#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_CART_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_CART_H

#include <bits/stdc++.h>

#include "CartItem.h"

namespace ecommerce_checkout_lld {

using namespace std;

class Cart {
public:
    explicit Cart(string userId) : userId_(std::move(userId)) {}

    const string &getUserId() const { return userId_; }

    void upsertItem(const Product &product, int quantity) {
        if (quantity <= 0) {
            items_.erase(product.getProductId());
            return;
        }
        auto it = items_.find(product.getProductId());
        if (it != items_.end()) {
            it->second = CartItem(product, quantity);
        } else {
            items_.emplace(product.getProductId(), CartItem(product, quantity));
        }
    }

    void removeItem(const string &productId) { items_.erase(productId); }

    void clear() { items_.clear(); }

    bool isEmpty() const { return items_.empty(); }

    const unordered_map<string, CartItem> &getItems() const { return items_; }

    double subtotal() const {
        double total = 0.0;
        for (const auto &entry : items_) {
            total += entry.second.lineTotal();
        }
        return total;
    }

    int totalUnits() const {
        int units = 0;
        for (const auto &entry : items_) {
            units += entry.second.getQuantity();
        }
        return units;
    }

private:
    string userId_;
    unordered_map<string, CartItem> items_;
};

}  // namespace ecommerce_checkout_lld

#endif

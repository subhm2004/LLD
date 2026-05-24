#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_CARTSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_CARTSERVICE_H

#include <bits/stdc++.h>

#include "../models/Cart.h"
#include "CatalogService.h"

namespace ecommerce_checkout_lld {

using namespace std;

class CartService {
public:
    explicit CartService(CatalogService &catalog) : catalog_(catalog) {}

    Cart &getOrCreateCart(const string &userId) {
        auto it = carts_.find(userId);
        if (it == carts_.end()) {
            it = carts_.emplace(userId, Cart(userId)).first;
        }
        return it->second;
    }

    void addToCart(const string &userId, const string &productId, int quantity) {
        if (quantity <= 0) {
            throw invalid_argument("Quantity must be positive");
        }
        const Product &product = catalog_.getProductOrThrow(productId);
        Cart &cart = getOrCreateCart(userId);
        const int existingQty =
            cart.getItems().count(productId) ? cart.getItems().at(productId).getQuantity() : 0;
        cart.upsertItem(product, existingQty + quantity);
    }

    void updateQuantity(const string &userId, const string &productId, int quantity) {
        Cart &cart = getOrCreateCart(userId);
        if (quantity <= 0) {
            cart.removeItem(productId);
            return;
        }
        const Product &product = catalog_.getProductOrThrow(productId);
        cart.upsertItem(product, quantity);
    }

    void removeFromCart(const string &userId, const string &productId) {
        getOrCreateCart(userId).removeItem(productId);
    }

    void clearCart(const string &userId) { getOrCreateCart(userId).clear(); }

    const Cart &getCartOrThrow(const string &userId) const {
        auto it = carts_.find(userId);
        if (it == carts_.end() || it->second.isEmpty()) {
            throw runtime_error("Cart is empty for user: " + userId);
        }
        return it->second;
    }

    bool hasCart(const string &userId) const {
        auto it = carts_.find(userId);
        return it != carts_.end() && !it->second.isEmpty();
    }

    unordered_map<string, int> toQuantityMap(const Cart &cart) const {
        unordered_map<string, int> quantities;
        for (const auto &entry : cart.getItems()) {
            quantities[entry.first] = entry.second.getQuantity();
        }
        return quantities;
    }

private:
    CatalogService &catalog_;
    unordered_map<string, Cart> carts_;
};

}  // namespace ecommerce_checkout_lld

#endif

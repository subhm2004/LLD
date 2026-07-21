// ============================================================================
//  services/CartService.h  —  Cart operations (add / update / remove / clear)
// ----------------------------------------------------------------------------
//  Har user ka apna Cart: `unordered_map<userId, Cart>`. Ye service Cart model ke
//  upar BUSINESS RULES lagati hai (validation + catalog lookup).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Cart (model) vs CartService — kaam ka batwara                         │
//  │                                                                          │
//  │     Cart        -> data structure: items rakho, subtotal jodo (dumb)      │
//  │     CartService -> rules: qty > 0 ho, product catalog me EXIST kare,      │
//  │                    add pe purani qty me JODO, price catalog se lo         │
//  │                                                                          │
//  │  Model ko catalog ka pata nahi hona chahiye (warna coupling). Service     │
//  │  bicholiya hai. 📌 Ye classic "model + service layer" split hai.         │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `addToCart` ADD karta hai (replace nahi): purani qty padho -> +new -> upsert.
//     Isliye "Add to cart" 3 baar dabao to qty 3 hoti hai. Jabki `updateQuantity`
//     SET karta hai (user ne quantity box me 5 likha -> 5 hi hoga). Do alag intents,
//     do alag methods — ye chhota sa farak UI me bada matter karta hai.
//
//  ⚠ `CatalogService &catalog_` — REFERENCE member. Iska matlab:
//     (a) CartService ko safely copy/move nahi kiya ja sakta, aur
//     (b) catalog object CartService se ZYADA jeena chahiye.
//     Facade (EcommerceCheckoutSystem) me isi wajah se copy/move `= delete` hai —
//     wahan ka comment padho, wo is repo ka ek real dangling-reference bug tha.
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_CARTSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_CARTSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "../models/Cart.h"
#include "CatalogService.h"

namespace ecommerce_checkout_lld {

using namespace std;

class CartService {
public:
    // catalog INJECTED hai (CartService khud nahi banata) — Dependency Injection.
    explicit CartService(CatalogService &catalog) : catalog_(catalog) {}

    // Cart nahi hai to bana do (lazy creation) — user ko pehle "create cart"
    // call nahi karni padti.
    Cart &getOrCreateCart(const string &userId) {
        auto it = carts_.find(userId);
        if (it == carts_.end()) {
            it = carts_.emplace(userId, Cart(userId)).first;
        }
        return it->second;
    }

    // ⭐ ADD = purani quantity + nayi quantity (replace NAHI).
    void addToCart(const string &userId, const string &productId, int quantity) {
        if (quantity <= 0) {
            throw invalid_argument("Quantity must be positive");
        }
        // Product exist karta hai? Nahi to yahi phenk do (cart ganda na ho).
        const Product &product = catalog_.getProductOrThrow(productId);
        Cart &cart = getOrCreateCart(userId);
        const int existingQty =
            cart.getItems().count(productId) ? cart.getItems().at(productId).getQuantity() : 0;
        cart.upsertItem(product, existingQty + quantity); // ⭐ jod ke daalo
    }

    // ⭐ UPDATE = quantity SET karo (add nahi). qty<=0 -> item hata do.
    void updateQuantity(const string &userId, const string &productId, int quantity) {
        Cart &cart = getOrCreateCart(userId);
        if (quantity <= 0) {
            cart.removeItem(productId); // 0 set karna = remove karna (UI jaisa)
            return;
        }
        const Product &product = catalog_.getProductOrThrow(productId);
        cart.upsertItem(product, quantity); // ⭐ seedha set
    }

    void removeFromCart(const string &userId, const string &productId) {
        getOrCreateCart(userId).removeItem(productId);
    }

    // Successful checkout ke baad cart khaali (CheckoutService call karta hai).
    void clearCart(const string &userId) { getOrCreateCart(userId).clear(); }

    // Checkout ke liye: cart hona CHAHIYE aur khaali nahi hona chahiye.
    // 📌 "nahi mila" aur "khaali hai" — checkout ke liye dono ka matlab ek hi hai.
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

    // ⭐ Cart -> {productId: qty} — InventoryService ko yahi format chahiye.
    // Isse Inventory ko Cart/Product classes ka pata hi nahi chalta (decoupling):
    // wo bas ids aur numbers samajhta hai. Chhota sa ANTI-CORRUPTION LAYER.
    unordered_map<string, int> toQuantityMap(const Cart &cart) const {
        unordered_map<string, int> quantities;
        for (const auto &entry : cart.getItems()) {
            quantities[entry.first] = entry.second.getQuantity();
        }
        return quantities;
    }

private:
    CatalogService &catalog_;               // ⚠ reference (lifetime ka dhyaan — upar note)
    unordered_map<string, Cart> carts_;     // userId -> cart
};

}  // namespace ecommerce_checkout_lld

#endif

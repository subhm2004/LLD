// ============================================================================
//  models/Cart.h  —  Ek user ka shopping cart
// ----------------------------------------------------------------------------
//  Cart me items ek MAP me hain: productId -> CartItem.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `unordered_map<productId, CartItem>` (vector nahi) — kyun?            │
//  │                                                                          │
//  │  1. DUPLICATE apne aap band: ek product cart me sirf EK baar (key unique).│
//  │     Wahi product dobara add karo to quantity BADHTI hai, nayi line nahi   │
//  │     banti (CartService::addToCart me existing qty + new qty).            │
//  │  2. O(1) lookup/update/remove by productId (vector me scan karna padta).  │
//  │                                                                          │
//  │  ⚠ Trade-off: insertion ORDER nahi rehta (cart print me items ka order    │
//  │  har baar alag ho sakta). Real UI me order matter karta — tab             │
//  │  vector<CartItem> + index map, ya map<> (ordered) use karte.             │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `upsertItem` ka smart behavior: quantity <= 0 diya to item HATA deta hai
//     (0 quantity wali line cart me rakhne ka koi matlab nahi). Warna insert
//     ya update. Ek hi method se add/update/remove — clean API.
//
//  📌 `subtotal()` har baar SUM karke nikalta hai (cached nahi) — isse stale
//     total ka risk hi nahi. Items kam hain to ye bilkul theek hai.
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_CART_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_CART_H

#include <string>
#include <unordered_map>
#include <utility>

#include "CartItem.h"

namespace ecommerce_checkout_lld {

using namespace std;

class Cart {
public:
    explicit Cart(string userId) : userId_(std::move(userId)) {}

    const string &getUserId() const { return userId_; }

    // ⭐ Ek method se add / update / remove (qty<=0 -> hata do).
    void upsertItem(const Product &product, int quantity) {
        if (quantity <= 0) {
            items_.erase(product.getProductId()); // 0 qty line rakhne ka matlab nahi
            return;
        }
        auto it = items_.find(product.getProductId());
        if (it != items_.end()) {
            it->second = CartItem(product, quantity); // update (duplicate line nahi banti)
        } else {
            items_.emplace(product.getProductId(), CartItem(product, quantity));
        }
    }

    void removeItem(const string &productId) { items_.erase(productId); }
    void clear() { items_.clear(); } // successful checkout ke baad
    bool isEmpty() const { return items_.empty(); }

    const unordered_map<string, CartItem> &getItems() const { return items_; }

    // Har baar fresh sum (cached nahi -> stale hone ka risk nahi).
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
    string userId_;                            // kiska cart
    unordered_map<string, CartItem> items_;    // ⭐ productId -> item (duplicate-safe)
};

}  // namespace ecommerce_checkout_lld

#endif

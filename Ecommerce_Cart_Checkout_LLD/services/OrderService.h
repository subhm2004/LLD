// ============================================================================
//  services/OrderService.h  —  Orders ka repository (save + dhundo)
// ----------------------------------------------------------------------------
//  In-memory "order database". Ye REPOSITORY PATTERN hai: baaki system ko sirf
//  save/find dikhta hai, storage andar `unordered_map` hai. Kal ko real DB laga
//  do (SQL/Mongo) — sirf is class ka andar badlega, callers waise ke waise.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `findByClientRequestId` — IDEMPOTENCY ka lookup                       │
//  │                                                                          │
//  │  Checkout sabse pehle yahi poochta hai: "is request id se order pehle se  │
//  │  bana hua hai kya?" Mil gaya -> wahi purana order laut jaata hai, naya    │
//  │  order/payment hota hi nahi. Double-click ya network retry pe user ka     │
//  │  paisa DO baar nahi katta. (Order.h me poora scenario likha hai.)        │
//  │                                                                          │
//  │  ⚠ Abhi ye LINEAR SCAN hai — O(N) over saare orders. 10 orders pe theek,  │
//  │     10 lakh pe disaster. Fix aasan hai: ek aur map                        │
//  │     `unordered_map<clientRequestId, orderId>` maintain karo -> O(1).      │
//  │     (Real DB me clientRequestId pe UNIQUE INDEX daalte hain — ye index    │
//  │     race condition bhi rokta hai, sirf speed nahi.)                       │
//  │  📌 Interview me ye trade-off khud bolna: "abhi scan hai, index se O(1)". │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `getOrderOrThrow` ke DO overloads (const aur non-const) kyun?
//     - non-const: checkout ko order MODIFY karna hota hai (status update).
//     - const:     read-only caller (const OrderService& ke through) ko bhi
//                  order chahiye, par badalne ki permission nahi.
//     C++ me `const` overload isi tarah "read view" aur "write view" alag
//     karta hai. 📌 Const-correctness ka classic example.
//
//  ⭐ `getOrdersForUser` sort karta hai — kyunki `unordered_map` ka traversal
//     order RANDOM hota hai (build-to-build alag ho sakta). Sort ke bina order
//     history har baar alag sequence me dikhti = user confuse + tests flaky.
//     orderId "ORD_1", "ORD_2"... hai to string sort ≈ chronological.
//     ⚠ Caveat: ORD_10 < ORD_9 string comparison me (lexicographic). 100+ orders
//        pe zero-padding ya numeric id chahiye hogi.
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_ORDERSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_ORDERSERVICE_H

#include <algorithm>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../models/Order.h"

namespace ecommerce_checkout_lld {

using namespace std;

class OrderService {
public:
    // by-value + move: caller apna order "de deta" hai (copy nahi hoti).
    void saveOrder(Order order) { orders_.insert_or_assign(order.getOrderId(), std::move(order)); }

    // Non-const: caller order badal sakta hai (status/txnId update).
    Order &getOrderOrThrow(const string &orderId) {
        auto it = orders_.find(orderId);
        if (it == orders_.end()) {
            throw runtime_error("Order not found: " + orderId);
        }
        return it->second;
    }

    // ⭐ Const overload: read-only view (const-correctness — upar note).
    const Order &getOrderOrThrow(const string &orderId) const {
        auto it = orders_.find(orderId);
        if (it == orders_.end()) {
            throw runtime_error("Order not found: " + orderId);
        }
        return it->second;
    }

    // ⭐ Idempotency lookup. Nahi mila -> nullptr (ye ERROR nahi hai — pehli
    //    baar wali request ka normal case hai, isliye throw nahi karte).
    const Order *findByClientRequestId(const string &clientRequestId) const {
        if (clientRequestId.empty()) {
            return nullptr; // id di hi nahi -> idempotency guard off
        }
        for (const auto &entry : orders_) { // ⚠ O(N) — index se O(1) ho sakta (upar)
            if (entry.second.getClientRequestId() == clientRequestId) {
                return &entry.second;
            }
        }
        return nullptr;
    }

    // Order history — sort zaroori (unordered_map ka order random hota hai).
    vector<const Order *> getOrdersForUser(const string &userId) const {
        vector<const Order *> result;
        for (const auto &entry : orders_) {
            if (entry.second.getUserId() == userId) {
                result.push_back(&entry.second); // pointers — Order copy nahi hoti
            }
        }
        sort(result.begin(), result.end(), [](const Order *a, const Order *b) {
            return a->getOrderId() < b->getOrderId(); // ⭐ deterministic output
        });
        return result;
    }

private:
    unordered_map<string, Order> orders_; // orderId -> order (ye hi "DB" hai)
};

}  // namespace ecommerce_checkout_lld

#endif

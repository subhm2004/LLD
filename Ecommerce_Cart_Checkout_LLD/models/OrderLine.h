// ============================================================================
//  models/OrderLine.h  —  Order ki ek line (cart item ka SNAPSHOT)
// ----------------------------------------------------------------------------
//  Dekhne me CartItem jaisa hi lagta hai, par ek BADA farak hai:
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ CartItem vs OrderLine — dono alag kyun? (SNAPSHOT ka concept)         │
//  │                                                                          │
//  │     CartItem  -> `Product` object rakhta hai (live-ish, cart badalta rehta)│
//  │     OrderLine -> productId + productName + unitPrice ko COPY karke rakhta │
//  │                                                                          │
//  │  Order ban jaane ke BAAD agar product ka naam ya price catalog me badal   │
//  │  jaye, to PURANI order ka bill BADALNA NAHI CHAHIYE — invoice historical  │
//  │  record hai. Isliye OrderLine order ke waqt ka SNAPSHOT store karta hai   │
//  │  (naam + price copy). Agar order Product ko reference karta to kal ko     │
//  │  purani invoice pe naya price dikhta = accounting/legal problem. 😱       │
//  │                                                                          │
//  │  📌 Yahi wajah hai ki Order me bhi subtotal/discount/payable COMPUTE      │
//  │     karke STORE hote hain (har baar recalculate nahi hote).              │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Sirf getters (immutable) — order ban gaya to line kabhi badalti nahi.
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_ORDERLINE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_ORDERLINE_H

#include <string>
#include <utility>

namespace ecommerce_checkout_lld {

using namespace std;

class OrderLine {
public:
    OrderLine(string productId, string productName, double unitPrice, int quantity)
        : productId_(std::move(productId)),
          productName_(std::move(productName)), // ⭐ SNAPSHOT (order ke waqt ka naam)
          unitPrice_(unitPrice),                 // ⭐ SNAPSHOT (order ke waqt ka daam)
          quantity_(quantity) {}

    const string &getProductId() const { return productId_; }
    const string &getProductName() const { return productName_; }
    double getUnitPrice() const { return unitPrice_; }
    int getQuantity() const { return quantity_; }
    double lineTotal() const { return unitPrice_ * quantity_; }

private:
    string productId_;
    string productName_;   // catalog badle to bhi ye waisa hi rahega (invoice record)
    double unitPrice_{0.0};
    int quantity_{0};
};

}  // namespace ecommerce_checkout_lld

#endif

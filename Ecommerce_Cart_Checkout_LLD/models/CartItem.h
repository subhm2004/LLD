// ============================================================================
//  models/CartItem.h  —  Cart ki ek line: product + quantity
// ----------------------------------------------------------------------------
//  Ek cart item = kaunsa product + kitne. `lineTotal()` = price × qty.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Product ko VALUE me rakha (`Product product_`), pointer/reference nahi │
//  │                                                                          │
//  │  Yaani cart me product ki apni COPY hai. Kyun?                            │
//  │    - Catalog se cart ka data DECOUPLE ho jaata: agar admin catalog me     │
//  │      price badle, cart me pada purana price (jis pe user ne dekha tha)    │
//  │      waisa hi rahega — koi "surprise price change at checkout" nahi.      │
//  │    - Pointer hota to catalog se product hatate hi cart me DANGLING ptr.   │
//  │                                                                          │
//  │  ⚠ Trade-off: thodi memory duplicate (har cart me product copy). Real     │
//  │  system me productId + snapshot-price store karte hain. Interview-scale   │
//  │  pe value-copy simple aur safe hai.                                       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `setQuantity` me `max(0, quantity)` — negative quantity kabhi set nahi hoti
//     (chhota invariant guard).
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_CARTITEM_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_CARTITEM_H

#include <algorithm>

#include "Product.h"

namespace ecommerce_checkout_lld {

using namespace std;

class CartItem {
public:
    CartItem(const Product &product, int quantity) : product_(product), quantity_(quantity) {}

    const Product &getProduct() const { return product_; }
    int getQuantity() const { return quantity_; }

    // Negative qty kabhi nahi (clamp to 0).
    void setQuantity(int quantity) { quantity_ = max(0, quantity); }

    double lineTotal() const { return product_.getUnitPrice() * quantity_; }

private:
    Product product_;   // ⭐ COPY (catalog se decoupled — upar note)
    int quantity_{0};
};

}  // namespace ecommerce_checkout_lld

#endif

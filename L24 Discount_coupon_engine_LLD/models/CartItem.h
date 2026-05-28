#ifndef DISCOUNT_COUPON_LLD_MODELS_CARTITEM_H
#define DISCOUNT_COUPON_LLD_MODELS_CARTITEM_H

#include "Product.h"

namespace discount_coupon_lld {

// -----------------------------------------------------------------------------
// CartItem model
// Kya karta hai:
// - Ek Product + uski quantity ko cart line item ke form me represent karta hai.
// - Coupon calculation me item-level subtotal (price * qty) yahi se milta hai.
// -----------------------------------------------------------------------------
class CartItem {
    const Product *product;
    int quantity;

public:
    // prod pointer ownership CartItem ke paas nahi hai.
    // Product objects caller manage karta hai (main.cpp demo me manual delete hota hai).
    CartItem(const Product *prod, int qty) : product(prod), quantity(qty) {}

    // Item ka line-total (unit price * qty)
    double itemTotal() const { return product->getPrice() * quantity; }

    // Coupon classes ko product metadata (category) read karna hota hai.
    const Product *getProduct() const { return product; }
};

}

#endif

#ifndef DISCOUNT_COUPON_LLD_MODELS_CARTITEM_H
#define DISCOUNT_COUPON_LLD_MODELS_CARTITEM_H

#include "Product.h"

namespace discount_coupon_lld {

class CartItem {
    const Product *product;
    int quantity;

public:
    CartItem(const Product *prod, int qty) : product(prod), quantity(qty) {}

    double itemTotal() const { return product->getPrice() * quantity; }
    const Product *getProduct() const { return product; }
};

}

#endif

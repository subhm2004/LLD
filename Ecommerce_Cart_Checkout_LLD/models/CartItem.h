#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_CARTITEM_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_CARTITEM_H

#include <bits/stdc++.h>

#include "Product.h"

namespace ecommerce_checkout_lld {

using namespace std;

class CartItem {
public:
    CartItem(const Product &product, int quantity) : product_(product), quantity_(quantity) {}

    const Product &getProduct() const { return product_; }
    int getQuantity() const { return quantity_; }
    void setQuantity(int quantity) { quantity_ = max(0, quantity); }

    double lineTotal() const { return product_.getUnitPrice() * quantity_; }

private:
    Product product_;
    int quantity_{0};
};

}  // namespace ecommerce_checkout_lld

#endif

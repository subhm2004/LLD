#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_ORDERLINE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_ORDERLINE_H

#include <bits/stdc++.h>

namespace ecommerce_checkout_lld {

using namespace std;

class OrderLine {
public:
    OrderLine(string productId, string productName, double unitPrice, int quantity)
        : productId_(std::move(productId)),
          productName_(std::move(productName)),
          unitPrice_(unitPrice),
          quantity_(quantity) {}

    const string &getProductId() const { return productId_; }
    const string &getProductName() const { return productName_; }
    double getUnitPrice() const { return unitPrice_; }
    int getQuantity() const { return quantity_; }
    double lineTotal() const { return unitPrice_ * quantity_; }

private:
    string productId_;
    string productName_;
    double unitPrice_{0.0};
    int quantity_{0};
};

}  // namespace ecommerce_checkout_lld

#endif

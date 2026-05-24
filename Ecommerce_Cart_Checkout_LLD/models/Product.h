#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_PRODUCT_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_PRODUCT_H

#include <bits/stdc++.h>

namespace ecommerce_checkout_lld {

using namespace std;

class Product {
public:
    Product(string productId, string name, string category, double unitPrice)
        : productId_(std::move(productId)),
          name_(std::move(name)),
          category_(std::move(category)),
          unitPrice_(unitPrice) {}

    const string &getProductId() const { return productId_; }
    const string &getName() const { return name_; }
    const string &getCategory() const { return category_; }
    double getUnitPrice() const { return unitPrice_; }

private:
    string productId_;
    string name_;
    string category_;
    double unitPrice_;
};

}  // namespace ecommerce_checkout_lld

#endif

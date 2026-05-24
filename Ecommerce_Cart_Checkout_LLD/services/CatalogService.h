#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_CATALOGSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_CATALOGSERVICE_H

#include <bits/stdc++.h>

#include "../models/Product.h"

namespace ecommerce_checkout_lld {

using namespace std;

class CatalogService {
public:
    void addProduct(const Product &product) {
        products_.insert_or_assign(product.getProductId(), product);
    }

    const Product &getProductOrThrow(const string &productId) const {
        auto it = products_.find(productId);
        if (it == products_.end()) {
            throw runtime_error("Product not found: " + productId);
        }
        return it->second;
    }

    vector<const Product *> searchByCategory(const string &category) const {
        vector<const Product *> result;
        for (const auto &entry : products_) {
            if (entry.second.getCategory() == category) {
                result.push_back(&entry.second);
            }
        }
        return result;
    }

private:
    unordered_map<string, Product> products_;
};

}  // namespace ecommerce_checkout_lld

#endif

#ifndef BLINKIT_LLD_MODELS_PRODUCT_H
#define BLINKIT_LLD_MODELS_PRODUCT_H

#include <bits/stdc++.h>

using namespace std;

namespace blinkit_lld {

class Product {
public:
    Product(int sku, string name, double price) : sku_(sku), name_(std::move(name)), price_(price) {}

    int getSku() const { return sku_; }
    const string &getName() const { return name_; }
    double getPrice() const { return price_; }

private:
    int sku_;
    string name_;
    double price_;
};

class ProductFactory {
public:
    static Product *createProduct(int sku) {
        if (sku == 101) return new Product(101, "Apple", 20.0);
        if (sku == 102) return new Product(102, "Banana", 10.0);
        if (sku == 103) return new Product(103, "Chocolate", 50.0);
        if (sku == 201) return new Product(201, "T-Shirt", 500.0);
        if (sku == 202) return new Product(202, "Jeans", 1000.0);
        return new Product(sku, "Item" + to_string(sku), 100.0);
    }
};

} // namespace blinkit_lld

#endif // BLINKIT_LLD_MODELS_PRODUCT_H

#ifndef DISCOUNT_COUPON_LLD_MODELS_PRODUCT_H
#define DISCOUNT_COUPON_LLD_MODELS_PRODUCT_H

#include <string>

namespace discount_coupon_lld {

class Product {
    std::string name;
    std::string category;
    double price;

public:
    Product(std::string productName, std::string productCategory, double productPrice)
        : name(std::move(productName)), category(std::move(productCategory)), price(productPrice) {}

    const std::string &getName() const { return name; }
    const std::string &getCategory() const { return category; }
    double getPrice() const { return price; }
};

}

#endif

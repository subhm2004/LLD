#ifndef DISCOUNT_COUPON_LLD_MODELS_PRODUCT_H
#define DISCOUNT_COUPON_LLD_MODELS_PRODUCT_H

#include <string>

namespace discount_coupon_lld {

// -----------------------------------------------------------------------------
// Product model
// Kya karta hai:
// - Cart me add hone wale ek sellable item ka immutable data hold karta hai.
// - Coupon rules (jaise SeasonalOffer) category check karne ke liye yahi model use karti hain.
// -----------------------------------------------------------------------------
class Product {
    std::string name;
    std::string category;
    double price;

public:
    // productName: display name (e.g. "Winter Jacket")
    // productCategory: coupon targeting ke liye (e.g. "Clothing")
    // productPrice: per-unit price
    Product(std::string productName, std::string productCategory, double productPrice)
        : name(std::move(productName)), category(std::move(productCategory)), price(productPrice) {}

    // Read-only getters:
    // const reference return kar rahe hain taaki unnecessary string copy na ho.
    const std::string &getName() const { return name; }
    const std::string &getCategory() const { return category; }
    double getPrice() const { return price; }
};

}

#endif

// ============================================================================
//  models/Product.h  —  Catalog ka ek product
// ----------------------------------------------------------------------------
//  Product ki basic details: id, naam, category, aur per-unit price.
//
//  ⭐ IMMUTABLE — sirf getters, koi setter. Ek baar catalog me product aa gaya,
//     uski details is object me badalti nahi. Fayda: product cart me COPY hoke
//     jaata hai (CartItem me), to agar koi beech me price badal deta to cart aur
//     catalog ka data mismatch ho jaata. Immutable rakhne se ye confusion nahi.
//
//  📌 Notice: Product me STOCK nahi hai! Stock alag service (InventoryService) me
//     hai. Kyun? Kyunki product ki DEFINITION (naam/price) aur uski AVAILABILITY
//     (kitne bache) do alag concerns hain — stock har waqt badalta hai, product
//     details nahi. Alag rakhne se dono independently evolve karte (SRP).
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_PRODUCT_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_PRODUCT_H

#include <string>
#include <utility>

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
    const string &getCategory() const { return category_; } // search-by-category ke liye
    double getUnitPrice() const { return unitPrice_; }      // cart/order pricing ka base

private:
    string productId_;   // "P1" — unique
    string name_;        // "Wireless Mouse"
    string category_;    // "Electronics"
    double unitPrice_;   // per-unit daam (stock yahan NAHI — upar note)
};

}  // namespace ecommerce_checkout_lld

#endif

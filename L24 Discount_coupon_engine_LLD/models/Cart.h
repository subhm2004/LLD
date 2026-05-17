#ifndef DISCOUNT_COUPON_LLD_MODELS_CART_H
#define DISCOUNT_COUPON_LLD_MODELS_CART_H

#include <string>
#include <vector>

#include "CartItem.h"

namespace discount_coupon_lld {

class Cart {
    std::vector<CartItem *> items;
    double originalTotal = 0.0;
    double currentTotal = 0.0;
    bool loyaltyMember = false;
    std::string paymentBank;

public:
    void addProduct(const Product *prod, int qty = 1) {
        auto *item = new CartItem(prod, qty);
        items.push_back(item);
        originalTotal += item->itemTotal();
        currentTotal += item->itemTotal();
    }

    double getOriginalTotal() const { return originalTotal; }
    double getCurrentTotal() const { return currentTotal; }

    void applyDiscount(double discount) {
        currentTotal -= discount;
        if (currentTotal < 0) {
            currentTotal = 0;
        }
    }

    void setLoyaltyMember(bool member) { loyaltyMember = member; }
    bool isLoyaltyMember() const { return loyaltyMember; }

    void setPaymentBank(const std::string &bank) { paymentBank = bank; }
    const std::string &getPaymentBank() const { return paymentBank; }

    const std::vector<CartItem *> &getItems() const { return items; }

    ~Cart() {
        for (CartItem *item : items) {
            delete item;
        }
    }
};

}

#endif

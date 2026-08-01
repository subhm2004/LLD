// ============================================================================
//  models/Cart.h — Cart model (coupons ka "playground" 🛒)
// ----------------------------------------------------------------------------
//  Items ki list + DO totals + eligibility flags. DO totals kyun?! Ye is
//  design ka sabse smart hissa hai:
//    originalTotal = pre-discount total — KABHI nahi badalta.
//                    Threshold checks (bulk min 1000, bank min 2000) ISI
//                    pe hote hain — taaki pehle lage discounts kisi coupon
//                    ko unfairly disqualify na kar dein!
//    currentTotal  = running total — har coupon ke discount se GHATTA
//                    jaata hai. Agla coupon ka % isi pe lagta hai (stacking).
//  Eligibility metadata bhi yahin: loyaltyMember flag + paymentBank string.
//  Cart CartItems ka MALIK hai (khud banata hai) -> destructor me delete.
// ============================================================================
#ifndef DISCOUNT_COUPON_LLD_MODELS_CART_H
#define DISCOUNT_COUPON_LLD_MODELS_CART_H

#include <string>
#include <vector>

#include "CartItem.h"

namespace discount_coupon_lld {

// -----------------------------------------------------------------------------
// Cart model
// Kya karta hai:
// - Cart ke items hold karta hai.
// - Original total (pre-discount) aur current total (post-discount running state) track karta hai.
// - Eligibility metadata store karta hai:
//   1) loyalty member flag
//   2) payment bank
// -----------------------------------------------------------------------------
class Cart {
    std::vector<CartItem *> items;
    // originalTotal: threshold checks ke liye stable base (bulk/bank min spend)
    double originalTotal = 0.0;
    // currentTotal: chain me discount apply hone ke baad mutate hota rehta hai
    double currentTotal = 0.0;
    bool loyaltyMember = false;
    std::string paymentBank;

public:
    // Product cart me add karo:
    // - new CartItem banega
    // - original + current totals dono update honge
    void addProduct(const Product *prod, int qty = 1) {
        auto *item = new CartItem(prod, qty);
        items.push_back(item);
        originalTotal += item->itemTotal();
        currentTotal += item->itemTotal();
    }

    // Reporting/eligibility getters
    double getOriginalTotal() const { return originalTotal; }
    double getCurrentTotal() const { return currentTotal; }

    // Coupon engine current total ko reduce karta hai.
    // Guard: total negative nahi jana chahiye.
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

    // Coupon classes ko cart items iterate karne ke liye read-only list milti hai.
    const std::vector<CartItem *> &getItems() const { return items; }

    // CartItem allocations ka cleanup
    ~Cart() {
        for (CartItem *item : items) {
            delete item;
        }
    }
};

}

#endif

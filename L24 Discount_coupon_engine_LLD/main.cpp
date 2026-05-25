#include <bits/stdc++.h>

#include "core/CouponManager.h"
#include "coupons/BankingCoupon.h"
#include "coupons/BulkPurchaseDiscount.h"
#include "coupons/LoyaltyDiscount.h"
#include "coupons/SeasonalOffer.h"
#include "models/Cart.h"
#include "models/Product.h"

using namespace std;
using namespace discount_coupon_lld;

// ---------------------------------------------------------------------------
// Helpers — readable demo output
// ---------------------------------------------------------------------------
static void printBanner(const string &title) {
    cout << "\n========================================\n";
    cout << "  " << title << "\n";
    cout << "========================================\n";
}

static void printApplicable(CouponManager &mgr, Cart &cart) {
    cout << "Original cart total : " << cart.getOriginalTotal() << " Rs\n";
    cout << "Loyalty member      : " << (cart.isLoyaltyMember() ? "yes" : "no") << "\n";
    cout << "Payment bank        : " << (cart.getPaymentBank().empty() ? "(none)" : cart.getPaymentBank())
         << "\n\n";

    vector<string> applicable = mgr.getApplicable(&cart);
    cout << "Applicable coupons (" << applicable.size() << "):\n";
    if (applicable.empty()) {
        cout << "  (none)\n";
    }
    for (const string &name : applicable) {
        cout << "  - " << name << "\n";
    }
}

static void applyAndShowFinal(CouponManager &mgr, Cart &cart, const string &note) {
    cout << "\n" << note << "\n";
    double finalTotal = mgr.applyAll(&cart);
    cout << "Final cart total    : " << finalTotal << " Rs\n";
}

// ---------------------------------------------------------------------------
// main — tests every coupon type + key behaviours from requirements.md
// ---------------------------------------------------------------------------
int main() {
    printBanner("Discount Coupon Engine — full feature demo");

    // -----------------------------------------------------------------------
    // STEP 1: Register ALL coupon types at RUNTIME (FR #4, #7)
    // Chain order matters: combinable coupons first, EXCLUSIVE banking LAST.
    // -----------------------------------------------------------------------
    printBanner("Step 1 — Runtime coupon registration (4 types)");

    CouponManager &mgr = CouponManager::getInstance();

    // (A) SeasonalOffer — CATEGORY-level, PERCENTAGE strategy (FR #8, #5)
    //     10% off only Clothing line items (not whole cart).
    mgr.registerCoupon(new SeasonalOffer(10, "Clothing"));
    cout << "Registered: SeasonalOffer (10% off Clothing category)\n";

    // (B) LoyaltyDiscount — CART-level, PERCENTAGE strategy (FR #9, #5)
    //     5% off current cart total when user is loyalty member.
    mgr.registerCoupon(new LoyaltyDiscount(5));
    cout << "Registered: LoyaltyDiscount (5% cart-level, loyalty only)\n";

    // (C) BulkPurchaseDiscount — CART-level, FLAT strategy (FR #10, #6)
    //     Rs 100 off when original cart total >= Rs 1000.
    mgr.registerCoupon(new BulkPurchaseDiscount(1000, 100));
    cout << "Registered: BulkPurchaseDiscount (flat Rs 100, min cart 1000)\n";

    // (D) BankingCoupon — CART-level, PERCENT WITH CAP strategy (FR #11, #6)
    //     15% off up to Rs 500 if bank == ABC and spend >= 2000.
    //     isCombinable() == false → stops chain after this coupon (FR #15).
    mgr.registerCoupon(new BankingCoupon("ABC", 2000, 15, 500));
    cout << "Registered: BankingCoupon (15% capped, bank ABC, EXCLUSIVE)\n";

    // -----------------------------------------------------------------------
    // DEMO 1: Full cart — ALL four coupons should apply (then chain stops)
    // Tests: flat + % + % with cap, cart + category level, combinable stack,
    //        applicable list, final total (FR #12–#13)
    // -----------------------------------------------------------------------
    printBanner("Demo 1 — Full cart (all coupon types applicable)");

    Product *jacket = new Product("Winter Jacket", "Clothing", 1000);
    Product *phone = new Product("Smartphone", "Electronics", 20000);
    Product *jeans = new Product("Jeans", "Clothing", 1000);
    Product *headphones = new Product("Headphones", "Electronics", 2000);

    Cart fullCart;
    fullCart.addProduct(jacket, 1);   // Clothing  → Seasonal eligible
    fullCart.addProduct(phone, 1);    // Electronics
    fullCart.addProduct(jeans, 2);    // Clothing  → more Seasonal base
    fullCart.addProduct(headphones, 1);
    fullCart.setLoyaltyMember(true);  // Loyalty eligible
    fullCart.setPaymentBank("ABC"); // Banking eligible (total 25000 >= 2000)

    printApplicable(mgr, fullCart);
    applyAndShowFinal(mgr, fullCart,
                      "Apply chain: Seasonal → Loyalty → Bulk → Banking (exclusive stop)\n"
                      "Expected: 4 discounts, then 'stopping coupon chain' message");

    // -----------------------------------------------------------------------
    // DEMO 2: Small cart — only SOME coupons applicable (FR #14 skip others)
    // Tests: ineligible coupons skipped (no loyalty, wrong bank, below bulk min)
    // -----------------------------------------------------------------------
    printBanner("Demo 2 — Small cart (only Seasonal should apply)");

    Product *tshirt = new Product("T-Shirt", "Clothing", 500);

    Cart smallCart;
    smallCart.addProduct(tshirt, 1);       // Clothing only, total 500
    smallCart.setLoyaltyMember(false);     // Loyalty NOT applicable
    smallCart.setPaymentBank("HDFC");      // Banking NOT applicable (needs ABC)

    printApplicable(mgr, smallCart);
    applyAndShowFinal(mgr, smallCart,
                      "Apply chain: only Seasonal runs; others skipped (not applicable)");

    // -----------------------------------------------------------------------
    // DEMO 3: Medium cart — combinable stack WITHOUT banking
    // Tests: Loyalty + Bulk stack; Banking skipped (wrong bank); no exclusive stop
    // -----------------------------------------------------------------------
    printBanner("Demo 3 — Medium cart (Loyalty + Bulk, no Banking)");

    Product *shoes = new Product("Shoes", "Clothing", 800);
    Product *watch = new Product("Watch", "Electronics", 1500);

    Cart mediumCart;
    mediumCart.addProduct(shoes, 2);       // 1600 Clothing
    mediumCart.addProduct(watch, 1);       // total 3100 >= 1000 bulk
    mediumCart.setLoyaltyMember(true);
    mediumCart.setPaymentBank("ICICI");    // Banking skipped

    printApplicable(mgr, mediumCart);
    applyAndShowFinal(mgr, mediumCart,
                      "Apply chain: Seasonal + Loyalty + Bulk; Banking skipped (wrong bank)");

    // -----------------------------------------------------------------------
    // DEMO 4: Electronics-only cart — Seasonal skipped (wrong category)
    // Tests: category-level coupon not applied when no Clothing in cart
    // -----------------------------------------------------------------------
    printBanner("Demo 4 — Electronics only (Seasonal not applicable)");

    Product *laptop = new Product("Laptop", "Electronics", 50000);

    Cart electronicsCart;
    electronicsCart.addProduct(laptop, 1);
    electronicsCart.setLoyaltyMember(true);
    electronicsCart.setPaymentBank("ABC"); // Banking applicable (50000 >= 2000)

    printApplicable(mgr, electronicsCart);
    applyAndShowFinal(mgr, electronicsCart,
                      "Apply chain: Loyalty + Bulk + Banking; Seasonal skipped (no Clothing)");

    // Cleanup
    delete jacket;
    delete phone;
    delete jeans;
    delete headphones;
    delete tshirt;
    delete shoes;
    delete watch;
    delete laptop;

    printBanner("Demo complete — strategies: FLAT | PERCENT | PERCENT_WITH_CAP");
    return 0;
}

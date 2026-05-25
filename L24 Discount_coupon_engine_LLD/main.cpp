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

int main() {
    CouponManager &mgr = CouponManager::getInstance();
    // Chain order: combinable first, exclusive Banking last (stops further coupons)
    mgr.registerCoupon(new SeasonalOffer(10, "Clothing"));
    mgr.registerCoupon(new LoyaltyDiscount(5));
    mgr.registerCoupon(new BulkPurchaseDiscount(1000, 100));
    mgr.registerCoupon(new BankingCoupon("ABC", 2000, 15, 500));

    Product *p1 = new Product("Winter Jacket", "Clothing", 1000);
    Product *p2 = new Product("Smartphone", "Electronics", 20000);
    Product *p3 = new Product("Jeans", "Clothing", 1000);
    Product *p4 = new Product("Headphones", "Electronics", 2000);

    Cart cart;
    cart.addProduct(p1, 1);
    cart.addProduct(p2, 1);
    cart.addProduct(p3, 2);
    cart.addProduct(p4, 1);
    cart.setLoyaltyMember(true);
    cart.setPaymentBank("ABC");

    cout << "Original Cart Total: " << cart.getOriginalTotal() << " Rs\n";

    vector<string> applicable = mgr.getApplicable(&cart);
    cout << "Applicable Coupons:\n";
    for (const string &couponName : applicable) {
        cout << " - " << couponName << '\n';
    }

    cout << "\nApplying coupon chain (Banking is exclusive if applicable):\n";
    double finalTotal = mgr.applyAll(&cart);
    cout << "Final Cart Total after discounts: " << finalTotal << " Rs\n";

    delete p1;
    delete p2;
    delete p3;
    delete p4;
    return 0;
}

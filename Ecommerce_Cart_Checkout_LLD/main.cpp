#include <bits/stdc++.h>

#include "core/EcommerceCheckoutSystem.h"

using namespace std;
using namespace ecommerce_checkout_lld;

int main() {
    EcommerceCheckoutSystem system;
    system.setCurrentDay(10);

    system.registerUser(User("U1", "Shubham", "shubham@email.com"));
    system.addAddress(Address("A1", "U1", "221B Baker Street", "Jaipur", "302001"));

    system.addProduct(Product("P1", "Wireless Mouse", "Electronics", 799.0), 50);
    system.addProduct(Product("P2", "Mechanical Keyboard", "Electronics", 3499.0), 20);
    system.addProduct(Product("P3", "USB-C Cable", "Accessories", 299.0), 100);

    cout << "=== Add items to cart ===\n";
    system.addToCart("U1", "P1", 2);
    system.addToCart("U1", "P2", 1);
    system.printCart("U1");

    cout << "\n=== Checkout with UPI + SAVE10 coupon ===\n";
    Order order1 = system.checkout("U1", "A1", PaymentMethod::UPI, "shubham@okbank", "SAVE10",
                                   "REQ_CHECKOUT_001");
    system.printOrder(order1.getOrderId());

    cout << "\n=== Idempotent retry with same clientRequestId ===\n";
    Order order1Retry = system.checkout("U1", "A1", PaymentMethod::UPI, "shubham@okbank", "SAVE10",
                                        "REQ_CHECKOUT_001");
    cout << "Same order returned: " << (order1Retry.getOrderId() == order1.getOrderId() ? "yes" : "no")
         << "\n";

    cout << "\n=== Build cart again for payment failure demo ===\n";
    system.addToCart("U1", "P2", 1);
    system.printCart("U1");

    cout << "\n=== Card payment decline (ends with 0000) ===\n";
    try {
        system.checkout("U1", "A1", PaymentMethod::CREDIT_CARD, "4111111111110000");
    } catch (const exception &ex) {
        cout << "Expected failure: " << ex.what() << "\n";
    }
    cout << "Stock restored after failed payment — P2 available: "
         << system.getAvailableStock("P2") << "\n";
    system.printCart("U1");

    cout << "\n=== Successful card checkout ===\n";
    Order order2 = system.checkout("U1", "A1", PaymentMethod::CREDIT_CARD, "4111111111114242");
    system.printOrder(order2.getOrderId());

    cout << "\n=== COD checkout with free shipping (cart >= 999) ===\n";
    system.addToCart("U1", "P2", 1);
    system.addToCart("U1", "P3", 2);
    system.printCart("U1");
    Order order3 = system.checkout("U1", "A1", PaymentMethod::COD, "NA", "FLAT100");
    system.printOrder(order3.getOrderId());

    cout << "\n=== Out-of-stock attempt ===\n";
    system.addToCart("U1", "P2", 100);
    try {
        system.checkout("U1", "A1", PaymentMethod::UPI, "shubham@okbank");
    } catch (const exception &ex) {
        cout << "Expected failure: " << ex.what() << "\n";
    }

    cout << "\nEcommerce_Cart_Checkout_LLD demo completed.\n";
    return 0;
}

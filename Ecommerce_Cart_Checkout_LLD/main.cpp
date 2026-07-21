// ============================================================================
//  main.cpp  —  Ecommerce Cart & Checkout ka DEMO (7 scenarios)
// ----------------------------------------------------------------------------
//  Dekho main sirf FACADE se baat karta hai — 9 services me se ek ka bhi naam
//  yahan nahi hai. Yahi Facade pattern ka poora point hai.
//
//  Demo me ye 7 cheezein prove hoti hain:
//     1. Cart me items add (P1 x2, P2 x1) — subtotal kaise banta hai
//     2. UPI + SAVE10 (10% off) se successful checkout
//     3. ⭐ IDEMPOTENCY — wahi clientRequestId dobara -> WAHI order (naya nahi)
//     4. ⭐ PAYMENT FAIL (card "0000" pe khatam) -> stock WAPAS aata hai,
//           aur cart bhi SALAAMAT rehta hai (clearCart sirf success pe)
//     5. Successful card payment ("4242" wala)
//     6. COD + FLAT100 -> cart >= 999 to shipping FREE, aur order CONFIRMED
//           par payment PENDING (do alag status ka fayda)
//     7. ⭐ OUT OF STOCK -> reserve step pe hi saaf error
// ============================================================================
#include <exception>
#include <iostream>

#include "core/EcommerceCheckoutSystem.h"

using namespace std;
using namespace ecommerce_checkout_lld;

int main() {
    EcommerceCheckoutSystem system;
    system.setCurrentDay(10); // coupons ki expiry day 365 hai -> abhi valid

    // ---- Setup: user + address + catalog(with stock) -----------------------
    system.registerUser(User("U1", "Shubham", "shubham@email.com"));
    system.addAddress(Address("A1", "U1", "221B Baker Street", "Jaipur", "302001"));

    system.addProduct(Product("P1", "Wireless Mouse", "Electronics", 799.0), 50);
    system.addProduct(Product("P2", "Mechanical Keyboard", "Electronics", 3499.0), 20);
    system.addProduct(Product("P3", "USB-C Cable", "Accessories", 299.0), 100);

    // ---- (1) Cart bharo ----------------------------------------------------
    cout << "=== Add items to cart ===\n";
    system.addToCart("U1", "P1", 2); // 799 x 2
    system.addToCart("U1", "P2", 1); // 3499 x 1
    system.printCart("U1");

    // ---- (2) UPI + SAVE10 (10% discount) -----------------------------------
    cout << "\n=== Checkout with UPI + SAVE10 coupon ===\n";
    Order order1 = system.checkout("U1", "A1", PaymentMethod::UPI, "shubham@okbank", "SAVE10",
                                   "REQ_CHECKOUT_001"); // <- idempotency key
    system.printOrder(order1.getOrderId());

    // ---- (3) ⭐ Wahi request dobara -> naya order NAHI banega ---------------
    // Real duniya me: user ne double-click kiya / network ne retry maara.
    cout << "\n=== Idempotent retry with same clientRequestId ===\n";
    Order order1Retry = system.checkout("U1", "A1", PaymentMethod::UPI, "shubham@okbank", "SAVE10",
                                        "REQ_CHECKOUT_001");
    cout << "Same order returned: " << (order1Retry.getOrderId() == order1.getOrderId() ? "yes" : "no")
         << "\n"; // "yes" -> paisa DOBARA nahi kata ✅

    cout << "\n=== Build cart again for payment failure demo ===\n";
    system.addToCart("U1", "P2", 1);
    system.printCart("U1");

    // ---- (4) ⭐ Payment fail -> ROLLBACK ka proof ---------------------------
    // CardPaymentStrategy me rule: card "0000" pe khatam -> decline.
    cout << "\n=== Card payment decline (ends with 0000) ===\n";
    try {
        system.checkout("U1", "A1", PaymentMethod::CREDIT_CARD, "4111111111110000");
    } catch (const exception &ex) {
        cout << "Expected failure: " << ex.what() << "\n";
    }
    // ⭐ Stock wapas aa gaya (releaseReservation ne kaam kiya) — atka nahi.
    cout << "Stock restored after failed payment — P2 available: "
         << system.getAvailableStock("P2") << "\n";
    // ⭐ Aur cart bhi bacha hua hai (clearCart sirf SUCCESS pe hota hai) —
    //    user dobara try kar sakta hai, saman phir se daalne ki zaroorat nahi.
    system.printCart("U1");

    // ---- (5) Successful card payment ---------------------------------------
    cout << "\n=== Successful card checkout ===\n";
    Order order2 = system.checkout("U1", "A1", PaymentMethod::CREDIT_CARD, "4111111111114242");
    system.printOrder(order2.getOrderId());

    // ---- (6) COD + FLAT100 -> free shipping + PENDING payment --------------
    cout << "\n=== COD checkout with free shipping (cart >= 999) ===\n";
    system.addToCart("U1", "P2", 1); // 3499
    system.addToCart("U1", "P3", 2); // 299 x 2 = 598  -> subtotal 4097
    system.printCart("U1");
    // 4097 - 100 (FLAT100) = 3997 >= 999 -> shipping FREE -> payable 3997
    Order order3 = system.checkout("U1", "A1", PaymentMethod::COD, "NA", "FLAT100");
    // ⭐ Output me dekho: Status=CONFIRMED par Payment=PENDING (COD ka case)
    system.printOrder(order3.getOrderId());

    // ---- (7) ⭐ Out of stock -> reserve step pe hi rok diya -----------------
    cout << "\n=== Out-of-stock attempt ===\n";
    system.addToCart("U1", "P2", 100); // stock itna hai hi nahi
    try {
        system.checkout("U1", "A1", PaymentMethod::UPI, "shubham@okbank");
    } catch (const exception &ex) {
        // Error message me: kaunsa product, kitna maanga, kitna available
        cout << "Expected failure: " << ex.what() << "\n";
    }

    cout << "\nEcommerce_Cart_Checkout_LLD demo completed.\n";
    return 0;
}

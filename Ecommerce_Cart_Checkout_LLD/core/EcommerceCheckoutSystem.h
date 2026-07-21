// ============================================================================
//  core/EcommerceCheckoutSystem.h  —  FACADE (poore system ka ek hi darwaza)
// ----------------------------------------------------------------------------
//  Andar 9 services hain. Bahar wale (main.cpp / API layer) ko unme se ek ka bhi
//  naam nahi pata — wo bas `system.addToCart(...)`, `system.checkout(...)` bulata
//  hai. Yahi FACADE PATTERN hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Facade kya karta hai (aur kya NAHI)                                   │
//  │                                                                          │
//  │   KARTA HAI:                                                             │
//  │     - services ko OWN karta hai aur unhe aapas me WIRE karta hai          │
//  │     - cross-cutting validation: user hai? address usi user ka hai?        │
//  │     - system-level state: currentDay_, orderCounter_                      │
//  │     - checkout ke baad notification bhejna                               │
//  │     - printing/demo helpers                                              │
//  │                                                                          │
//  │   NAHI KARTA:                                                            │
//  │     - koi business math (wo Pricing/Coupon/Payment services me hai)       │
//  │  📌 Facade "coordinator" hota hai, "God object" nahi. Logic andar wali    │
//  │     services me hi rehna chahiye.                                        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠⚠ BUG FIX: copy/move `= delete` — ye ek ASLI dangling-reference bug tha │
//  │                                                                          │
//  │  PEHLE kya tha: ye class implicitly MOVABLE thi (compiler auto move       │
//  │  constructor bana deta tha, kyunki koi member move ko block nahi karta).  │
//  │  Test karke CONFIRM kiya tha — ye line COMPILE ho jaati thi:              │
//  │                                                                          │
//  │      EcommerceCheckoutSystem a;                                          │
//  │      EcommerceCheckoutSystem b = std::move(a);   // ✅ compile ho gaya 😱  │
//  │                                                                          │
//  │  PROBLEM KYA THA: `cartService_` andar `CatalogService&` rakhta hai, aur  │
//  │  `checkoutService_` 6 services ke REFERENCES rakhta hai — sab SIBLING     │
//  │  MEMBERS hain (isi object ke andar ke). Move karne pe references COPY     │
//  │  hoti hain, RE-BIND nahi hoti. Matlab `b.checkoutService_` abhi bhi       │
//  │  `a.orderService_`, `a.inventory_` ko point karta rahega!                 │
//  │                                                                          │
//  │      b.checkout(...) -> stock 'a' ka ghata, order 'a' me save hua,        │
//  │                        aur 'b' ka apna orderService_ khaali. 🤯           │
//  │      Aur `a` scope se bahar gaya -> `b` ki saari references DANGLING      │
//  │      -> use-after-free (undefined behavior, silent corruption).           │
//  │                                                                          │
//  │  ⭐ ASLI WAJAH: C++ ka default move/copy MEMBERWISE hota hai — usse pata  │
//  │     hi nahi ki hamare members aapas me jude hue hain (self-referential).  │
//  │                                                                          │
//  │  FIX: copy aur move dono `= delete`. Ab galti se move likha to COMPILE    │
//  │  ERROR aayega — runtime pe crash hone ka mauka hi nahi milega.            │
//  │  📌 RULE: jis class ke members ek dusre ko reference/point karte hon,     │
//  │     wo class by-default NA copy honi chahiye NA move. Zaroorat pade to    │
//  │     custom move likho jo references RE-BIND kare (mushkil hai) — ya       │
//  │     unique_ptr members use karo taaki pate badle hi na.                  │
//  │     (Yahi bug In_Memory_SQL_Database_LLD me bhi tha — same fix.)         │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ MEMBER DECLARATION ORDER — yahan ye critical hai!                     │
//  │                                                                          │
//  │  C++ me members declaration ke ORDER me construct hote hain (constructor  │
//  │  ki init-list ke order me NAHI). Isliye:                                  │
//  │     catalog_ ... orderService_  (jinke references chahiye)                │
//  │     PHIR cartService_ (catalog_ ka ref leta hai)                          │
//  │     PHIR checkoutService_ (6 services ke ref lete hai)                    │
//  │  Agar `checkoutService_` upar declare hota to wo un members ke references │
//  │  leta jo ABHI BANE HI NAHI (uninitialized memory) = UB. 😱                │
//  │  📌 Isi liye `-Wreorder` warning ko kabhi ignore mat karna.               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `checkout()` me idempotency check DO baar hai (yahan bhi, CheckoutService
//     me bhi). Redundant nahi hai — yahan wala check EARLY return karta hai
//     taaki duplicate request pe notification DOBARA na bheje. (Andar wala check
//     tab bhi kaam aata hai jab koi CheckoutService ko seedha use kare.)
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_CORE_ECOMMERCECHECKOUTSYSTEM_H
#define ECOMMERCE_CART_CHECKOUT_LLD_CORE_ECOMMERCECHECKOUTSYSTEM_H

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "../enums/PaymentMethod.h"
#include "../models/Address.h"
#include "../models/Coupon.h"
#include "../models/Order.h"
#include "../models/Product.h"
#include "../models/User.h"
#include "../services/CartService.h"
#include "../services/CatalogService.h"
#include "../services/CheckoutService.h"
#include "../services/CouponService.h"
#include "../services/InventoryService.h"
#include "../services/NotificationService.h"
#include "../services/OrderService.h"
#include "../services/PaymentService.h"
#include "../services/PricingService.h"
#include "../strategies/FlatDiscountStrategy.h"
#include "../strategies/PercentageDiscountStrategy.h"

namespace ecommerce_checkout_lld {

using namespace std;

class EcommerceCheckoutSystem {
public:
    // Constructor sab kuch WIRE karta hai (dependency injection ka assembly point).
    EcommerceCheckoutSystem()
        : cartService_(catalog_),                                    // catalog ka ref
          checkoutService_(cartService_, inventory_, couponService_, pricingService_,
                           paymentService_, orderService_) {         // 6 services ke ref
        seedDefaultCoupons();
    }

    // ⚠⚠ BUG FIX: self-referential members ki wajah se copy/move BANNED (upar note).
    // Ye 4 lines na hoti to `auto b = std::move(a);` chup-chaap compile ho jaata
    // aur runtime pe dangling references milte.
    EcommerceCheckoutSystem(const EcommerceCheckoutSystem &) = delete;
    EcommerceCheckoutSystem &operator=(const EcommerceCheckoutSystem &) = delete;
    EcommerceCheckoutSystem(EcommerceCheckoutSystem &&) = delete;
    EcommerceCheckoutSystem &operator=(EcommerceCheckoutSystem &&) = delete;

    // ---- Setup APIs --------------------------------------------------------
    void registerUser(const User &user) { users_.insert_or_assign(user.getUserId(), user); }

    // Address hamesha kisi VALID user ka hi hona chahiye (orphan address nahi).
    void addAddress(const Address &address) {
        if (!users_.count(address.getUserId())) {
            throw runtime_error("User not found: " + address.getUserId());
        }
        addresses_.insert_or_assign(address.getAddressId(), address);
    }

    // ⭐ Product + stock ek saath — kyunki ye do ALAG services me jaate hain
    // (catalog = details, inventory = stock). Facade dono ko sync me rakhta hai
    // taaki caller se ek call chhoot na jaye (aisa product na bane jiska stock hi na ho).
    void addProduct(const Product &product, int stockQuantity) {
        catalog_.addProduct(product);
        inventory_.setStock(product.getProductId(), stockQuantity);
    }

    // ---- Cart APIs (har ek pehle user validate karta hai) ------------------
    void addToCart(const string &userId, const string &productId, int quantity) {
        validateUser(userId);
        cartService_.addToCart(userId, productId, quantity);
    }

    void updateCartQuantity(const string &userId, const string &productId, int quantity) {
        validateUser(userId);
        cartService_.updateQuantity(userId, productId, quantity);
    }

    void removeFromCart(const string &userId, const string &productId) {
        validateUser(userId);
        cartService_.removeFromCart(userId, productId);
    }

    // Coupon move-only hai (unique_ptr strategy) -> by-value + move.
    void registerCoupon(Coupon coupon) { couponService_.registerCoupon(std::move(coupon)); }

    // Simplified calendar — coupon expiry test karne ke liye "aaj ka din" set karo.
    void setCurrentDay(int day) {
        if (day < 0) {
            throw invalid_argument("current day cannot be negative");
        }
        currentDay_ = day;
    }

    // ---- ⭐ Main API: CHECKOUT ---------------------------------------------
    Order checkout(const string &userId,
                   const string &addressId,
                   PaymentMethod paymentMethod,
                   const string &paymentInstrument,
                   const string &couponCode = "",
                   const string &clientRequestId = "") {
        // Cross-cutting validation (ye facade ka kaam hai, CheckoutService ka nahi):
        const User &user = getUserOrThrow(userId);
        getAddressOrThrow(addressId, userId); // address kisi AUR user ka to nahi?

        // ⭐ Early idempotency return — taaki duplicate request pe email DOBARA na jaye.
        if (!clientRequestId.empty()) {
            const Order *existing = orderService_.findByClientRequestId(clientRequestId);
            if (existing != nullptr) {
                return *existing;
            }
        }

        // Asli kaam CheckoutService karta hai (orchestration + rollback).
        Order order = checkoutService_.checkout(userId, addressId, paymentMethod, paymentInstrument,
                                                couponCode, clientRequestId, currentDay_,
                                                orderCounter_);
        // 📌 Notification checkout ke BAAD — email fail ho to order cancel na ho.
        notificationService_.sendOrderConfirmed(user.getEmail(), order);
        return order;
    }

    // ---- Display helpers (demo ke liye) ------------------------------------
    void printCart(const string &userId) const {
        if (!cartService_.hasCart(userId)) {
            cout << "\n=== Cart (" << userId << ") ===\n(empty)\n";
            return;
        }
        const Cart &cart = cartService_.getCartOrThrow(userId);
        cout << "\n=== Cart (" << userId << ") ===\n";
        for (const auto &entry : cart.getItems()) {
            const CartItem &item = entry.second;
            cout << item.getProduct().getProductId() << " | " << item.getProduct().getName()
                 << " | Qty=" << item.getQuantity() << " | Line=Rs " << item.lineTotal() << "\n";
        }
        cout << "Subtotal: Rs " << cart.subtotal() << "\n";
    }

    void printOrder(const string &orderId) const {
        const Order &order = orderService_.getOrderOrThrow(orderId);
        cout << "\n=== Order " << order.getOrderId() << " ===\n";
        // ⭐ Do status alag print hote hain — COD me CONFIRMED + PENDING dikhega.
        cout << "Status: " << orderStatusToString(order.getStatus()) << " | Payment: "
             << paymentStatusToString(order.getPaymentStatus()) << " | Method: "
             << paymentMethodToString(order.getPaymentMethod()) << "\n";
        for (const OrderLine &line : order.getLines()) {
            cout << line.getProductId() << " | " << line.getProductName()
                 << " | Qty=" << line.getQuantity() << " | Rs " << line.lineTotal() << "\n";
        }
        // Poora breakup (Order me stored hai, recompute nahi hota).
        cout << "Subtotal: Rs " << order.getSubtotal() << " | Discount: Rs " << order.getDiscount()
             << " | Shipping: Rs " << order.getShippingFee() << " | Payable: Rs "
             << order.getPayableAmount() << "\n";
        if (!order.getCouponCode().empty()) {
            cout << "Coupon: " << order.getCouponCode() << "\n";
        }
        if (!order.getPaymentTransactionId().empty()) {
            cout << "Txn: " << order.getPaymentTransactionId() << "\n";
        }
    }

    int getAvailableStock(const string &productId) const {
        return inventory_.getAvailableStock(productId);
    }

private:
    // Do demo coupons: percentage aur flat — same Coupon class, alag Strategy.
    void seedDefaultCoupons() {
        couponService_.registerCoupon(
            Coupon("SAVE10", 500.0, make_unique<PercentageDiscountStrategy>(10.0), 365));
        couponService_.registerCoupon(
            Coupon("FLAT100", 1000.0, make_unique<FlatDiscountStrategy>(100.0), 365));
    }

    void validateUser(const string &userId) const {
        if (!users_.count(userId)) {
            throw runtime_error("User not found: " + userId);
        }
    }

    const User &getUserOrThrow(const string &userId) const {
        validateUser(userId);
        return users_.at(userId);
    }

    // ⭐ Sirf address ka hona kaafi nahi — wo ISI user ka hona chahiye.
    // Warna koi bhi kisi aur ke address pe order bhej sakta (IDOR-type flaw).
    const Address &getAddressOrThrow(const string &addressId, const string &userId) const {
        auto it = addresses_.find(addressId);
        if (it == addresses_.end()) {
            throw runtime_error("Address not found: " + addressId);
        }
        if (it->second.getUserId() != userId) {
            throw runtime_error("Address does not belong to user");
        }
        return it->second;
    }

    // ⭐⭐ ORDER MATTERS! (upar wala note padho) — pehle wo services jinke
    // references chahiye, phir unhe use karne wali services.
    CatalogService catalog_;
    InventoryService inventory_;
    CouponService couponService_;
    PricingService pricingService_;
    PaymentService paymentService_;
    OrderService orderService_;
    NotificationService notificationService_;
    CartService cartService_;         // <- catalog_ ka reference leta hai
    CheckoutService checkoutService_; // <- upar wali 6 services ke reference leta hai

    unordered_map<string, User> users_;
    unordered_map<string, Address> addresses_;
    int currentDay_{1};    // simplified calendar (coupon expiry ke liye)
    int orderCounter_{0};  // ORD_1, ORD_2, ... (checkout by-reference badhata hai)
};

}  // namespace ecommerce_checkout_lld

#endif

#ifndef ECOMMERCE_CART_CHECKOUT_LLD_CORE_ECOMMERCECHECKOUTSYSTEM_H
#define ECOMMERCE_CART_CHECKOUT_LLD_CORE_ECOMMERCECHECKOUTSYSTEM_H

#include <bits/stdc++.h>

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
    EcommerceCheckoutSystem()
        : cartService_(catalog_),
          checkoutService_(cartService_, inventory_, couponService_, pricingService_,
                           paymentService_, orderService_) {
        seedDefaultCoupons();
    }

    void registerUser(const User &user) { users_.insert_or_assign(user.getUserId(), user); }

    void addAddress(const Address &address) {
        if (!users_.count(address.getUserId())) {
            throw runtime_error("User not found: " + address.getUserId());
        }
        addresses_.insert_or_assign(address.getAddressId(), address);
    }

    void addProduct(const Product &product, int stockQuantity) {
        catalog_.addProduct(product);
        inventory_.setStock(product.getProductId(), stockQuantity);
    }

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

    void registerCoupon(Coupon coupon) { couponService_.registerCoupon(std::move(coupon)); }

    void setCurrentDay(int day) {
        if (day < 0) {
            throw invalid_argument("current day cannot be negative");
        }
        currentDay_ = day;
    }

    Order checkout(const string &userId,
                   const string &addressId,
                   PaymentMethod paymentMethod,
                   const string &paymentInstrument,
                   const string &couponCode = "",
                   const string &clientRequestId = "") {
        const User &user = getUserOrThrow(userId);
        getAddressOrThrow(addressId, userId);

        if (!clientRequestId.empty()) {
            const Order *existing = orderService_.findByClientRequestId(clientRequestId);
            if (existing != nullptr) {
                return *existing;
            }
        }

        Order order = checkoutService_.checkout(userId, addressId, paymentMethod, paymentInstrument,
                                                couponCode, clientRequestId, currentDay_,
                                                orderCounter_);
        notificationService_.sendOrderConfirmed(user.getEmail(), order);
        return order;
    }

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
        cout << "Status: " << orderStatusToString(order.getStatus()) << " | Payment: "
             << paymentStatusToString(order.getPaymentStatus()) << " | Method: "
             << paymentMethodToString(order.getPaymentMethod()) << "\n";
        for (const OrderLine &line : order.getLines()) {
            cout << line.getProductId() << " | " << line.getProductName()
                 << " | Qty=" << line.getQuantity() << " | Rs " << line.lineTotal() << "\n";
        }
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

    CatalogService catalog_;
    InventoryService inventory_;
    CouponService couponService_;
    PricingService pricingService_;
    PaymentService paymentService_;
    OrderService orderService_;
    NotificationService notificationService_;
    CartService cartService_;
    CheckoutService checkoutService_;

    unordered_map<string, User> users_;
    unordered_map<string, Address> addresses_;
    int currentDay_{1};
    int orderCounter_{0};
};

}  // namespace ecommerce_checkout_lld

#endif

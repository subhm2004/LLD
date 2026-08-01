// ============================================================================
//  models/Order.h  —  Ek placed order (poora invoice record)
// ----------------------------------------------------------------------------
//  Order sab kuch jodta hai: kaunsa user, kaunsa address, kya-kya kharida
//  (lines), poora price breakup, payment method/status, aur order lifecycle status.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ PRICE BREAKUP puri tarah STORE hota hai (recompute nahi hota)         │
//  │                                                                          │
//  │     subtotal  -> saare items ka jod                                       │
//  │     discount  -> coupon se kitna kam hua                                  │
//  │     shipping  -> delivery charge (999+ pe free)                           │
//  │     payable   -> jo actually charge hua                                   │
//  │                                                                          │
//  │  Ye chaaron ALAG store hote hain (sirf final payable nahi) — kyunki       │
//  │  invoice pe poora breakup dikhana padta hai, aur refund/audit me har      │
//  │  component alag chahiye hota. Aur order ke baad rules badle (shipping     │
//  │  threshold badla) to bhi PURANI order ka bill waisa hi rahega.           │
//  │  (OrderLine.h ka "snapshot" wala note bhi padho — wahi soch hai.)        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `clientRequestId_` — IDEMPOTENCY ka dil                              │
//  │                                                                          │
//  │  User ka internet atka, usne "Place Order" DO baar daba diya. Dono        │
//  │  requests me client SAME `clientRequestId` bhejta hai. System pehle       │
//  │  dekh leta hai "is id se order pehle se hai kya?" — hai to WAHI purana    │
//  │  order lauta deta (naya nahi banata, paisa dobara nahi katta).            │
//  │  Bina iske: double order + double charge. 😱                              │
//  │  (CheckoutService/facade me `findByClientRequestId` check dekho.)        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 DO status fields (OrderStatus + PaymentStatus) — COD me order CONFIRMED
//     par payment PENDING (PaymentStatus.h me poora explanation).
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_ORDER_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_ORDER_H

#include <string>
#include <utility>
#include <vector>

#include "../enums/OrderStatus.h"
#include "../enums/PaymentMethod.h"
#include "../enums/PaymentStatus.h"
#include "OrderLine.h"

namespace ecommerce_checkout_lld {

using namespace std;

class Order {
public:
    Order(string orderId, string userId, string addressId, vector<OrderLine> lines,
          double subtotal, double discount, double shippingFee, double payableAmount,
          PaymentMethod paymentMethod, string couponCode = "")
        : orderId_(std::move(orderId)),
          userId_(std::move(userId)),
          addressId_(std::move(addressId)),
          lines_(std::move(lines)),      // snapshot lines (OrderLine.h note)
          subtotal_(subtotal),
          discount_(discount),
          shippingFee_(shippingFee),
          payableAmount_(payableAmount), // ⭐ locked at order time
          paymentMethod_(paymentMethod),
          couponCode_(std::move(couponCode)) {}

    // ---- Immutable order data (getters only) -------------------------------
    const string &getOrderId() const { return orderId_; }
    const string &getUserId() const { return userId_; }
    const string &getAddressId() const { return addressId_; }
    const vector<OrderLine> &getLines() const { return lines_; }
    double getSubtotal() const { return subtotal_; }
    double getDiscount() const { return discount_; }
    double getShippingFee() const { return shippingFee_; }
    double getPayableAmount() const { return payableAmount_; }
    PaymentMethod getPaymentMethod() const { return paymentMethod_; }
    const string &getCouponCode() const { return couponCode_; }

    // ---- Mutable lifecycle state (checkout ke dauran badalti hai) ----------
    OrderStatus getStatus() const { return status_; }
    PaymentStatus getPaymentStatus() const { return paymentStatus_; }
    const string &getPaymentTransactionId() const { return paymentTransactionId_; }
    const string &getClientRequestId() const { return clientRequestId_; } // ⭐ idempotency key

    void setStatus(OrderStatus status) { status_ = status; }
    void setPaymentStatus(PaymentStatus status) { paymentStatus_ = status; }
    void setPaymentTransactionId(const string &txnId) { paymentTransactionId_ = txnId; }
    void setClientRequestId(const string &clientRequestId) { clientRequestId_ = clientRequestId; }

private:
    string orderId_;
    string userId_;
    string addressId_;
    vector<OrderLine> lines_;      // item snapshots
    double subtotal_{0.0};         // ⭐ poora breakup store (upar note)
    double discount_{0.0};
    double shippingFee_{0.0};
    double payableAmount_{0.0};
    PaymentMethod paymentMethod_{PaymentMethod::UPI};
    string couponCode_;
    OrderStatus status_{OrderStatus::CREATED};        // lifecycle
    PaymentStatus paymentStatus_{PaymentStatus::PENDING}; // paise ki haalat (alag!)
    string paymentTransactionId_;                      // gateway reference
    string clientRequestId_;                           // ⭐ duplicate-order guard
};

}  // namespace ecommerce_checkout_lld

#endif

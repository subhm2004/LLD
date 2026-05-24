#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_ORDER_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_ORDER_H

#include <bits/stdc++.h>

#include "../enums/OrderStatus.h"
#include "../enums/PaymentMethod.h"
#include "../enums/PaymentStatus.h"
#include "OrderLine.h"

namespace ecommerce_checkout_lld {

using namespace std;

class Order {
public:
    Order(string orderId,
          string userId,
          string addressId,
          vector<OrderLine> lines,
          double subtotal,
          double discount,
          double shippingFee,
          double payableAmount,
          PaymentMethod paymentMethod,
          string couponCode = "")
        : orderId_(std::move(orderId)),
          userId_(std::move(userId)),
          addressId_(std::move(addressId)),
          lines_(std::move(lines)),
          subtotal_(subtotal),
          discount_(discount),
          shippingFee_(shippingFee),
          payableAmount_(payableAmount),
          paymentMethod_(paymentMethod),
          couponCode_(std::move(couponCode)) {}

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

    OrderStatus getStatus() const { return status_; }
    PaymentStatus getPaymentStatus() const { return paymentStatus_; }
    const string &getPaymentTransactionId() const { return paymentTransactionId_; }
    const string &getClientRequestId() const { return clientRequestId_; }

    void setStatus(OrderStatus status) { status_ = status; }
    void setPaymentStatus(PaymentStatus status) { paymentStatus_ = status; }
    void setPaymentTransactionId(const string &txnId) { paymentTransactionId_ = txnId; }
    void setClientRequestId(const string &clientRequestId) { clientRequestId_ = clientRequestId; }

private:
    string orderId_;
    string userId_;
    string addressId_;
    vector<OrderLine> lines_;
    double subtotal_{0.0};
    double discount_{0.0};
    double shippingFee_{0.0};
    double payableAmount_{0.0};
    PaymentMethod paymentMethod_{PaymentMethod::UPI};
    string couponCode_;
    OrderStatus status_{OrderStatus::CREATED};
    PaymentStatus paymentStatus_{PaymentStatus::PENDING};
    string paymentTransactionId_;
    string clientRequestId_;
};

}  // namespace ecommerce_checkout_lld

#endif

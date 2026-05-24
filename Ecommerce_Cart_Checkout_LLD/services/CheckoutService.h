#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_CHECKOUTSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_CHECKOUTSERVICE_H

#include <bits/stdc++.h>

#include "../enums/OrderStatus.h"
#include "../enums/PaymentMethod.h"
#include "../enums/PaymentStatus.h"
#include "../models/Order.h"
#include "../models/OrderLine.h"
#include "CartService.h"
#include "CouponService.h"
#include "InventoryService.h"
#include "OrderService.h"
#include "PaymentService.h"
#include "PricingService.h"

namespace ecommerce_checkout_lld {

using namespace std;

class CheckoutService {
public:
    CheckoutService(CartService &cartService,
                    InventoryService &inventoryService,
                    CouponService &couponService,
                    PricingService &pricingService,
                    PaymentService &paymentService,
                    OrderService &orderService)
        : cartService_(cartService),
          inventoryService_(inventoryService),
          couponService_(couponService),
          pricingService_(pricingService),
          paymentService_(paymentService),
          orderService_(orderService) {}

    Order checkout(const string &userId,
                   const string &addressId,
                   PaymentMethod paymentMethod,
                   const string &paymentInstrument,
                   const string &couponCode,
                   const string &clientRequestId,
                   int currentDay,
                   int &orderCounter) {
        if (!clientRequestId.empty()) {
            const Order *existing = orderService_.findByClientRequestId(clientRequestId);
            if (existing != nullptr) {
                return *existing;
            }
        }

        const Cart &cart = cartService_.getCartOrThrow(userId);
        const unordered_map<string, int> requestedItems = cartService_.toQuantityMap(cart);

        const string reservationId = inventoryService_.reserveItems(userId, requestedItems);

        try {
            const double subtotal = cart.subtotal();
            const double discount =
                couponService_.applyCouponOrThrow(couponCode, subtotal, currentDay);
            const PriceBreakup breakup = pricingService_.calculate(subtotal, discount);

            vector<OrderLine> lines;
            for (const auto &entry : cart.getItems()) {
                const CartItem &item = entry.second;
                lines.emplace_back(item.getProduct().getProductId(), item.getProduct().getName(),
                                   item.getProduct().getUnitPrice(), item.getQuantity());
            }

            const string orderId = "ORD_" + to_string(++orderCounter);
            Order order(orderId, userId, addressId, std::move(lines), breakup.subtotal,
                        breakup.discount, breakup.shippingFee, breakup.payableAmount, paymentMethod,
                        couponCode);
            order.setClientRequestId(clientRequestId);
            order.setStatus(OrderStatus::PAYMENT_PENDING);

            const PaymentResult paymentResult =
                paymentService_.processPayment(paymentMethod, breakup.payableAmount,
                                               paymentInstrument);

            if (!paymentResult.isSuccess()) {
                inventoryService_.releaseReservation(reservationId);
                order.setStatus(OrderStatus::FAILED);
                order.setPaymentStatus(PaymentStatus::FAILED);
                orderService_.saveOrder(std::move(order));
                throw runtime_error("Payment failed: " + paymentResult.getMessage());
            }

            inventoryService_.commitReservation(reservationId);
            order.setPaymentTransactionId(paymentResult.getTransactionId());

            if (paymentMethod == PaymentMethod::COD) {
                order.setPaymentStatus(PaymentStatus::PENDING);
                order.setStatus(OrderStatus::CONFIRMED);
            } else {
                order.setPaymentStatus(PaymentStatus::SUCCESS);
                order.setStatus(OrderStatus::CONFIRMED);
            }

            cartService_.clearCart(userId);
            orderService_.saveOrder(std::move(order));
            Order &savedOrder = orderService_.getOrderOrThrow(orderId);
            return savedOrder;
        } catch (...) {
            inventoryService_.releaseReservation(reservationId);
            throw;
        }
    }

private:
    CartService &cartService_;
    InventoryService &inventoryService_;
    CouponService &couponService_;
    PricingService &pricingService_;
    PaymentService &paymentService_;
    OrderService &orderService_;
};

}  // namespace ecommerce_checkout_lld

#endif

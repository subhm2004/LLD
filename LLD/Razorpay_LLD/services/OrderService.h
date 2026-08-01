#ifndef RAZORPAY_LLD_SERVICES_ORDER_SERVICE_H
#define RAZORPAY_LLD_SERVICES_ORDER_SERVICE_H

#include <stdexcept>
#include <unordered_map>

#include "../clients/RazorpayApiClient.h"
#include "../enums/OrderStatus.h"
#include "../factories/EntityIdFactory.h"
#include "../models/RazorpayOrder.h"
#include "IdempotencyService.h"

namespace razorpay_lld {

class OrderService {
public:
    explicit OrderService(RazorpayApiClient* client, IdempotencyService* idempotency)
        : client_(client), idempotency_(idempotency) {}

    RazorpayOrder createOrder(int amountPaise, const std::string& currency,
                              const std::string& receipt, const std::string& idempotencyKey = "") {
        if (amountPaise <= 0) {
            throw std::invalid_argument("amount must be positive paise");
        }
        if (currency.empty()) {
            throw std::invalid_argument("currency is required");
        }
        if (receipt.empty()) {
            throw std::invalid_argument("receipt reference is required");
        }
        if (!idempotencyKey.empty() && idempotency_->exists(idempotencyKey)) {
            return orders_.at(idempotency_->resolveDuplicate(idempotencyKey));
        }

        RazorpayOrder order;
        order.orderId = EntityIdFactory::nextOrderId();
        order.amountPaise = amountPaise;
        order.currency = currency;
        order.receipt = receipt;
        order.status = OrderStatus::CREATED;
        order.idempotencyKey = idempotencyKey;

        client_->postOrder(order.orderId, order.amountPaise, order.currency);
        orders_[order.orderId] = order;
        if (!idempotencyKey.empty()) {
            idempotency_->store(idempotencyKey, order.orderId);
        }
        return order;
    }

    RazorpayOrder& getOrder(const std::string& orderId) {
        auto it = orders_.find(orderId);
        if (it == orders_.end()) {
            throw std::runtime_error("order not found: " + orderId);
        }
        return it->second;
    }

    const RazorpayOrder& getOrder(const std::string& orderId) const {
        return const_cast<OrderService*>(this)->getOrder(orderId);
    }

    void markPaid(const std::string& orderId) { getOrder(orderId).status = OrderStatus::PAID; }

    void markAttempted(const std::string& orderId) {
        auto& order = getOrder(orderId);
        if (order.status == OrderStatus::CREATED) {
            order.status = OrderStatus::ATTEMPTED;
        }
    }

private:
    RazorpayApiClient* client_;
    IdempotencyService* idempotency_;
    std::unordered_map<std::string, RazorpayOrder> orders_;
};

}  // namespace razorpay_lld

#endif

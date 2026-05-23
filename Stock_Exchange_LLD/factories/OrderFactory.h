#ifndef STOCK_EXCHANGE_LLD_FACTORIES_ORDERFACTORY_H
#define STOCK_EXCHANGE_LLD_FACTORIES_ORDERFACTORY_H

#include <stdexcept>
#include <string>

#include "../enums/OrderSide.h"
#include "../enums/OrderStatus.h"
#include "../enums/OrderType.h"
#include "../models/Order.h"

namespace stock_exchange_lld {

class OrderFactory {
public:
    static Order create(int& orderCounter, const std::string& userId, const std::string& symbol,
                        OrderSide side, OrderType type, double price, int quantity,
                        long long createdAtEpochMs) {
        if (userId.empty() || symbol.empty()) {
            throw std::invalid_argument("userId and symbol required");
        }
        if (quantity <= 0) {
            throw std::invalid_argument("quantity must be positive");
        }
        if (type == OrderType::LIMIT && price <= 0.0) {
            throw std::invalid_argument("limit order requires price > 0");
        }
        if (type == OrderType::MARKET) {
            price = 0.0;
        }

        Order order;
        order.orderId = "ORD_" + std::to_string(++orderCounter);
        order.userId = userId;
        order.symbol = symbol;
        order.side = side;
        order.type = type;
        order.price = price;
        order.quantity = quantity;
        order.filledQuantity = 0;
        order.status = OrderStatus::OPEN;
        order.createdAtEpochMs = createdAtEpochMs;
        return order;
    }
};

}  // namespace stock_exchange_lld

#endif  // STOCK_EXCHANGE_LLD_FACTORIES_ORDERFACTORY_H

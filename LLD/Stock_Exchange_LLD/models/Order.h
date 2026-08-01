#ifndef STOCK_EXCHANGE_LLD_MODELS_ORDER_H
#define STOCK_EXCHANGE_LLD_MODELS_ORDER_H

#include <string>

#include "../enums/OrderSide.h"
#include "../enums/OrderStatus.h"
#include "../enums/OrderType.h"

namespace stock_exchange_lld {

struct Order {
    std::string orderId;
    std::string userId;
    std::string symbol;
    OrderSide side{OrderSide::BUY};
    OrderType type{OrderType::LIMIT};
    double price{0.0};
    int quantity{0};
    int filledQuantity{0};
    OrderStatus status{OrderStatus::OPEN};
    long long createdAtEpochMs{0};

    int remainingQuantity() const { return quantity - filledQuantity; }

    bool isTerminal() const {
        return status == OrderStatus::FILLED || status == OrderStatus::CANCELLED;
    }
};

}  // namespace stock_exchange_lld

#endif  // STOCK_EXCHANGE_LLD_MODELS_ORDER_H

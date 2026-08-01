#ifndef STOCK_EXCHANGE_LLD_SERVICES_ORDERREGISTRYSERVICE_H
#define STOCK_EXCHANGE_LLD_SERVICES_ORDERREGISTRYSERVICE_H

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "../models/Order.h"

namespace stock_exchange_lld {

class OrderRegistryService {
public:
    void save(const std::shared_ptr<Order>& order) { orders_[order->orderId] = order; }

    std::shared_ptr<Order> get(const std::string& orderId) const {
        auto it = orders_.find(orderId);
        if (it == orders_.end()) {
            throw std::runtime_error("order not found: " + orderId);
        }
        return it->second;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Order>> orders_;
};

}  // namespace stock_exchange_lld

#endif  // STOCK_EXCHANGE_LLD_SERVICES_ORDERREGISTRYSERVICE_H

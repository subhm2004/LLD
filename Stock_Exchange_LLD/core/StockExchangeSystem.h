#ifndef STOCK_EXCHANGE_LLD_CORE_STOCKEXCHANGESYSTEM_H
#define STOCK_EXCHANGE_LLD_CORE_STOCKEXCHANGESYSTEM_H

#include <chrono>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/OrderSide.h"
#include "../enums/OrderStatus.h"
#include "../enums/OrderType.h"
#include "../factories/OrderFactory.h"
#include "../models/Order.h"
#include "../models/OrderBookLevel.h"
#include "../models/Symbol.h"
#include "../models/Trade.h"
#include "../models/User.h"
#include "../services/MatchingEngineService.h"
#include "../services/OrderRegistryService.h"
#include "../services/TradeLedgerService.h"

namespace stock_exchange_lld {

struct PlaceOrderResult {
    std::shared_ptr<Order> order;
    std::vector<Trade> trades;
};

class StockExchangeSystem {
public:
    std::string registerUser(const std::string& name) {
        if (name.empty()) {
            throw std::invalid_argument("name required");
        }
        const std::string userId = "USR_" + std::to_string(++userCounter_);
        users_[userId] = User{userId, name};
        return userId;
    }

    void addSymbol(const std::string& ticker, const std::string& name) {
        if (ticker.empty()) {
            throw std::invalid_argument("ticker required");
        }
        if (symbols_.count(ticker) != 0) {
            throw std::runtime_error("symbol already listed: " + ticker);
        }
        symbols_[ticker] = Symbol{ticker, name};
        matchingEngine_.bookFor(ticker);
        matchingEngine_.bookMutexFor(ticker);
    }

    PlaceOrderResult placeOrder(const std::string& userId, const std::string& symbol, OrderSide side,
                                OrderType type, double price, int quantity) {
        validateUser(userId);
        validateSymbol(symbol);

        Order order = OrderFactory::create(orderCounter_, userId, symbol, side, type, price, quantity,
                                           nowEpochMs());
        auto orderPtr = std::make_shared<Order>(order);
        orderRegistry_.save(orderPtr);

        std::vector<Trade> trades =
            matchingEngine_.matchOrder(orderPtr, tradeLedger_, tradeCounter_, nowEpochMs());

        return PlaceOrderResult{orderPtr, std::move(trades)};
    }

    void cancelOrder(const std::string& orderId, const std::string& userId) {
        auto order = orderRegistry_.get(orderId);
        if (order->userId != userId) {
            throw std::runtime_error("not allowed to cancel order: " + orderId);
        }
        if (order->isTerminal()) {
            throw std::runtime_error("order already terminal: " + orderId);
        }

        std::lock_guard<std::mutex> lock(matchingEngine_.bookMutexFor(order->symbol));
        matchingEngine_.bookFor(order->symbol).removeRestingOrder(order);
        order->status = OrderStatus::CANCELLED;
        matchingEngine_.bookFor(order->symbol).pruneEmptyLevels();
    }

    std::shared_ptr<Order> getOrder(const std::string& orderId) const {
        return orderRegistry_.get(orderId);
    }

    std::vector<OrderBookLevel> getBidBook(const std::string& symbol, int depth = 5) const {
        validateSymbol(symbol);
        return const_cast<MatchingEngineService&>(matchingEngine_).bookFor(symbol).snapshotBids(depth);
    }

    std::vector<OrderBookLevel> getAskBook(const std::string& symbol, int depth = 5) const {
        validateSymbol(symbol);
        return const_cast<MatchingEngineService&>(matchingEngine_).bookFor(symbol).snapshotAsks(depth);
    }

    const std::vector<Trade>& getAllTrades() const { return tradeLedger_.listAll(); }

    std::vector<Trade> getTradesForSymbol(const std::string& symbol) const {
        return tradeLedger_.listBySymbol(symbol);
    }

private:
    std::unordered_map<std::string, User> users_;
    std::unordered_map<std::string, Symbol> symbols_;
    OrderRegistryService orderRegistry_;
    TradeLedgerService tradeLedger_;
    MatchingEngineService matchingEngine_;

    int userCounter_{0};
    int orderCounter_{0};
    int tradeCounter_{0};

    long long nowEpochMs() const {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }

    void validateUser(const std::string& userId) const {
        if (users_.find(userId) == users_.end()) {
            throw std::runtime_error("user not found: " + userId);
        }
    }

    void validateSymbol(const std::string& symbol) const {
        if (symbols_.find(symbol) == symbols_.end()) {
            throw std::runtime_error("symbol not listed: " + symbol);
        }
    }
};

}  // namespace stock_exchange_lld

#endif  // STOCK_EXCHANGE_LLD_CORE_STOCKEXCHANGESYSTEM_H

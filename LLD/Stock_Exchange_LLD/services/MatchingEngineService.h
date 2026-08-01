#ifndef STOCK_EXCHANGE_LLD_SERVICES_MATCHINGENGINESERVICE_H
#define STOCK_EXCHANGE_LLD_SERVICES_MATCHINGENGINESERVICE_H

#include <algorithm>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/OrderSide.h"
#include "../enums/OrderStatus.h"
#include "../enums/OrderType.h"
#include "../models/Order.h"
#include "../models/Trade.h"
#include "OrderBookService.h"
#include "OrderRegistryService.h"
#include "TradeLedgerService.h"

namespace stock_exchange_lld {

class MatchingEngineService {
public:
    std::vector<Trade> matchOrder(const std::shared_ptr<Order>& incoming, TradeLedgerService& ledger,
                                  int& tradeCounter, long long nowMs) {
        OrderBookService& book = bookFor(incoming->symbol);
        std::lock_guard<std::mutex> lock(bookMutexFor(incoming->symbol));

        std::vector<Trade> executed;
        while (incoming->remainingQuantity() > 0) {
            std::shared_ptr<Order> counter = findCounterparty(incoming, book);
            if (!counter) {
                break;
            }
            if (!isPriceCompatible(incoming, counter)) {
                break;
            }

            const int fillQty = std::min(incoming->remainingQuantity(), counter->remainingQuantity());
            const double tradePrice = counter->price;

            incoming->filledQuantity += fillQty;
            counter->filledQuantity += fillQty;
            updateStatus(incoming);
            updateStatus(counter);

            Trade trade;
            trade.tradeId = "TRD_" + std::to_string(++tradeCounter);
            trade.symbol = incoming->symbol;
            trade.price = tradePrice;
            trade.quantity = fillQty;
            trade.executedAtEpochMs = nowMs;
            if (incoming->side == OrderSide::BUY) {
                trade.buyOrderId = incoming->orderId;
                trade.sellOrderId = counter->orderId;
            } else {
                trade.buyOrderId = counter->orderId;
                trade.sellOrderId = incoming->orderId;
            }
            ledger.record(trade);
            executed.push_back(trade);

            if (counter->isTerminal()) {
                book.removeRestingOrder(counter);
            }
        }

        book.pruneEmptyLevels();

        if (incoming->remainingQuantity() > 0 && incoming->type == OrderType::LIMIT &&
            incoming->status != OrderStatus::FILLED) {
            book.addRestingOrder(incoming);
        }

        return executed;
    }

    OrderBookService& bookFor(const std::string& symbol) { return books_[symbol]; }

    std::mutex& bookMutexFor(const std::string& symbol) { return bookMutexes_[symbol]; }

private:
    std::unordered_map<std::string, OrderBookService> books_;
    std::unordered_map<std::string, std::mutex> bookMutexes_;

    static std::shared_ptr<Order> findCounterparty(const std::shared_ptr<Order>& incoming,
                                                   OrderBookService& book) {
        if (incoming->side == OrderSide::BUY) {
            return book.bestAsk();
        }
        return book.bestBid();
    }

    static bool isPriceCompatible(const std::shared_ptr<Order>& incoming,
                                  const std::shared_ptr<Order>& resting) {
        if (incoming->type == OrderType::MARKET) {
            return true;
        }
        if (incoming->side == OrderSide::BUY) {
            return incoming->price >= resting->price;
        }
        return incoming->price <= resting->price;
    }

    static void updateStatus(const std::shared_ptr<Order>& order) {
        if (order->remainingQuantity() == 0) {
            order->status = OrderStatus::FILLED;
        } else {
            order->status = OrderStatus::PARTIAL;
        }
    }
};

}  // namespace stock_exchange_lld

#endif  // STOCK_EXCHANGE_LLD_SERVICES_MATCHINGENGINESERVICE_H

#ifndef STOCK_EXCHANGE_LLD_SERVICES_ORDERBOOKSERVICE_H
#define STOCK_EXCHANGE_LLD_SERVICES_ORDERBOOKSERVICE_H

#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "../enums/OrderSide.h"
#include "../enums/OrderStatus.h"
#include "../models/Order.h"
#include "../models/OrderBookLevel.h"

namespace stock_exchange_lld {

class OrderBookService {
public:
    void addRestingOrder(const std::shared_ptr<Order>& order) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (order->side == OrderSide::BUY) {
            bids_[order->price].push_back(order);
        } else {
            asks_[order->price].push_back(order);
        }
    }

    bool removeRestingOrder(const std::shared_ptr<Order>& order) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (order->side == OrderSide::BUY) {
            return removeFromSide(bids_, order);
        }
        return removeFromSide(asks_, order);
    }

    std::shared_ptr<Order> bestAsk() const {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto it = asks_.begin(); it != asks_.end(); ++it) {
            for (const auto& order : it->second) {
                if (order->status == OrderStatus::OPEN || order->status == OrderStatus::PARTIAL) {
                    return order;
                }
            }
        }
        return nullptr;
    }

    std::shared_ptr<Order> bestBid() const {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto it = bids_.begin(); it != bids_.end(); ++it) {
            for (const auto& order : it->second) {
                if (order->status == OrderStatus::OPEN || order->status == OrderStatus::PARTIAL) {
                    return order;
                }
            }
        }
        return nullptr;
    }

    double bestAskPrice() const {
        auto order = bestAsk();
        return order ? order->price : -1.0;
    }

    double bestBidPrice() const {
        auto order = bestBid();
        return order ? order->price : -1.0;
    }

    void pruneEmptyLevels() {
        std::lock_guard<std::mutex> lock(mutex_);
        pruneSide(bids_);
        pruneSide(asks_);
    }

    std::vector<OrderBookLevel> snapshotBids(int depth = 5) const {
        return snapshotSide(bids_, depth);
    }

    std::vector<OrderBookLevel> snapshotAsks(int depth = 5) const {
        return snapshotSide(asks_, depth);
    }

private:
    using BidBook = std::map<double, std::deque<std::shared_ptr<Order>>, std::greater<double>>;
    using AskBook = std::map<double, std::deque<std::shared_ptr<Order>>>;

    mutable std::mutex mutex_;
    BidBook bids_;
    AskBook asks_;

    template <typename Book>
    static bool removeFromSide(Book& book, const std::shared_ptr<Order>& order) {
        auto levelIt = book.find(order->price);
        if (levelIt == book.end()) {
            return false;
        }
        auto& queue = levelIt->second;
        for (auto it = queue.begin(); it != queue.end(); ++it) {
            if ((*it)->orderId == order->orderId) {
                queue.erase(it);
                if (queue.empty()) {
                    book.erase(levelIt);
                }
                return true;
            }
        }
        return false;
    }

    template <typename Book>
    static void pruneSide(Book& book) {
        for (auto it = book.begin(); it != book.end();) {
            bool hasActive = false;
            for (const auto& order : it->second) {
                if (order->status == OrderStatus::OPEN || order->status == OrderStatus::PARTIAL) {
                    hasActive = true;
                    break;
                }
            }
            if (!hasActive) {
                it = book.erase(it);
            } else {
                ++it;
            }
        }
    }

    template <typename Book>
    static std::vector<OrderBookLevel> snapshotSide(const Book& book, int depth) {
        std::vector<OrderBookLevel> levels;
        int count = 0;
        for (const auto& entry : book) {
            OrderBookLevel level{entry.first, 0, 0};
            for (const auto& order : entry.second) {
                if (order->status == OrderStatus::OPEN || order->status == OrderStatus::PARTIAL) {
                    level.totalQuantity += order->remainingQuantity();
                    ++level.orderCount;
                }
            }
            if (level.totalQuantity > 0) {
                levels.push_back(level);
                if (++count >= depth) {
                    break;
                }
            }
        }
        return levels;
    }
};

}  // namespace stock_exchange_lld

#endif  // STOCK_EXCHANGE_LLD_SERVICES_ORDERBOOKSERVICE_H

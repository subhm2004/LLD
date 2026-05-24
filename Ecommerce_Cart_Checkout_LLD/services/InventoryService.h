#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_INVENTORYSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_INVENTORYSERVICE_H

#include <bits/stdc++.h>

namespace ecommerce_checkout_lld {

using namespace std;

class InventoryService {
public:
    void setStock(const string &productId, int quantity) {
        if (quantity < 0) {
            throw invalid_argument("Stock cannot be negative");
        }
        availableStock_[productId] = quantity;
    }

    int getAvailableStock(const string &productId) const {
        auto it = availableStock_.find(productId);
        return it == availableStock_.end() ? 0 : it->second;
    }

    void validateAvailability(const unordered_map<string, int> &requestedItems) const {
        for (const auto &entry : requestedItems) {
            const int available = getAvailableStock(entry.first);
            if (entry.second > available) {
                throw runtime_error("Insufficient stock for product: " + entry.first +
                                    " (requested=" + to_string(entry.second) +
                                    ", available=" + to_string(available) + ")");
            }
        }
    }

    string reserveItems(const string &userId, const unordered_map<string, int> &requestedItems) {
        validateAvailability(requestedItems);
        const string reservationId = "RES_" + userId + "_" + to_string(++reservationCounter_);

        for (const auto &entry : requestedItems) {
            availableStock_[entry.first] -= entry.second;
            reservedStock_[reservationId][entry.first] = entry.second;
        }
        reservationOwner_[reservationId] = userId;
        return reservationId;
    }

    void commitReservation(const string &reservationId) {
        auto reservationIt = reservedStock_.find(reservationId);
        if (reservationIt == reservedStock_.end()) {
            throw runtime_error("Reservation not found: " + reservationId);
        }
        reservedStock_.erase(reservationIt);
        reservationOwner_.erase(reservationId);
    }

    void releaseReservation(const string &reservationId) {
        auto reservationIt = reservedStock_.find(reservationId);
        if (reservationIt == reservedStock_.end()) {
            return;
        }
        for (const auto &entry : reservationIt->second) {
            availableStock_[entry.first] += entry.second;
        }
        reservedStock_.erase(reservationIt);
        reservationOwner_.erase(reservationId);
    }

private:
    unordered_map<string, int> availableStock_;
    unordered_map<string, unordered_map<string, int>> reservedStock_;
    unordered_map<string, string> reservationOwner_;
    int reservationCounter_{0};
};

}  // namespace ecommerce_checkout_lld

#endif

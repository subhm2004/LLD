#ifndef BLINKIT_LLD_CORE_BLINKITSYSTEM_H
#define BLINKIT_LLD_CORE_BLINKITSYSTEM_H

#include <bits/stdc++.h>

#include "../inventory/Inventory.h"
#include "../models/UserCart.h"

using namespace std;

namespace blinkit_lld {

enum class OrderStatus {
    PLACED,
    CONFIRMED,
    PACKING,
    OUT_FOR_DELIVERY,
    DELIVERED,
    CANCELLED
};

inline string orderStatusToString(OrderStatus status) {
    if (status == OrderStatus::PLACED) return "PLACED";
    if (status == OrderStatus::CONFIRMED) return "CONFIRMED";
    if (status == OrderStatus::PACKING) return "PACKING";
    if (status == OrderStatus::OUT_FOR_DELIVERY) return "OUT_FOR_DELIVERY";
    if (status == OrderStatus::DELIVERED) return "DELIVERED";
    return "CANCELLED";
}

struct DeliveryFeeBreakdown {
    double baseFee;
    double distanceFee;
    double surgeMultiplier;
    double totalFee;
};

class DarkStore {
public:
    DarkStore(string name, double x, double y) : name_(std::move(name)), x_(x), y_(y), inventory_(new InventoryManager(new InMemoryInventoryStore())) {}
    ~DarkStore() { delete inventory_; }

    double distanceTo(double ux, double uy) const {
        return sqrt((x_ - ux) * (x_ - ux) + (y_ - uy) * (y_ - uy));
    }

    void addStock(int sku, int qty) { inventory_->addStock(sku, qty); }
    void removeStock(int sku, int qty) { inventory_->removeStock(sku, qty); }
    int checkStock(int sku) const { return inventory_->checkStock(sku); }
    vector<Product *> products() const { return inventory_->availableProducts(); }
    const string &getName() const { return name_; }

private:
    string name_;
    double x_;
    double y_;
    InventoryManager *inventory_;
};

class DarkStoreManager {
public:
    static DarkStoreManager *getInstance() {
        static DarkStoreManager instance;
        return &instance;
    }

    void registerDarkStore(DarkStore *store) { stores_.push_back(store); }

    vector<DarkStore *> nearbyStores(double ux, double uy, double maxDistance) const {
        vector<pair<double, DarkStore *>> candidates;
        for (DarkStore *store : stores_) {
            const double distance = store->distanceTo(ux, uy);
            if (distance <= maxDistance) {
                candidates.push_back({distance, store});
            }
        }
        sort(candidates.begin(), candidates.end(), [](const auto &a, const auto &b) { return a.first < b.first; });

        vector<DarkStore *> result;
        for (const auto &entry : candidates) {
            result.push_back(entry.second);
        }
        return result;
    }

    ~DarkStoreManager() {
        for (DarkStore *store : stores_) {
            delete store;
        }
    }

private:
    vector<DarkStore *> stores_;
};

class OrderManager {
public:
    static OrderManager *getInstance() {
        static OrderManager instance;
        return &instance;
    }

    int placeOrder(User *user) {
        auto stores = DarkStoreManager::getInstance()->nearbyStores(user->getX(), user->getY(), 5.0);
        if (stores.empty()) {
            cout << "No nearby dark store found.\n";
            return -1;
        }

        DarkStore *store = stores.front();
        const double distanceKm = store->distanceTo(user->getX(), user->getY());
        const DeliveryFeeBreakdown fee = calculateDeliveryFee(distanceKm);
        const int orderId = ++orderCounter_;
        orderStatus_[orderId] = OrderStatus::PLACED;

        cout << "Placing order #" << orderId << " from " << store->getName() << " for user " << user->getName() << "\n";
        double total = 0.0;

        for (const auto &entry : user->getCart()->getItems()) {
            Product *product = entry.first;
            const int qty = entry.second;
            if (store->checkStock(product->getSku()) < qty) {
                cout << "Out of stock: " << product->getName() << "\n";
                continue;
            }
            store->removeStock(product->getSku(), qty);
            total += product->getPrice() * qty;
            cout << "  " << product->getName() << " x " << qty << "\n";
        }

        cout << "Item Total: " << total << "\n";
        cout << "Delivery Fee [Base: " << fee.baseFee
             << ", DistanceFee: " << fee.distanceFee
             << ", Surge: x" << fee.surgeMultiplier
             << ", Total: " << fee.totalFee << "]\n";
        cout << "Final Payable Amount: " << (total + fee.totalFee) << "\n";
        return orderId;
    }

    void updateOrderStatus(int orderId, OrderStatus nextStatus) {
        auto it = orderStatus_.find(orderId);
        if (it == orderStatus_.end()) {
            throw runtime_error("Order not found");
        }
        if (!isValidTransition(it->second, nextStatus)) {
            throw runtime_error("Invalid order status transition from " + orderStatusToString(it->second) +
                                " to " + orderStatusToString(nextStatus));
        }
        it->second = nextStatus;
        cout << "Order #" << orderId << " -> " << orderStatusToString(nextStatus) << "\n";
    }

private:
    unordered_map<int, OrderStatus> orderStatus_;
    int orderCounter_ = 0;

    static DeliveryFeeBreakdown calculateDeliveryFee(double distanceKm) {
        const double baseFee = 20.0;
        const double perKm = 5.0;
        const double distanceFee = distanceKm * perKm;
        const double surgeMultiplier = (distanceKm > 3.0) ? 1.5 : 1.0;
        const double totalFee = (baseFee + distanceFee) * surgeMultiplier;
        return {baseFee, distanceFee, surgeMultiplier, totalFee};
    }

    static bool isValidTransition(OrderStatus current, OrderStatus next) {
        if (current == OrderStatus::CANCELLED || current == OrderStatus::DELIVERED) {
            return false;
        }
        if (next == OrderStatus::CANCELLED) {
            return true;
        }
        if (current == OrderStatus::PLACED && next == OrderStatus::CONFIRMED) return true;
        if (current == OrderStatus::CONFIRMED && next == OrderStatus::PACKING) return true;
        if (current == OrderStatus::PACKING && next == OrderStatus::OUT_FOR_DELIVERY) return true;
        if (current == OrderStatus::OUT_FOR_DELIVERY && next == OrderStatus::DELIVERED) return true;
        return false;
    }
};

} // namespace blinkit_lld

#endif // BLINKIT_LLD_CORE_BLINKITSYSTEM_H

// inventory/ReplenishStrategy.h — STRATEGY: stock kab/kaise refill ho
// (Threshold-based ya Weekly). Per dark-store swap ho sakti hai, naya strategy
// add karna easy. (Strategy pattern)
#ifndef BLINKIT_LLD_INVENTORY_REPLENISHSTRATEGY_H
#define BLINKIT_LLD_INVENTORY_REPLENISHSTRATEGY_H

#include <bits/stdc++.h>

#include "Inventory.h"

using namespace std;

namespace blinkit_lld {

// Strategy pattern: replenishment logic swappable per dark store.
class ReplenishStrategy {
public:
    virtual ~ReplenishStrategy() = default;
    virtual void replenish(InventoryManager *manager, const map<int, int> &itemsToReplenish) = 0;
    virtual string name() const = 0;
};

class ThresholdReplenishStrategy : public ReplenishStrategy {
public:
    explicit ThresholdReplenishStrategy(int threshold) : threshold_(threshold) {}

    void replenish(InventoryManager *manager, const map<int, int> &itemsToReplenish) override {
        cout << "[ThresholdReplenish] Checking threshold=" << threshold_ << "...\n";
        for (const auto &entry : itemsToReplenish) {
            const int sku = entry.first;
            const int qtyToAdd = entry.second;
            const int current = manager->checkStock(sku);
            if (current < threshold_) {
                manager->addStock(sku, qtyToAdd);
                cout << "  -> SKU " << sku << " was " << current << ", replenished by " << qtyToAdd << "\n";
            }
        }
    }

    string name() const override { return "Threshold"; }

private:
    int threshold_;
};

class WeeklyReplenishStrategy : public ReplenishStrategy {
public:
    void replenish(InventoryManager *manager, const map<int, int> &itemsToReplenish) override {
        (void)manager;
        cout << "[WeeklyReplenish] Weekly replenishment triggered for SKUs: ";
        for (const auto &entry : itemsToReplenish) {
            cout << entry.first << "(+" << entry.second << ") ";
        }
        cout << "\n";
    }

    string name() const override { return "Weekly"; }
};

} // namespace blinkit_lld

#endif // BLINKIT_LLD_INVENTORY_REPLENISHSTRATEGY_H

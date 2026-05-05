#ifndef BLINKIT_LLD_INVENTORY_INVENTORY_H
#define BLINKIT_LLD_INVENTORY_INVENTORY_H

#include <bits/stdc++.h>

#include "../models/Product.h"

using namespace std;

namespace blinkit_lld {

class InventoryStore {
public:
    virtual ~InventoryStore() = default;
    virtual void addProduct(Product *product, int qty) = 0;
    virtual void removeProduct(int sku, int qty) = 0;
    virtual int checkStock(int sku) const = 0;
    virtual vector<Product *> listAvailableProducts() const = 0;
};

class InMemoryInventoryStore : public InventoryStore {
public:
    ~InMemoryInventoryStore() override {
        for (auto &entry : products_) {
            delete entry.second;
        }
    }

    void addProduct(Product *product, int qty) override {
        const int sku = product->getSku();
        auto it = products_.find(sku);
        if (it == products_.end()) {
            products_[sku] = product;
        } else {
            delete product;
        }
        stock_[sku] += qty;
    }

    void removeProduct(int sku, int qty) override {
        auto it = stock_.find(sku);
        if (it == stock_.end()) {
            return;
        }
        it->second = max(0, it->second - qty);
    }

    int checkStock(int sku) const override {
        auto it = stock_.find(sku);
        return (it == stock_.end()) ? 0 : it->second;
    }

    vector<Product *> listAvailableProducts() const override {
        vector<Product *> items;
        for (const auto &entry : stock_) {
            if (entry.second > 0 && products_.count(entry.first) != 0) {
                items.push_back(products_.at(entry.first));
            }
        }
        return items;
    }

private:
    unordered_map<int, int> stock_;
    unordered_map<int, Product *> products_;
};

class InventoryManager {
public:
    explicit InventoryManager(InventoryStore *store) : store_(store) {}

    ~InventoryManager() { delete store_; }

    void addStock(int sku, int qty) {
        Product *product = ProductFactory::createProduct(sku);
        store_->addProduct(product, qty);
    }

    void removeStock(int sku, int qty) { store_->removeProduct(sku, qty); }
    int checkStock(int sku) const { return store_->checkStock(sku); }
    vector<Product *> availableProducts() const { return store_->listAvailableProducts(); }

private:
    InventoryStore *store_;
};

} // namespace blinkit_lld

#endif // BLINKIT_LLD_INVENTORY_INVENTORY_H

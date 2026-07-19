// ============================================================================
//  core/BlinkitSystem.h — SYSTEM KA DIL: managers + order splitting + state ❤️
// ----------------------------------------------------------------------------
//  Ye file sabse bada hai kyunki isme poore system ka core logic hai.
//  Multiple patterns + ek smart algorithm:
//
//  1. SINGLETON — DarkStoreManager + OrderManager (ek-ek central manager,
//     Meyers style: static local instance). Poore app me ek hi store-registry
//     aur ek hi order-manager.
//
//  2. FACADE (BlinkitSystem overall) — managers ko coordinate karte hain;
//     client (main) sirf simple calls karta (showCatalog, placeOrder,
//     updateOrderStatus) — andar ka splitting/distance/fee jhamela chhupa.
//
//  3. STATE MACHINE — OrderStatus transitions. Order sirf VALID order me
//     aage badh sakta: PLACED -> CONFIRMED -> PACKING -> OUT_FOR_DELIVERY
//     -> DELIVERED (ya kabhi bhi CANCELLED). isValidTransition() galat
//     jump (jaise PLACED -> DELIVERED) rok deta hai — exception!
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ ORDER SPLITTING ALGORITHM (placeOrder ka dil):                      │
//  │                                                                          │
//  │   1. Nearby stores nikaalo (5km, distance se sorted — paas wala pehle)  │
//  │   2. Pehla store poora cart de sakta hai? -> haan: single store se do   │
//  │   3. Nahi? -> SPLIT: har nearby store se jitna mil sake lo (greedy),    │
//  │      har store ki apni delivery partner. Jo remaining bacha wo agla     │
//  │      store degа. Sab store cover, phir bhi bacha? "could not fulfill".  │
//  │                                                                          │
//  │  Ye asli Blinkit jaisa hai — ek store me sab nahi to paas ke stores     │
//  │  se mangwa ke alag-alag deliveries!                                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠️ DESIGN NOTE: placeOrder pehle stock DEDUCT karta hai (removeStock) —
//  agar order aage fail ho jaye (payment) to rollback nahi hota (reserved
//  stock leak). Real system me "reserve then confirm" pattern chahiye
//  (jaise L39 Memento se rollback). Interview me ye limitation bolna good sign.
// ============================================================================
#ifndef BLINKIT_LLD_CORE_BLINKITSYSTEM_H
#define BLINKIT_LLD_CORE_BLINKITSYSTEM_H

#include <bits/stdc++.h>

#include "../inventory/Inventory.h"
#include "../inventory/ReplenishStrategy.h"
#include "../models/UserCart.h"

using namespace std;

namespace blinkit_lld {

constexpr double kServiceRadiusKm = 5.0;

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

class DeliveryPartner {
public:
    explicit DeliveryPartner(string name) : name_(std::move(name)) {}
    const string &getName() const { return name_; }

private:
    string name_;
};

struct OrderLineItem {
    int sku;
    string productName;
    int quantity;
    double unitPrice;
};

struct OrderFulfillmentSlice {
    string darkStoreName;
    vector<OrderLineItem> items;
    string deliveryPartnerName;
};

class DarkStore {
public:
    DarkStore(string name, double x, double y, InventoryStoreType storeType = InventoryStoreType::DB)
        : name_(std::move(name)), x_(x), y_(y), inventory_(new InventoryManager(InventoryStoreFactory::create(storeType))),
          replenishStrategy_(nullptr) {}

    ~DarkStore() {
        delete inventory_;
        delete replenishStrategy_;
    }

    double distanceTo(double ux, double uy) const {
        return sqrt((x_ - ux) * (x_ - ux) + (y_ - uy) * (y_ - uy));
    }

    void setReplenishStrategy(ReplenishStrategy *strategy) {
        delete replenishStrategy_;
        replenishStrategy_ = strategy;
    }

    void runReplenishment(const map<int, int> &itemsToReplenish) {
        if (replenishStrategy_ != nullptr) {
            replenishStrategy_->replenish(inventory_, itemsToReplenish);
        }
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
    ReplenishStrategy *replenishStrategy_;
};

class DarkStoreManager {
public:
    static DarkStoreManager *getInstance() {
        static DarkStoreManager instance;
        return &instance;
    }

    void registerDarkStore(DarkStore *store) { stores_.push_back(store); }

    // User ke (ux,uy) se maxDistanceKm ke andar wale stores, PAAS se DOOR
    // sorted. Order splitting isi order pe chalta (closest store pehle try) —
    // isliye sorting zaroori: kam distance = kam delivery time/cost.
    vector<DarkStore *> nearbyStores(double ux, double uy, double maxDistanceKm) const {
        vector<pair<double, DarkStore *>> candidates;
        for (DarkStore *store : stores_) {
            const double distance = store->distanceTo(ux, uy);
            if (distance <= maxDistanceKm) {   // 5km service radius ke andar hi
                candidates.push_back({distance, store});
            }
        }
        // Distance se sort — front() hamesha closest store hoga
        sort(candidates.begin(), candidates.end(),
             [](const auto &a, const auto &b) { return a.first < b.first; });

        vector<DarkStore *> result;
        for (const auto &entry : candidates) {
            result.push_back(entry.second);
        }
        return result;
    }

    void showCatalogWithinRadius(double ux, double uy, double radiusKm, const string &userName) const {
        cout << "\n[Catalog] Products available within " << radiusKm << " KM for " << userName << ":\n";
        const vector<DarkStore *> nearby = nearbyStores(ux, uy, radiusKm);
        if (nearby.empty()) {
            cout << "  No dark stores in range.\n";
            return;
        }

        map<int, pair<string, double>> skuCatalog;
        for (DarkStore *store : nearby) {
            for (Product *product : store->products()) {
                if (skuCatalog.count(product->getSku()) == 0) {
                    skuCatalog[product->getSku()] = {product->getName(), product->getPrice()};
                }
            }
        }

        for (const auto &entry : skuCatalog) {
            cout << "  SKU " << entry.first << " - " << entry.second.first << " @ Rs " << entry.second.second << "\n";
        }
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
        const vector<pair<Product *, int>> requestedItems = user->getCart()->getItems();
        if (requestedItems.empty()) {
            cout << "Cart is empty.\n";
            return -1;
        }

        vector<DarkStore *> nearbyStores =
            DarkStoreManager::getInstance()->nearbyStores(user->getX(), user->getY(), kServiceRadiusKm);
        if (nearbyStores.empty()) {
            cout << "No dark stores within " << kServiceRadiusKm << " KM. Cannot fulfill order.\n";
            return -1;
        }

        const int orderId = ++orderCounter_;
        orderStatus_[orderId] = OrderStatus::PLACED;
        vector<OrderFulfillmentSlice> slices;
        map<int, int> remaining;

        for (const auto &item : requestedItems) {
            remaining[item.first->getSku()] += item.second;
        }

        // FAST PATH check: kya SABSE PAAS wala store poora cart de sakta hai?
        // (nearbyStores distance-sorted hai, front() = closest). Agar haan to
        // ek hi store se, ek hi delivery — best case (splitting ki zaroorat nahi).
        DarkStore *firstStore = nearbyStores.front();
        bool allInFirstStore = true;
        for (const auto &entry : remaining) {
            if (firstStore->checkStock(entry.first) < entry.second) {
                allInFirstStore = false;  // ek bhi item kam? -> split karna padega
                break;
            }
        }

        cout << "\n[OrderManager] Placing order #" << orderId << " for " << user->getName() << "\n";

        if (allInFirstStore) {
            cout << "  All items available at: " << firstStore->getName() << "\n";
            OrderFulfillmentSlice slice;
            slice.darkStoreName = firstStore->getName();
            slice.deliveryPartnerName = "Partner1";

            for (const auto &entry : remaining) {
                const int sku = entry.first;
                const int qty = entry.second;
                Product *product = ProductFactory::createProduct(sku);
                firstStore->removeStock(sku, qty);
                slice.items.push_back({sku, product->getName(), qty, product->getPrice()});
                delete product;
            }
            slices.push_back(slice);
            cout << "  Assigned delivery partner: Partner1\n";
        } else {
            // SPLIT PATH: ek store se sab nahi milega, to nearby stores me
            // baant do. Greedy: har store se jitna mil sake lo, remaining
            // agla store degа. Har contributing store ki apni delivery partner.
            cout << "  Splitting order across dark stores...\n";
            int partnerId = 1;

            for (DarkStore *store : nearbyStores) {
                if (remaining.empty()) {
                    break;  // pura cart fulfill ho gaya, aur stores nahi chahiye
                }

                cout << "   Checking: " << store->getName() << "\n";
                OrderFulfillmentSlice slice;
                slice.darkStoreName = store->getName();
                bool assignedFromStore = false;
                vector<int> fulfilledSkus;

                for (const auto &entry : remaining) {
                    const int sku = entry.first;
                    const int qtyNeeded = entry.second;
                    const int availableQty = store->checkStock(sku);
                    if (availableQty <= 0) {
                        continue;
                    }

                    const int takenQty = min(availableQty, qtyNeeded);
                    store->removeStock(sku, takenQty);
                    Product *product = ProductFactory::createProduct(sku);
                    slice.items.push_back({sku, product->getName(), takenQty, product->getPrice()});
                    delete product;

                    cout << "     " << store->getName() << " supplies SKU " << sku << " x" << takenQty << "\n";

                    if (qtyNeeded > takenQty) {
                        remaining[sku] = qtyNeeded - takenQty;
                    } else {
                        fulfilledSkus.push_back(sku);
                    }
                    assignedFromStore = true;
                }

                for (int sku : fulfilledSkus) {
                    remaining.erase(sku);
                }

                if (assignedFromStore) {
                    slice.deliveryPartnerName = "Partner" + to_string(partnerId++);
                    slices.push_back(slice);
                    cout << "     Assigned: " << slice.deliveryPartnerName << " for " << store->getName() << "\n";
                }
            }

            if (!remaining.empty()) {
                cout << "  Could not fulfill completely:\n";
                for (const auto &entry : remaining) {
                    cout << "    SKU " << entry.first << " x" << entry.second << "\n";
                }
            }
        }

        printOrderSummary(orderId, user, slices);
        return orderId;
    }

    void updateOrderStatus(int orderId, OrderStatus nextStatus) {
        auto it = orderStatus_.find(orderId);
        if (it == orderStatus_.end()) {
            throw runtime_error("Order not found");
        }
        if (!isValidTransition(it->second, nextStatus)) {
            throw runtime_error("Invalid order status transition from " + orderStatusToString(it->second) + " to " +
                                orderStatusToString(nextStatus));
        }
        it->second = nextStatus;
        cout << "Order #" << orderId << " -> " << orderStatusToString(nextStatus) << "\n";
    }

private:
    unordered_map<int, OrderStatus> orderStatus_;
    int orderCounter_ = 0;

    static void printOrderSummary(int orderId, User *user, const vector<OrderFulfillmentSlice> &slices) {
        double itemTotal = 0.0;
        cout << "\n[OrderManager] Order #" << orderId << " summary\n";
        cout << "  User: " << user->getName() << "\n";

        for (const OrderFulfillmentSlice &slice : slices) {
            cout << "  Store: " << slice.darkStoreName << " | Partner: " << slice.deliveryPartnerName << "\n";
            for (const OrderLineItem &line : slice.items) {
                cout << "    SKU " << line.sku << " (" << line.productName << ") x" << line.quantity << " @ Rs "
                     << line.unitPrice << "\n";
                itemTotal += line.unitPrice * line.quantity;
            }
        }

        const double distanceKm = DarkStoreManager::getInstance()
                                      ->nearbyStores(user->getX(), user->getY(), kServiceRadiusKm)
                                      .front()
                                      ->distanceTo(user->getX(), user->getY());
        const DeliveryFeeBreakdown fee = calculateDeliveryFee(distanceKm);

        cout << "  Item total: Rs " << itemTotal << "\n";
        cout << "  Delivery fee [Base: " << fee.baseFee << ", Distance: " << fee.distanceFee
             << ", Surge: x" << fee.surgeMultiplier << ", Total: " << fee.totalFee << "]\n";
        cout << "  Final payable: Rs " << (itemTotal + fee.totalFee) << "\n";
    }

    static DeliveryFeeBreakdown calculateDeliveryFee(double distanceKm) {
        const double baseFee = 20.0;
        const double perKm = 5.0;
        const double distanceFee = distanceKm * perKm;
        const double surgeMultiplier = (distanceKm > 3.0) ? 1.5 : 1.0;
        const double totalFee = (baseFee + distanceFee) * surgeMultiplier;
        return {baseFee, distanceFee, surgeMultiplier, totalFee};
    }

    // >>> STATE MACHINE ka core: sirf VALID transitions allow karo <<<
    // Order lifecycle: PLACED -> CONFIRMED -> PACKING -> OUT_FOR_DELIVERY
    //                  -> DELIVERED. Har state se sirf AGLA state valid hai.
    static bool isValidTransition(OrderStatus current, OrderStatus next) {
        // TERMINAL states — delivered/cancelled ke baad kuch nahi ho sakta
        if (current == OrderStatus::CANCELLED || current == OrderStatus::DELIVERED) {
            return false;
        }
        // CANCELLED har (non-terminal) state se allowed hai (order kabhi bhi cancel)
        if (next == OrderStatus::CANCELLED) {
            return true;
        }
        // Normal forward transitions — har state ka sirf EK valid agla state.
        // Isse "PLACED -> DELIVERED" jaisa GALAT jump rok jaata hai (packing/
        // delivery skip nahi kar sakte). Galat transition = exception (upar).
        if (current == OrderStatus::PLACED && next == OrderStatus::CONFIRMED) return true;
        if (current == OrderStatus::CONFIRMED && next == OrderStatus::PACKING) return true;
        if (current == OrderStatus::PACKING && next == OrderStatus::OUT_FOR_DELIVERY) return true;
        if (current == OrderStatus::OUT_FOR_DELIVERY && next == OrderStatus::DELIVERED) return true;
        return false;  // baaki sab invalid
    }
};

} // namespace blinkit_lld

#endif // BLINKIT_LLD_CORE_BLINKITSYSTEM_H

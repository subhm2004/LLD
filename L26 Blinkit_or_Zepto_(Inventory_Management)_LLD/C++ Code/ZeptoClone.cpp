// ============================================================================
//  ZeptoClone.cpp — BLINKIT/ZEPTO inventory system (single-file MONOLITH)
// ----------------------------------------------------------------------------
//  Poora quick-commerce inventory + order system EK file me. Modular folder-
//  based version parent folder me hai (core/, inventory/, models/ + main.cpp).
//  Detail: ../design_patterns_used.md
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  STAR FEATURE — ORDER SPLITTING:                                        │
//  │   Ek store poora cart na de sake to nearby stores me baant do (greedy,  │
//  │   closest se), har store ki apni delivery partner. Asli Blinkit jaisa!  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERNS: Strategy (replenish + storage backend) + Factory (product +
//  store) + Facade (system) + Singleton (managers) + State machine (order
//  status). Modular version me ye sab clean files me; ye monolith "sab ek
//  jagah" reference. Dono ka behavior same.
// ============================================================================
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>

using namespace std;

// ----------------------------------------------------------------------------
// [DESIGN PATTERN: Simple Factory Pattern]
// ----------------------------------------------------------------------------
// ProductFactory class `createProduct` method ke zariye SKUs ko valid parameters
// (name, price) ke sath link karke Product instances initiate karti hai.
// client ko product parameters (price etc.) manual hardcode nahi karne padte,
// factory input code dynamically resolve karti hai.
// ----------------------------------------------------------------------------

class Product {
private:
    int sku;       // Stock Keeping Unit (Unique Identifier for each product)
    string name;   // Product Name (e.g. Apple, Banana)
    double price;  // Product Price per unit

public:
    Product(int id, string nm, double pr) {
        sku   = id;
        name  = nm;
        price = pr;
    }

    // Getters
    int getSku() { return this->sku; }
    string getName() { return this->name; }
    double getPrice() { return this->price; }
};

// Factory Class for Product instantiation
class ProductFactory {
public:
    // Simple Factory static method: resolves SKU configurations
    static Product* createProduct(int sku) {
        // In real systems, yeh detail database se fetch hoti hai.
        string name;
        double price;

        if (sku == 101) {
            name  = "Apple";
            price = 20;
        }
        else if (sku == 102) {
            name  = "Banana";
            price = 10;
        }
        else if (sku == 103) {
            name  = "Chocolate";
            price = 50;
        }
        else if (sku == 201) {
            name  = "T-Shirt";
            price = 500;
        }
        else if (sku == 202) {
            name  = "Jeans";
            price = 1000;
        }
        else {
            name  = "Item" + to_string(sku);
            price = 100;
        }
        return new Product(sku, name, price);
    }
};

// ----------------------------------------------------------------------------
// Inventory Store interface and implementations
// ----------------------------------------------------------------------------
// InventoryStore abstract class repository pattern standard define karti hai.
// DbInventoryStore in-memory maps (SKU maps) manage karta hai aur stock adjustments,
// queries ko concrete execute karta hai.
// ----------------------------------------------------------------------------

class InventoryStore {
public:
    virtual ~InventoryStore() {}
    virtual void addProduct(Product* prod, int qty) = 0;
    virtual void removeProduct(int sku, int qty) = 0;
    virtual int checkStock(int sku) = 0;
    virtual vector<Product*> listAvailableProducts() = 0;
};

class DbInventoryStore : public InventoryStore {
private:
    map<int,int>* stock;             // SKU -> Quantity map
    map<int,Product*>* products;     // SKU -> Product pointer mapping table

public:
    DbInventoryStore() {
        stock = new map<int,int>();
        products = new map<int,Product*>();
    }
    
    // Destructor: products and stock dynamically allocated map memory clean karna
    ~DbInventoryStore() {
        for (auto it : *products) {
            delete it.second; // Clean individual product memory
        }
        delete products;
        delete stock;
    }

    // Add Product updates
    void addProduct(Product* prod, int qty) override {
        int sku = prod->getSku();
        if (products->count(sku) == 0) {
            // Agar pehle se store me product model register nahi hai
            (*products)[sku] = prod;
        } else {
            // Duplicate model dynamic reference clean parameters
            delete prod;
        }
        (*stock)[sku] += qty; // Add stock value
    }

    // Remove stock items
    void removeProduct(int sku, int qty) override {
        if (stock->count(sku) == 0) 
            return;

        int currentQuantity = (*stock)[sku];
        int remainingQuantity = currentQuantity - qty;
        if (remainingQuantity > 0) {
            (*stock)[sku] = remainingQuantity;
        } else {
            stock->erase(sku); // Stock finish, map key deleted
        }
    }

    // Return stock quantity
    int checkStock(int sku) override {
        if (stock->count(sku) == 0) 
            return 0;

        return (*stock)[sku];
    }

    // Get list of available active stock products
    vector<Product*> listAvailableProducts() override {
        vector<Product*> available;
        for (auto it : *stock) {
            int sku = it.first;
            int qty = it.second;
            if (qty > 0 && products->count(sku)) {
                available.push_back((*products)[sku]);
            }
        }
        return available;
    }
};

// ----------------------------------------------------------------------------
// InventoryManager orchestrates local store requests
// ----------------------------------------------------------------------------
class InventoryManager {
private:
    InventoryStore* store; // Loosely coupled repository pointer

public:
    InventoryManager(InventoryStore* store) {
        this->store = store;
    }

    // Destructor: Fix leak of raw store pointer
    ~InventoryManager() {
        delete store;
    }

    void addStock(int sku, int qty) {
        Product* prod = ProductFactory::createProduct(sku);
        store->addProduct(prod, qty);
        cout << "[InventoryManager] Added SKU " << sku << " Qty " << qty << endl;
    }

    void removeStock(int sku, int qty) {
        store->removeProduct(sku, qty); 
    }

    int checkStock(int sku) {
        return store->checkStock(sku);
    }

    vector<Product*> getAvailableProducts() {
        return store->listAvailableProducts();
    }
};

// ----------------------------------------------------------------------------
// [DESIGN PATTERN: Strategy Pattern]
// ----------------------------------------------------------------------------
// ReplenishStrategy interface automatic inventory stock replenishment behavior define karti hai.
// - ThresholdReplenishStrategy check karti hai ki current stock threshold limit (e.g. < 3) se niche
//   to nahi gaya. Agar haan, to automatic order/replenish invoke karti hai.
// - WeeklyReplenishStrategy routine calendar scheduling ke dynamic checks define karti hai.
// - Strategy implementation is dynamic logic resolution ko client DarkStore settings se clean decouple rakhta hai.
// ----------------------------------------------------------------------------

class ReplenishStrategy {
public:
    virtual void replenish(InventoryManager* manager, map<int,int> itemsToReplenish) = 0;
    virtual ~ReplenishStrategy() {}
};

// Threshold based replenishment (Concrete Strategy 1)
class ThresholdReplenishStrategy : public ReplenishStrategy {
private:
    int threshold; // Minimum safe limit trigger
public:
    ThresholdReplenishStrategy(int threshold) {
        this->threshold = threshold;
    }
    void replenish(InventoryManager* manager, map<int,int> itemsToReplenish) override {
        cout << "[ThresholdReplenish] Checking threshold... \n";
        for (auto it : itemsToReplenish) {
            int sku = it.first;
            int qtyToAdd = it.second;
            int current  = manager->checkStock(sku);
            
            // Safe stock limits validation
            if (current < threshold) {
                manager->addStock(sku, qtyToAdd);
                cout << "  -> SKU " << sku << " was " << current 
                     << ", replenished by " << qtyToAdd << endl;
            }
        }
    }
};

// Calendar based replenishment (Concrete Strategy 2)
class WeeklyReplenishStrategy : public ReplenishStrategy {
public:
    WeeklyReplenishStrategy() {}
    void replenish(InventoryManager* manager, map<int,int> itemsToReplenish) override {
        cout << "[WeeklyReplenish] Weekly replenishment triggered for inventory.\n";
    }
};

// ----------------------------------------------------------------------------
// DarkStore representing local fulfillment micro-warehouses
// ----------------------------------------------------------------------------
class DarkStore {
private:
    string name;
    double x, y;                       // Geo Coordinates (User-to-Store distances mapping)
    InventoryManager* inventoryManager;// Local inventory controller
    ReplenishStrategy* replenishStrategy; // Bound Strategy pointer

public:
    DarkStore(string n, double x_coord, double y_coord) {
        name = n;
        x = x_coord;
        y = y_coord;
        
        // Dynamic construction setup
        inventoryManager = new InventoryManager(new DbInventoryStore);
        replenishStrategy = nullptr;
    }
    
    ~DarkStore() {
        delete inventoryManager; // Destructor triggers chain deletes down to DB store
        if (replenishStrategy) delete replenishStrategy;
    }

    // Distance mapping formula (Euclidean distance)
    double distanceTo(double ux, double uy) {
        return sqrt((x - ux)*(x - ux) + (y - uy)*(y - uy));
    }

    // Triggers replenishment strategy
    void runReplenishment(map<int,int> itemsToReplenish) {
        if (replenishStrategy) {
            replenishStrategy->replenish(inventoryManager, itemsToReplenish);
        }
    }

    // Delegation to internal manager
    vector<Product*> getAllProducts() {
        return inventoryManager->getAvailableProducts();
    }

    int checkStock(int sku) {
        return inventoryManager->checkStock(sku);
    }

    void removeStock(int sku, int qty) {
        inventoryManager->removeStock(sku, qty); 
    }

    void addStock(int sku, int qty) {
        inventoryManager->addStock(sku, qty);
    }

    // Strategy configuration setter
    void setReplenishStrategy(ReplenishStrategy* strategy) {
        if (replenishStrategy) delete replenishStrategy; // Delete older active strategy if configured
        this->replenishStrategy = strategy;
    }

    string getName() { return this->name; }
    double getXCoordinate() { return this->x; }
    double getYCoordinate() { return this->y; }
    InventoryManager* getInventoryManager() { return this->inventoryManager; }
};

// ----------------------------------------------------------------------------
// [DESIGN PATTERN: Singleton Pattern]
// ----------------------------------------------------------------------------
// DarkStoreManager pure system me Dark Stores ke configurations registry index ko manage 
// karta hai. Global coordinate checks me nearby dark stores query resolve karne me yeh helper
// roles execute karta hai. Singleton layout registry operations consistent rakhta hai.
// ----------------------------------------------------------------------------

class DarkStoreManager {
private:
    vector<DarkStore*>* darkStores;
    static DarkStoreManager* instance;

    DarkStoreManager() {
        darkStores = new vector<DarkStore*>();
    }

public:
    static DarkStoreManager* getInstance() {
        if(instance == nullptr) {
            instance = new DarkStoreManager();
        }
        return instance;
    }

    void registerDarkStore(DarkStore* ds) {
        darkStores->push_back(ds);
    }

    // Returns a sorted list of dark stores sorted by shortest distance inside maxDistance limit
    vector<DarkStore*> getNearbyDarkStores(double ux, double uy, double maxDistance) {
        vector<pair<double,DarkStore*>> distList;
        for (auto ds : *darkStores) {
            double d = ds->distanceTo(ux, uy);
            if (d <= maxDistance) {
                distList.push_back(make_pair(d, ds));
            }
        }
        
        // Sort stores by proximity (nearest store first)
        sort(distList.begin(), distList.end(),
             [](auto &a, auto &b){ return a.first < b.first; });

        vector<DarkStore*> result;
        for (auto &p : distList) {
            result.push_back(p.second);
        }
        return result;
    }

    ~DarkStoreManager() {
        for (auto ds : *darkStores) {
            delete ds;
        }
        delete darkStores;
    }
};

DarkStoreManager* DarkStoreManager::instance = nullptr;

// ----------------------------
// User & Cart Classes
// ----------------------------
class Cart {
public:
    vector<pair<Product*,int>> items;  // Cart list mapping (Product pointers, Quantity purchased)

    void addItem(int sku, int qty) {
        Product* prod = ProductFactory::createProduct(sku);
        items.push_back(make_pair(prod, qty));
        cout << "[Cart] Added SKU " << sku << " (" << prod->getName() 
             << ") x" << qty << endl;
    }

    double getTotal() {
        double sum = 0.0;
        for (auto &it : items) {
            sum += (it.first->getPrice() * it.second);
        }
        return sum;
    }

    vector<pair<Product*,int>> getItems() { return items; }

    ~Cart() {
        for (auto &it : items) {
            delete it.first; // Clean cart allocated product descriptors
        }
    }
};

class User {
public:
    string name;
    double x, y; // Geo-coordinates location of the customer
    Cart* cart;  // User aggregates/owns a shopping cart

    User(string n, double x_coord, double y_coord) {
        name = n;
        x = x_coord;
        y = y_coord;
        cart = new Cart();
    }
    
    ~User() {
        delete cart;
    }

    Cart* getCart() { return cart; }
};

// ----------------------------
// DeliveryPartner Class representing the delivery fleet member
// ----------------------------
class DeliveryPartner {
public:
    string name;
    DeliveryPartner(string n) {
        name = n;
    }
};

// ----------------------------------------------------------------------------
// Order & OrderManager (Singleton Pattern)
// ----------------------------------------------------------------------------
// OrderManager client coordinates coordinate checking logic process execute karta hai.
// - `placeOrder()` nearest stores verification run karti hai.
// - Agar sabhi items nearest store me present hain, to processing single store complete
//   ho jati hai aur 1 delivery partner call dynamic allocate hota hai.
// - Agar stock split options process hote hain, to multiple stores items deduct karte hain
//   aur multiple order tracking partners assign hotey hain.
// ----------------------------------------------------------------------------

class Order {
public:
    static int nextId;
    int orderId;
    User* user;
    vector<pair<Product*,int>> items;     // Final assigned items list for tracking
    vector<DeliveryPartner*> partners;   // Delivery partners assigned to this order
    double totalAmount;

    Order(User* u) {
        orderId = nextId++;
        user = u;
        totalAmount = 0.0;
    }
    
    // Destructor: Order takes ownership of assigned DeliveryPartners and Products copies
    ~Order() {
        for (auto* dp : partners) delete dp;
        for (auto& item : items) delete item.first;
    }
};

int Order::nextId = 1;

// Singleton Order Manager
class OrderManager {
private:
    vector<Order*>* orders;
    static OrderManager* instance;

    OrderManager() {
        orders = new vector<Order*>();
    }

public:
    static OrderManager* getInstance() {
        if(instance == nullptr) {
            instance = new OrderManager();
        }
        return instance;
    }

    // Central place order workflow logic
    void placeOrder(User* user, Cart* cart) {
        cout << "\n[OrderManager] Placing Order for: " << user->name << "\n";

        vector<pair<Product*,int>> requestedItems = cart->getItems();
    
        // 1) Find nearby dark stores within 5 KM radius limits
        double maxDist = 5.0;
        vector<DarkStore*> nearbyDarkStores = DarkStoreManager::getInstance()->getNearbyDarkStores(user->x, user->y, maxDist);
        
        if (nearbyDarkStores.empty()) {
            cout << "  No dark stores within 5 KM. Cannot fulfill order.\n";
            return;
        }
    
        // 2) Proximity optimization: check if the closest store has all items (Single delivery route)
        DarkStore* firstStore = nearbyDarkStores.front();
        bool allInFirst = true;
        for (pair<Product*,int>& item : requestedItems) {
            int sku = item.first->getSku();
            int qty = item.second;

            if (firstStore->checkStock(sku) < qty) {
                allInFirst = false;
                break;
            }
        }
    
        Order* order = new Order(user);

        // Case A: Fulfill order from a single store (Optimized route)
        if (allInFirst) {
            cout << "  All items at: " << firstStore->getName() << "\n";

            for (pair<Product*,int>& item : requestedItems) {
                int sku = item.first->getSku();
                int qty = item.second;
                firstStore->removeStock(sku, qty);
                
                // Copy product for the order history logs
                order->items.push_back({ ProductFactory::createProduct(sku), qty });
            }

            order->totalAmount = cart->getTotal();
            order->partners.push_back(new DeliveryPartner("Partner1")); // Single partner allocation
            cout << "  Assigned Delivery Partner: Partner1\n";
        } 
        // Case B: Split order execution across multiple warehouses (Split route)
        else {
            cout << "  Splitting order across stores...\n";

            map<int,int> allItems; // SKU -> Qty map of remaining items to fulfill
            for (pair<Product*,int>& item : requestedItems) {
                allItems[item.first->getSku()] = item.second;
            }
    
            int partnerId = 1;
            for (DarkStore* store : nearbyDarkStores) {
                if (allItems.empty()) break; // Exit loop if all SKUs are fulfilled

                cout << "   Checking: " << store->getName() << "\n";
    
                bool assigned = false;
                vector<int> toErase;
                
                for (auto& [sku, qtyNeeded] : allItems) {
                    int availableQty = store->checkStock(sku);
                    if (availableQty <= 0) continue;
    
                    // Take minimum of available vs required stock
                    int takenQty = min(availableQty, qtyNeeded);
                    store->removeStock(sku, takenQty);

                    cout << "     " << store->getName() << " supplies SKU " << sku 
                          << " x" << takenQty << "\n";

                    order->items.push_back({ ProductFactory::createProduct(sku), takenQty });
    
                    // Adjust quantities
                    if (qtyNeeded > takenQty) {
                        allItems[sku] = qtyNeeded - takenQty;
                    } else {
                        toErase.push_back(sku);
                    }
                    assigned = true;
                }

                // Clean-up fully-satisfied SKUs from mapping table
                for (int sku : toErase) allItems.erase(sku);
    
                // Assign a dedicated delivery partner for this warehouse route
                if (assigned) {
                    string pname = "Partner" + to_string(partnerId++);
                    order->partners.push_back(new DeliveryPartner(pname));
                    cout << "     Assigned: " << pname << " for " << store->getName() << "\n";
                }
            }
    
            // Error report log for unfulfilled items
            if (!allItems.empty()) {
                cout << "  Could not fulfill:\n";
                for (auto& [sku, qty] : allItems) {
                    cout << "    SKU " << sku << " x" << qty << "\n";
                }
            }

            // Re-calculate split order total amounts
            double sum = 0;
            for (auto& item : order->items) {
                sum += item.first->getPrice() * item.second;
            }
            order->totalAmount = sum;
        }
    
        // Display order details summary
        cout << "\n[OrderManager] Order #" << order->orderId << " Summary:\n";
        cout << "  User: " << user->name << "\n  Items:\n";
        for (auto& item : order->items) {
            cout << "    SKU " << item.first->getSku()
                 << " (" << item.first->getName() << ") x" << item.second
                 << " @ ₹" << item.first->getPrice() << "\n";
        }
        cout << "  Total: ₹" << order->totalAmount << "\n  Partners:\n";
        for (auto* dp : order->partners) {
            cout << "    " << dp->name << "\n";
        }
        cout << endl;
    
        orders->push_back(order); // Store the completed order inside system database vector
    }    

    vector<Order*> getAllOrders() { return *orders; }

    ~OrderManager() {
        for (auto ord : *orders) {
            delete ord; // Triggers internal Order destructor to clean partners/products copies
        }
        delete orders;
    }
};

// Initialize static pointers
OrderManager* OrderManager::instance = nullptr;

// ----------------------------
// ZeptoHelper: Utility Coordinator (Facade Pattern style)
// ----------------------------
class ZeptoHelper {
public:
    // Prints consolidated unique list of products nearby
    static void showAllItems(User* user) {
        cout << "\n[Zepto] All Available products within 5 KM for " << user->name << ":\n";

        DarkStoreManager* dsManager = DarkStoreManager::getInstance();
        vector<DarkStore*> nearbyStores = dsManager->getNearbyDarkStores(user->x, user->y, 5.0);

        map<int, double> skuToPrice;
        map<int, string> skuToName;

        for (DarkStore* darkStore : nearbyStores) {
            vector<Product*> products = darkStore->getAllProducts();
            for (Product* product : products) {
                int sku = product->getSku();
                if (skuToPrice.count(sku) == 0) {
                    skuToPrice[sku] = product->getPrice();
                    skuToName[sku]  = product->getName();
                }
            }
        }

        for (auto& entry : skuToPrice) {
            int sku = entry.first;
            double price = entry.second;
            cout << "  SKU " << sku << " - " << skuToName[sku] << " @ ₹" << price << "\n";
        }
    }

    // Initialize mock database values
    static void initialize() {
        auto dsManager = DarkStoreManager::getInstance();

        // DarkStore A
        DarkStore* darkStoreA = new DarkStore("DarkStoreA", 0.0, 0.0);
        darkStoreA->setReplenishStrategy(new ThresholdReplenishStrategy(3));
        cout << "\nAdding stocks in DarkStoreA...." << endl;  
        darkStoreA->addStock(101, 5); // Apple
        darkStoreA->addStock(102, 2); // Banana

        // DarkStore B
        DarkStore* darkStoreB = new DarkStore("DarkStoreB", 4.0, 1.0);
        darkStoreB->setReplenishStrategy(new ThresholdReplenishStrategy(3));
        cout << "\nAdding stocks in DarkStoreB...." << endl; 
        darkStoreB->addStock(101, 3); // Apple
        darkStoreB->addStock(103, 10); // Chocolate

        // DarkStore C
        DarkStore* darkStoreC = new DarkStore("DarkStoreC", 2.0, 3.0);
        darkStoreC->setReplenishStrategy(new ThresholdReplenishStrategy(3));
        cout << "\nAdding stocks in DarkStoreC...." << endl; 
        darkStoreC->addStock(102, 5); // Banana
        darkStoreC->addStock(201, 7); // T-Shirt

        // Register in manager list
        dsManager->registerDarkStore(darkStoreA);
        dsManager->registerDarkStore(darkStoreB);
        dsManager->registerDarkStore(darkStoreC);
    }
};

// ----------------------------
// Main: Application Entry point
// ----------------------------
int main() {
    // 1) Initialize warehouses and setup mock stock database
    ZeptoHelper::initialize();

    // 2) Initialize User profile coming online
    User* user = new User("Aditya", 1.0, 1.0);
    cout <<"\nUser with name " << user->name << " comes on platform" << endl;

    // 3) Display items within range (5 KM)
    ZeptoHelper::showAllItems(user);

    // 4) Add items to customer shopping cart
    cout << "\nAdding items to cart\n";
    Cart* cart = user->getCart();
    cart->addItem(101, 4);  // apple
    cart->addItem(102, 3);  // banana
    cart->addItem(103, 2);  // chocolate

    // 5) Execute Order validation and dispatching via Manager
    OrderManager::getInstance()->placeOrder(user, user->cart);

    // 6) Clean up allocated memory dynamically
    delete user;
    
    // Clean Manager Singletons properly to release all aggregated entities (DarkStores, Order logs etc.)
    delete DarkStoreManager::getInstance();
    delete OrderManager::getInstance();

    return 0;
}

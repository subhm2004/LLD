// ============================================================================
//  DiscountCoupon.cpp  —  Discount Coupon Engine (original single-file version)
// ----------------------------------------------------------------------------
//  Poore coupon engine ka original monolithic version (saari classes ek file me).
//  Modular, folder-based version parent folder me hai (coupons/, strategies/,
//  factories/, core/). Reference ke liye preserve kiya gaya.
//  Is file me detail me (Hinglish + English mix) comments add kiye gaye hain.
// ============================================================================
#include <bits/stdc++.h>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

using namespace std;

// Enum to identify different types of discount calculations
enum StrategyType { FLAT, PERCENT, PERCENT_WITH_CAP };

// ----------------------------------------------------------------------------
// [DESIGN PATTERN: Strategy Pattern]
// ----------------------------------------------------------------------------
// Strategy Pattern ka use discount calculator math ko coupon rule algorithms se
// alag (decouple) karne ke liye kiya gaya hai.
// - Discount calculation ka logic (Flat discount, Percentage discount, or Percentage with Cap)
//   alag strategy classes me encapsulated hai.
// - Coupon classes (like SeasonalOffer, LoyaltyDiscount) concrete calculation algorithm ko
//   hardcode karne ke bajaye `DiscountStrategy` interface ke through bind (composition) karti hain.
// - Isse naye mathematical models implement karna asaan ho jata hai bina coupon classes ko modify kiye.
// ----------------------------------------------------------------------------

// Discount Strategy Interface (Abstract Strategy)
class DiscountStrategy {
public:
  // Virtual destructor ensure karta hai ki dynamically allocated subclass objects leak na ho
  virtual ~DiscountStrategy() {}
  
  // calculate: Base amount accept karke net discount amount return karta hai
  virtual double calculate(double baseAmount) = 0;
};

// Concrete Strategy 1: Flat Discount
class FlatDiscountStrategy : public DiscountStrategy {
private:
  double amount; // Kitna fixed cash discount dena hai

public:
  FlatDiscountStrategy(double amt) { amount = amt; }

  double calculate(double baseAmount) override {
    // Flat discount total cart value se bada nahi ho sakta (negatives avoid karne ke liye)
    // Example: cart value 80, coupon off 100 => total discount 80 hi milega.
    return min(amount, baseAmount);
  }
};

// Concrete Strategy 2: Percentage-based Discount
class PercentageDiscountStrategy : public DiscountStrategy {
private:
  double percent; // Kitne percent discount dena hai (e.g. 10%)

public:
  PercentageDiscountStrategy(double pct) { percent = pct; }

  double calculate(double baseAmount) override {
    // Standard percentage calculation: (percent / 100) * total
    return (percent / 100.0) * baseAmount;
  }
};

// Concrete Strategy 3: Percentage Discount with an Upper Limit Cap
class PercentageWithCapStrategy : public DiscountStrategy {
private:
  double percent; // Discount percentage
  double cap;     // Max discount limit (Maximum threshold cap)

public:
  PercentageWithCapStrategy(double pct, double capVal) {
    percent = pct;
    cap = capVal;
  }

  double calculate(double baseAmount) override {
    // Pehle percentage discount calculate karenge, fir use cap value ke limit me restrict karenge
    double disc = (percent / 100.0) * baseAmount;
    if (disc > cap) {
      return cap; // Cap threshold limit crossed, apply capped amount only
    }
    return disc;
  }
};

// ----------------------------------------------------------------------------
// [DESIGN PATTERN: Singleton Pattern & Factory Pattern]
// ----------------------------------------------------------------------------
// 1. Singleton: DiscountStrategyManager pure application me single instantiation control
//    rakhta hai. Static functions, private constructor, and block operators iske duplication
//    ko prevent karte hain. Multithreading environments me Double-Checked Locking (DCL)
//    ensure karta hai safety.
// 2. Simple Factory: `getStrategy` method is a factory method jo strategy type ke rules ke basis par
//    concrete strategy objects instantiate aur distribute karta hai.
// ----------------------------------------------------------------------------

class DiscountStrategyManager {
private:
  // Static instance pointer and synchronization mutex for thread-safe access
  static DiscountStrategyManager *instance;
  static mutex instanceMtx;

  // Private constructor to disable direct instantiation
  DiscountStrategyManager() {}

  // Deleting copy & move constructors/assignments to avoid copies or moves
  DiscountStrategyManager(const DiscountStrategyManager &) = delete;
  DiscountStrategyManager &operator=(const DiscountStrategyManager &) = delete;
  DiscountStrategyManager(DiscountStrategyManager &&) = delete;
  DiscountStrategyManager &operator=(DiscountStrategyManager &&) = delete;

  // Private destructor to prevent explicit deletion from outside
  ~DiscountStrategyManager() {}

public:
  // Thread-Safe Singleton Getter utilizing Double-Checked Locking (DCL)
  static DiscountStrategyManager *getInstance() {
    // 1st Check (Lock-Free): Performance speed badhane ke liye. 
    // Agar object already active hai to lock call avoid ho sake.
    if (!instance) {
      // Locking step: Sync issues control karne ke liye
      lock_guard<mutex> lock(instanceMtx);
      
      // 2nd Check: Dual threads race condition protection
      if (!instance) {
        instance = new DiscountStrategyManager();
      }
    }
    return instance;
  }

  // Factory creation algorithm: returns dynamic strategy pointer based on strategy type input
  DiscountStrategy *getStrategy(StrategyType type, double param1,
                                double param2 = 0.0) const {
    if (type == StrategyType::FLAT) {
      return new FlatDiscountStrategy(param1);
    }
    if (type == StrategyType::PERCENT) {
      return new PercentageDiscountStrategy(param1);
    }
    if (type == StrategyType::PERCENT_WITH_CAP) {
      return new PercentageWithCapStrategy(param1, param2);
    }
    return nullptr;
  }
};

// Static instances setup definitions
DiscountStrategyManager *DiscountStrategyManager::instance = nullptr;
mutex DiscountStrategyManager::instanceMtx;

// ----------------------------
// Product Class representing Cart Line Items
// ----------------------------
class Product {
private:
  string name;
  string category; // Helpful in resolving category level discounts (e.g. Clothing/Electronics)
  double price;

public:
  Product(string name, string category, double price) {
    this->name = name;
    this->category = category;
    this->price = price;
  }

  string getName() { return name; }
  string getCategory() const { return category; }
  double getPrice() { return price; }
};

// CartItem represents quantity metadata mapping against a single Product
class CartItem {
private:
  Product *product;
  int quantity;

public:
  CartItem(Product *prod, int qty) {
    product = prod;
    quantity = qty;
  }

  // Subtotal calculated dynamically as: (Price * Quantity)
  double itemTotal() { return product->getPrice() * quantity; }
  const Product *getProduct() { return product; }
};

// Cart represents the customer order bucket
class Cart {
private:
  vector<CartItem *> items;
  double originalTotal; // Fixed baseline total (Useful in checks like minimum spend threshold eligibility)
  double currentTotal;  // Mutable running total. Redundant discount deductions dynamically affect this total.
  bool loyaltyMember;   // Flag to check if user has premium loyalty membership
  string paymentBank;   // Selected Bank gateway during payment (Useful in Resolving Bank specific coupon rules)

public:
  Cart() {
    originalTotal = 0.0;
    currentTotal = 0.0;
    loyaltyMember = false;
    paymentBank = "";
  }

  // Add Item wrapper logic
  void addProduct(Product *prod, int qty = 1) {
    CartItem *item = new CartItem(prod, qty);
    items.push_back(item);
    originalTotal += item->itemTotal();
    currentTotal += item->itemTotal(); // Initially currentTotal and originalTotal are identical
  }

  double getOriginalTotal() { return originalTotal; }
  double getCurrentTotal() { return currentTotal; }

  // Apply absolute discount deductions on current running total
  void applyDiscount(double d) {
    currentTotal -= d;
    if (currentTotal < 0) {
      currentTotal = 0; // Total cart price cannot drop below zero
    }
  }

  void setLoyaltyMember(bool member) { loyaltyMember = member; }
  bool isLoyaltyMember() { return loyaltyMember; }

  void setPaymentBank(string bank) { paymentBank = bank; }
  string getPaymentBank() { return paymentBank; }

  vector<CartItem *> getItems() { return items; }
};

// ----------------------------------------------------------------------------
// [DESIGN PATTERN: Chain of Responsibility Pattern (CoR)]
// ----------------------------------------------------------------------------
// Chain of Responsibility Pattern ka use discount validation aur sequencing me kiya gaya hai.
// - Coupon base class ek node ki tarah act karti hai jisme `next` pointer sequential link banata hai.
// - `applyDiscount` method chain ko automatic sequence me iterate karwata hai.
// - Har coupon object dynamic criteria validation check karta hai.
// - `isCombinable()` mechanism exclusive coupons provide karta hai; agar koi coupon application ke
//   baad combinable nahi hai (returns false), to discount traversal pipeline wahin stop ho jata hai.
// ----------------------------------------------------------------------------

// Coupon Base class (Handler interface for the Chain)
class Coupon {
private:
  Coupon *next; // Pointer to the next coupon in the chain

public:
  Coupon() { next = nullptr; }

  // Recursive Destructor: Chain me clear deletion sequence enforce karne ke liye
  virtual ~Coupon() {
    if (next) {
      delete next; // Har node agle standard nodes ko recursive clean karta hai
    }
  }

  void setNext(Coupon *nxt) { next = nxt; }
  Coupon *getNext() { return next; }

  // CoR traversal processor logic
  void applyDiscount(Cart *cart) {
    // 1. eligibility criteria resolve karo
    if (isApplicable(cart)) {
      double discount = getDiscount(cart);
      cart->applyDiscount(discount);
      cout << name() << " applied: " << discount << " Rs" << endl;

      // Exclusive coupon handling check
      if (!isCombinable()) {
        cout << "[CoR] Exclusive coupon applied (" << name() << "). Terminating discount chain execution." << endl;
        return; // Next nodes execution logic cut/blocked
      }
    }

    // 2. Delegation to the next link in sequence
    if (next) {
      next->applyDiscount(cart);
    }
  }

  // Virtual hooks defined for concrete coupons implementation
  virtual bool isApplicable(Cart *cart) = 0;
  virtual double getDiscount(Cart *cart) = 0;
  virtual bool isCombinable() { return true; } // Combinable default set as true
  virtual string name() = 0;
};

// ----------------------------------------------------------------------------
// Concrete Coupon implementations (Chain nodes)
// ----------------------------------------------------------------------------

// 1. Seasonal Offer Coupon (Applies to a specific product category)
class SeasonalOffer : public Coupon {
private:
  double percent;
  string category;
  DiscountStrategy *strat; // Strategy composition

public:
  SeasonalOffer(double pct, string cat) {
    percent = pct;
    category = cat;
    // Strategy instance retrieved from Singleton Factory
    strat = DiscountStrategyManager::getInstance()->getStrategy(
        StrategyType::PERCENT, percent);
  }

  ~SeasonalOffer() { delete strat; }

  // Applicable only if the cart contains at least one item matching the target category
  bool isApplicable(Cart *cart) override {
    for (CartItem *item : cart->getItems()) {
      if (item->getProduct()->getCategory() == category) {
        return true;
      }
    }
    return false;
  }

  // Computes percentage discount over the subtotal of matching category items only
  double getDiscount(Cart *cart) override {
    double subtotal = 0.0;
    for (CartItem *item : cart->getItems()) {
      if (item->getProduct()->getCategory() == category) {
        subtotal += item->itemTotal();
      }
    }
    return strat->calculate(subtotal);
  }

  bool isCombinable() override { return true; }

  string name() override {
    return "Seasonal Offer " + to_string((int)percent) + "% off on " + category;
  }
};

// 2. Loyalty Coupon (Applies to loyalty premium club members)
class LoyaltyDiscount : public Coupon {
private:
  double percent;
  DiscountStrategy *strat;

public:
  LoyaltyDiscount(double pct) {
    percent = pct;
    strat = DiscountStrategyManager::getInstance()->getStrategy(
        StrategyType::PERCENT, percent);
  }

  ~LoyaltyDiscount() { delete strat; }

  bool isApplicable(Cart *cart) override { return cart->isLoyaltyMember(); }

  // Runs on running current total (compounding discount application support)
  double getDiscount(Cart *cart) override {
    return strat->calculate(cart->getCurrentTotal());
  }

  string name() override {
    return "Loyalty Discount " + to_string((int)percent) + "% off";
  }
};

// 3. Bulk Purchase Coupon (Applies a flat discount when total crosses threshold limit)
class BulkPurchaseDiscount : public Coupon {
private:
  double threshold;
  double flatOff;
  DiscountStrategy *strat;

public:
  BulkPurchaseDiscount(double thr, double off) {
    threshold = thr;
    flatOff = off;
    strat = DiscountStrategyManager::getInstance()->getStrategy(
        StrategyType::FLAT, flatOff);
  }

  ~BulkPurchaseDiscount() { delete strat; }

  // Uses originalTotal so that discount thresholds are validated on original spend value
  bool isApplicable(Cart *cart) override {
    return cart->getOriginalTotal() >= threshold;
  }

  double getDiscount(Cart *cart) override {
    return strat->calculate(cart->getCurrentTotal());
  }

  string name() override {
    return "Bulk Purchase Rs " + to_string((int)flatOff) + " off over " +
           to_string((int)threshold);
  }
};

// 4. Banking Gateway Coupon (Applies discount with a capped upper limit based on selected Bank)
class BankingCoupon : public Coupon {
private:
  string bank;
  double minSpend;
  double percent;
  double offCap;
  DiscountStrategy *strat;

public:
  BankingCoupon(const string &b, double ms, double percent, double offCap) {
    bank = b;
    minSpend = ms;
    this->percent = percent;
    this->offCap = offCap;
    // Uses Capped Percentage strategy pattern setup
    strat = DiscountStrategyManager::getInstance()->getStrategy(
        StrategyType::PERCENT_WITH_CAP, percent, offCap);
  }

  ~BankingCoupon() { delete strat; }

  bool isApplicable(Cart *cart) override {
    return cart->getPaymentBank() == bank &&
           cart->getOriginalTotal() >= minSpend;
  }

  double getDiscount(Cart *cart) override {
    return strat->calculate(cart->getCurrentTotal());
  }

  string name() override {
    return bank + " Bank " + to_string((int)percent) + "% off (max Rs " +
           to_string((int)offCap) + ")";
  }
};

// ----------------------------------------------------------------------------
// [DESIGN PATTERN: Singleton Pattern]
// ----------------------------------------------------------------------------
// CouponManager application layer wrapper class hai jo rules registration list,
// applicable coupons audit reporting aur process execution order coordinate karti hai.
// Singleton implementation updates isko dynamic memory protection provide karti hai.
// ----------------------------------------------------------------------------

class CouponManager {
private:
  static CouponManager *instance; // Static singleton instance
  static mutex instanceMtx;       // Singleton instantiation lock
  
  Coupon *head;                   // Head of the Coupon Chain (CoR)
  mutable mutex mtx;              // Mutex for safe multi-threaded chain register/read operations

  // Private constructor
  CouponManager() { head = nullptr; }

public:
  // Singleton instance accessor (Double-Checked Locking)
  static CouponManager *getInstance() {
    if (!instance) {
      lock_guard<mutex> lock(instanceMtx);
      if (!instance) {
        instance = new CouponManager();
      }
    }
    return instance;
  }

  // Clean-up handler: Deletes the coupon chain dynamically to avoid memory leaks at shutdown
  ~CouponManager() {
    lock_guard<mutex> lock(mtx);
    if (head) {
      delete head; // Triggers recursive deletion chain of Coupon destructor
      head = nullptr;
    }
  }

  // Register a coupon at the end of the chain
  void registerCoupon(Coupon *coupon) {
    lock_guard<mutex> lock(mtx);
    if (!head) {
      head = coupon;
    } else {
      Coupon *cur = head;
      while (cur->getNext()) {
        cur = cur->getNext();
      }
      cur->setNext(coupon);
    }
  }

  // Retrieve list of currently applicable coupon names (Dry run evaluation)
  vector<string> getApplicable(Cart *cart) const {
    lock_guard<mutex> lock(mtx);
    vector<string> res;
    Coupon *cur = head;
    while (cur) {
      if (cur->isApplicable(cart)) {
        res.push_back(cur->name());
      }
      cur = cur->getNext();
    }
    return res;
  }

  // Sequentially apply all applicable coupons in the chain to the cart
  double applyAll(Cart *cart) {
    lock_guard<mutex> lock(mtx);
    if (head) {
      head->applyDiscount(cart);
    }
    return cart->getCurrentTotal();
  }
};

// Static initializations
CouponManager *CouponManager::instance = nullptr;
mutex CouponManager::instanceMtx;

// ----------------------------
// Main: Client application simulation
// ----------------------------
int main() {
  CouponManager *mgr = CouponManager::getInstance();

  // Step 1: Register coupons in specific sequence order (Chain evaluation order is critical in CoR)
  mgr->registerCoupon(new SeasonalOffer(10, "Clothing"));
  mgr->registerCoupon(new LoyaltyDiscount(5));
  mgr->registerCoupon(new BulkPurchaseDiscount(1000, 100));
  mgr->registerCoupon(new BankingCoupon("ABC", 2000, 15, 500));

  // Step 2: Create mock items (products)
  Product *p1 = new Product("Winter Jacket", "Clothing", 1000);
  Product *p2 = new Product("Smartphone", "Electronics", 20000);
  Product *p3 = new Product("Jeans", "Clothing", 1000);
  Product *p4 = new Product("Headphones", "Electronics", 2000);

  // Step 3: Populate user cart and customer attributes
  Cart *cart = new Cart();
  cart->addProduct(p1, 1);  // Subtotal clothing = 1000
  cart->addProduct(p2, 1);  // Subtotal electronics = 20000
  cart->addProduct(p3, 2);  // Subtotal clothing = 2000 (total clothing = 3000)
  cart->addProduct(p4, 1);  // Subtotal electronics = 2000 (total electronics = 22000)
  cart->setLoyaltyMember(true);
  cart->setPaymentBank("ABC");

  cout << "Original Cart Total: " << cart->getOriginalTotal() << " Rs" << endl;
  cout << "------------------------------------------" << endl;

  // Step 4: Preview eligible coupons
  vector<string> applicable = mgr->getApplicable(cart);
  cout << "Applicable Coupons (Preview):" << endl;
  for (string name : applicable) {
    cout << " - " << name << endl;
  }
  cout << "------------------------------------------" << endl;

  // Step 5: Execute pipeline application of discount coupon sequence
  double finalTotal = mgr->applyAll(cart);
  cout << "------------------------------------------" << endl;
  cout << "Final Cart Total after discounts: " << finalTotal << " Rs" << endl;

  // Cleanup dynamic allocations to prevent memory leaks
  delete p1;
  delete p2;
  delete p3;
  delete p4;
  delete cart;

  // Note: Coupon chain and managers are intentionally not deleted here 
  // since they are process-lifetime singletons.
  return 0;
}

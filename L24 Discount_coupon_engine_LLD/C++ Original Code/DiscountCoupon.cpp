// ============================================================================
//  DiscountCoupon.cpp  —  DISCOUNT COUPON ENGINE (single-file MONOLITH version)
// ----------------------------------------------------------------------------
//  Poora coupon engine EK file me — original lecture code. Isi ko refactor
//  karke MODULAR version banaya gaya hai (../coupons, ../strategies,
//  ../factories, ../core, ../models + ../main.cpp). Patterns ki detail:
//  ../design_patterns.md padho!
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  COUPON CHAIN KA FLOW (Chain of Responsibility):                        │
//  │                                                                          │
//  │   Cart ──> [Seasonal] ──> [Loyalty] ──> [Bulk] ──> [Banking-EXCLUSIVE]  │
//  │             lagta hai?     lagta hai?    lagta?      lagta hai?         │
//  │             haan->apply    nahi->skip    haan->apply haan->apply+STOP🛑 │
//  │             phir NEXT      phir NEXT     phir NEXT   (isCombinable=false│
//  │                                                       -> chain band)    │
//  │                                                                          │
//  │  Har coupon apna MATH khud nahi karta — STRATEGY se karwata hai         │
//  │  (flat/percent/percent-with-cap), jo SINGLETON FACTORY banati hai.      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  IS FILE KA LAYOUT (upar se neeche):
//    1. StrategyType enum + 3 discount strategies     <- STRATEGY pattern
//    2. DiscountStrategyManager                       <- SINGLETON(DCL) + FACTORY
//    3. Product / CartItem / Cart models
//    4. Coupon base + 4 concrete coupons              <- CHAIN OF RESPONSIBILITY
//    5. CouponManager                                 <- SINGLETON(DCL) + registry
//    6. main() — demo
//
//  ⭐ MONOLITH vs MODULAR ka EK BADA FARQ (interview gold!):
//  Is file ke Singletons DOUBLE-CHECKED LOCKING (DCL) style ke hain —
//  static pointer + mutex + do baar null-check. Modular version me MEYERS
//  style hai (getInstance me static local). Dono thread-safe hain, par:
//    DCL    = manual lock + checks — purana classic, likhna tricky
//    Meyers = C++11 se compiler khud guarantee deta hai — chhota aur safe
//  Dono styles jaanna aur farq bata paana = full marks! (detail neeche
//  DiscountStrategyManager ke comments me)
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

// ============================================================================
//  SECTION 1: STRATEGY PATTERN — discount ka MATH (L8 wala pattern, live!)
// ----------------------------------------------------------------------------
//  Discount ka FORMULA aur coupon ki ELIGIBILITY alag-alag cheezein hain:
//    - COUPON decide karta hai: "main is cart pe lagta hoon ya nahi?"
//    - STRATEGY decide karti hai: "kitna discount banega?" (sirf math)
//
//  Teen formulas ka scoreboard:
//   Strategy           | Formula                    | Safety/Twist
//   -------------------+----------------------------+------------------------
//   FLAT               | min(flat, base)            | total negative nahi hoga
//   PERCENT            | (pct/100) × base           | plain — koi twist nahi
//   PERCENT_WITH_CAP   | min((pct/100) × base, cap) | bank ka nuksan capped
//
//  REUSE ka proof: SeasonalOffer aur LoyaltyDiscount DONO PERCENT strategy
//  use karte hain — formula EK jagah, use DO jagah. Naya math aaye (jaise
//  "buy 1 get 1") to bas nayi strategy class — coupons untouched! (OCP ✅)
// ============================================================================

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

// ============================================================================
//  SECTION 2: SINGLETON (DCL style) + SIMPLE FACTORY — 2 patterns ek class me
// ----------------------------------------------------------------------------
//  FACTORY wala hissa: getStrategy(type, params) — enum do, sahi concrete
//  strategy object lo. Coupons ko FlatDiscountStrategy jaise naam pata hi
//  nahi (loose coupling) — L9 wala Simple Factory.
//
//  SINGLETON wala hissa — DOUBLE-CHECKED LOCKING (DCL), dhyan se samjho:
//
//    if (!instance) {                    // CHECK #1 — BINA lock ke
//        lock_guard<mutex> lock(mtx);    // ab lock lo
//        if (!instance) {                // CHECK #2 — lock ke ANDAR phir se!
//            instance = new ...;
//        }
//    }
//
//  DO checks KYUN?! (interview ka pakka sawal):
//    CHECK #1 (lock-free): 99.9% calls me instance BANA HUA hota hai —
//      unke liye mutex lock karna mehnga waste hai. Pehla check unhe
//      bina lock ke fast return de deta hai.
//    CHECK #2 (lock ke andar): Socho DO threads ek saath check #1 pe
//      pahunch gaye (dono ko null dikha). Dono lock ki line me lag gaye.
//      Pehla thread instance bana ke nikla... ab dusra lock paayega —
//      agar wo phir se check NA kare to DUSRA instance ban jayega!
//      Check #2 yahi race condition rokta hai.
//
//  Saath me: private ctor + private dtor + DELETED copy AND move
//  (constructor + assignment dono) — instance banane/copy/move karne ke
//  SAARE piche ke raste band. Sirf getInstance() hi darwaza hai.
//
//  NOTE: Modular version (../factories) me yahi kaam MEYERS singleton se
//  hua hai — 3 lines me! DCL classic hai, Meyers modern. Dono jaano.
// ============================================================================

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

// ============================================================================
//  SECTION 3: MODELS — Product / CartItem / Cart
// ----------------------------------------------------------------------------
//  Cart ka SABSE SMART hissa — DO totals (interview me bolne layak!):
//    originalTotal = pre-discount total, KABHI nahi badalta.
//                    Threshold checks (bulk min 1000, bank min 2000) ISI pe
//                    hote hain — taaki pehle lage discounts kisi coupon ko
//                    unfairly disqualify na kar dein!
//    currentTotal  = running total — har coupon ke discount se ghat-ta hai.
//                    Agla coupon ka % ISI pe lagta hai (stacking behavior).
//  Eligibility metadata bhi Cart me: loyaltyMember flag + paymentBank.
// ============================================================================
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

// ============================================================================
//  SECTION 4: CHAIN OF RESPONSIBILITY — coupons ki chain (system ka dil ❤️)
// ----------------------------------------------------------------------------
//  CoR = "request (Cart) ko handlers (Coupons) ki chain se guzaro — har
//  handler khud decide kare: apply karu? skip karu? ya poori chain rok du?"
//
//  Har coupon ka applyDiscount() flow (base class me EK jagah likha hai):
//    1. isApplicable(cart)?  -> nahi: SKIP, seedha next ko de do
//    2. haan: getDiscount() (strategy se math) -> cart pe lagao
//    3. isCombinable()?      -> false: chain YAHIN STOP 🛑 (exclusive!)
//    4. warna: next->applyDiscount(cart) — discounts STACK hote hain
//
//  Concrete coupon ko sirf 3 cheezein deni hain: isApplicable / getDiscount
//  / name — chain chalane ka saara logic base me hai (Template Method
//  flavour). Naya coupon = nayi class + register — PURANA CODE ZERO EDIT!
//
//  RECURSIVE CLEANUP ka trick: ~Coupon() apne next ko delete karta hai ->
//  sirf HEAD delete karo, poori chain domino ki tarah saaf! 🁢🁢🁢
//
//  Chaaro coupons ka naksha:
//   Coupon               | Level    | Strategy         | Extra rule
//   ---------------------+----------+------------------+----------------------
//   SeasonalOffer        | CATEGORY | PERCENT          | sirf matching category
//   LoyaltyDiscount      | CART     | PERCENT          | loyalty member ho
//   BulkPurchaseDiscount | CART     | FLAT             | originalTotal >= min
//   BankingCoupon        | CART     | PERCENT_WITH_CAP | bank+minSpend, EXCLUSIVE
// ============================================================================

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

  // EXCLUSIVE coupon: base ka default (true) override karke false —
  // ye lagne ke baad chain STOP! Ab guarantee STRUCTURE se aati hai,
  // chain me position se nahi (modular version jaisa sahi design).
  bool isCombinable() override { return false; }

  string name() override {
    return bank + " Bank " + to_string((int)percent) + "% off (max Rs " +
           to_string((int)offCap) + ")";
  }
};

// ============================================================================
//  SECTION 5: COUPON MANAGER — SINGLETON (DCL) + chain registry
// ----------------------------------------------------------------------------
//  System ka orchestrator — teen kaam:
//    registerCoupon() -> chain ke TAIL pe naya coupon jodo
//                        (REGISTRATION ORDER = APPLICATION ORDER! Isi liye
//                         main me exclusive BankingCoupon LAST register hota
//                         hai — pehle hota to baaki ko mauka na milta)
//    getApplicable()  -> DRY-RUN: kaunse coupons lagenge, BINA lagaye batao
//    applyAll()       -> head se poori chain chalao, final total do
//
//  Singleton wahi DCL style (upar DiscountStrategyManager me detail).
//  DO mutex hain — dhyan do, alag-alag kaam ke liye:
//    instanceMtx (static) -> singleton banate waqt ka lock
//    mtx (member)         -> chain register/read operations ka lock
//  `mutable mutex mtx` — taaki const method (getApplicable) me bhi lock ho.
// ============================================================================

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

// ============================================================================
//  SECTION 6: main() — pura engine action me
// ----------------------------------------------------------------------------
//  Flow: coupons register (order matters!) -> cart banao -> dry-run preview
//  (getApplicable) -> applyAll -> final total.
//  Cart: 3000 Clothing + 22000 Electronics = 25000, loyalty + ABC bank —
//  matlab CHAARO coupons lagenge, aur Banking (EXCLUSIVE) pe chain rukegi:
//    Seasonal: 3000 ka 10% = 300   -> current 24700
//    Loyalty:  24700 ka 5% = 1235  -> current 23465
//    Bulk:     flat 100            -> current 23365
//    Banking:  15% = 3504.75 par CAP 500 -> current 22865 + CHAIN STOP 🛑
//  (Output me "[CoR] Exclusive coupon applied... Terminating" bhi dikhega)
// ============================================================================
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

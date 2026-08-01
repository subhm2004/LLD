# Ecommerce Cart & Checkout — Design Patterns & Design Decisions

Ek e-commerce ka **cart → checkout → payment → order** flow. Isme sirf patterns hi nahi,
kuch **real-world reliability problems** bhi solve kiye gaye hain (idempotency, stock
reservation, rollback) — interview me asli marks yahi dilate hain.

---

## 📁 Folder structure

```
Ecommerce_Cart_Checkout_LLD/
├── enums/          OrderStatus, PaymentMethod, PaymentStatus
├── interfaces/     IDiscountStrategy
├── strategies/     IPaymentStrategy + Card/Upi/Cod  |  Flat/Percentage discount
├── factories/      PaymentStrategyFactory
├── models/         Product, User, Address, CartItem, Cart, Coupon,
│                   OrderLine, PaymentResult, Order
├── services/       Catalog, Inventory, Cart, Coupon, Pricing,
│                   Payment, Order, Notification, Checkout
├── core/           EcommerceCheckoutSystem  (Facade)
└── main.cpp        7-scenario demo
```

---

## 1. Strategy Pattern — **do jagah** use hua hai

### (a) Payment strategies

```
IPaymentStrategy (interface)
   ├── CardPaymentStrategy   → "0000" pe khatam = decline
   ├── UpiPaymentStrategy    → "@" hona chahiye
   └── CodPaymentStrategy    → hamesha success (paisa delivery pe)
```

Har payment method ka **apna validation aur apna behavior** hai. Interface ek hi:
`PaymentResult pay(double amount, const string &instrument)`.

### (b) Discount strategies

```
IDiscountStrategy (interface)
   ├── FlatDiscountStrategy       → ₹100 flat off
   └── PercentageDiscountStrategy → 10% off
```

**Fayda:** naya payment method (Wallet, NetBanking) ya naya discount type (BOGO, tiered)
add karna hai? Ek nayi class banao — **existing code bilkul chhuo mat**.
Ye **Open/Closed Principle** ka seedha example hai.

**Strategy na hoti to** `PaymentService` me ek lamba `if-else` hota jo har naye method
pe badalta — aur har badlaav purane methods ko todne ka risk laata.

---

## 2. Factory Pattern — `PaymentStrategyFactory`

```cpp
static unique_ptr<IPaymentStrategy> create(PaymentMethod method);
```

Enum → concrete strategy. **"Kaunsa object banega"** ka `switch` sirf ISI ek file me hai.

📌 **Factory + Strategy hamesha jodi me chalte hain:** Strategy batata hai *"kaam kaise hoga"*,
Factory batata hai *"kaunsa kaam wala object milega"*. Naya method add karne pe **2 hi
jagah** badalti hai — enum aur ye factory.

---

## 3. Facade Pattern — `EcommerceCheckoutSystem`

Andar **9 services** hain. Bahar wale ko ek bhi ka naam nahi pata:

```cpp
system.addToCart("U1", "P1", 2);
system.checkout("U1", "A1", PaymentMethod::UPI, "shubham@okbank", "SAVE10", "REQ_001");
```

**Facade ka kaam:**
- services ko **own** karna aur aapas me **wire** karna
- cross-cutting validation (user exist karta hai? address **usi** user ka hai?)
- system-level state (`currentDay_`, `orderCounter_`)
- checkout ke baad notification

**Facade ka kaam NAHI:** koi business math. Wo Pricing/Coupon/Payment services me hi hai.
📌 Facade **coordinator** hota hai, **God object** nahi.

---

## 4. Service Layer + SRP — 9 services, har ek ka ek kaam

| Service | Zimmedari | Ye NAHI karta |
|---|---|---|
| `CatalogService` | product details (naam, price, category) | stock |
| `InventoryService` | stock + **reservations** | product details |
| `CartService` | cart rules (add/update/remove) | pricing |
| `CouponService` | coupon registry + eligibility | discount ka math (Strategy karta hai) |
| `PricingService` | final bill ka math | coupon rules |
| `PaymentService` | Factory + Strategy ko jodna | order state |
| `OrderService` | orders ka repository | payment |
| `NotificationService` | user ko batana | kuch bhi aur |
| `CheckoutService` | **orchestration + rollback** | koi apna business rule |

⭐ **Catalog aur Inventory alag kyun?** Product details kabhi-kabhi badalti hain,
stock **har order pe** badalta hai. Alag rakhne se alag-alag scale/cache/DB de sakte ho.

---

## 5. Repository Pattern — `OrderService`

Bahar se sirf `saveOrder` / `getOrderOrThrow` / `findByClientRequestId` dikhta hai;
andar `unordered_map` hai. Kal ko real DB laga do — **sirf is class ka andar badlega**.

---

## ⭐⭐ 6. Reservation Saga — `reserve → commit / release`

Ye is LLD ka **sabse important** hissa hai.

```
reserveItems()        stock available se hata ke "reserved" me    [payment se PEHLE]
  ├─ payment SUCCESS → commitReservation()   reservation khatam, stock gaya
  └─ payment FAIL    → releaseReservation()  stock WAPAS available me
```

**Reserve payment se pehle kyun?**
Warna do log ek hi last item ke liye payment kar dete — dono ka paisa katta, item ek hi hota. 😱
Reserve pehle karke wo race jeet li jaati hai.

**Release kyun zaroori?**
Payment fail hone pe stock hamesha ke liye **atka** reh jaata — na kisi ko milta, na wapas aata.
Ye "inventory leak" hai.

### ⭐ `releaseReservation` **idempotent** hai — jaan-boojh ke

```cpp
void releaseReservation(const string &reservationId) {
    auto it = reservedStock_.find(reservationId);
    if (it == reservedStock_.end()) return;   // ⭐ pehle se released -> no-op
    ...
}
```

`CheckoutService` ke failure path me release **do baar** call hota hai (ek explicit,
ek `catch(...)` se). Idempotent hone ki wajah se doosri call kuch nahi karti —
**stock double wapas nahi hota**. Ye ek deliberate contract hai, ittefaq nahi.

---

## ⭐⭐ 7. Idempotency — `clientRequestId`

User ka internet atka, usne "Place Order" **do baar** daba diya.

```cpp
if (!clientRequestId.empty()) {
    const Order *existing = orderService_.findByClientRequestId(clientRequestId);
    if (existing != nullptr) return *existing;   // WAHI purana order
}
```

Same id wali doosri request pe **naya order nahi banta, naya payment nahi hota** —
purana order hi laut jaata hai. Bina iske: **double order + double charge**.

📌 Check **do jagah** hai (facade + CheckoutService). Facade wala early-return isliye hai
ki duplicate request pe **email dobara na jaye**.

⚠ Abhi lookup **O(N) linear scan** hai. Production me `clientRequestId` pe **unique index**
chahiye — wo speed ke saath-saath concurrent duplicate bhi rokta hai.

---

## ⭐⭐ 8. Rollback guarantee — `try / catch(...) / throw;`

```cpp
const string reservationId = inventoryService_.reserveItems(userId, requestedItems);
try {
    ... pricing, order banana, payment, commit, cart clear ...
} catch (...) {
    inventoryService_.releaseReservation(reservationId);  // stock kabhi atkega nahi
    throw;                                                // original exception preserve
}
```

Reserve ke baad ka **saara** code `try` me hai. Coupon invalid, product gayab, payment fail,
ya koi bhi anjaan error — stock hamesha wapas aata hai.

📌 `throw;` (bare) same exception ko rethrow karta hai — type aur message preserve.
`throw e;` likhte to **slicing** ho sakti thi.

---

## 9. Order of operations — sequence hi asli design hai

| Kadam | Kyun yahi jagah |
|---|---|
| Idempotency check **sabse pehle** | duplicate pe kuch bhi side-effect na ho |
| Reserve **payment se pehle** | last item ki race jeet lo |
| Order object **payment se pehle** bane | fail hone pe bhi `FAILED` order ka **record** bache |
| `clearCart` **sabse aakhir** | beech me fail ho to user ka cart salaamat rahe |
| Notification **checkout ke baad** | email fail hone se order cancel na ho |

⭐ `clearCart` last me hone ka proof demo me hai: card decline ke baad cart **waise ka waisa**
dikhta hai — user dobara try kar sakta hai.

---

## 10. Snapshot pattern — `OrderLine` aur stored price breakup

```cpp
class OrderLine {           // CartItem ka SNAPSHOT
    string productId;
    string productName;     // ⭐ order ke waqt ka naam (COPY)
    double unitPrice;       // ⭐ order ke waqt ka daam (COPY)
    int    quantity;
};
```

Order ban jaane ke baad catalog me price/naam badle to **purani invoice badalni nahi chahiye**.
Isi soch se `Order` me `subtotal / discount / shippingFee / payableAmount` **saare store**
hote hain — recompute nahi hote. Historical record = immutable.

---

## 11. Result Object — `PaymentResult` (exception ke bajaye)

```cpp
class PaymentResult { PaymentStatus status_; string transactionId_; string message_; };
```

Card decline hona **normal business case** hai, system fault nahi. Isliye exception ke bajaye
result object: caller ko **status + txnId + exact reason** teeno milte hain.

⭐ Default `status_{FAILED}` — **fail closed**. Galti se status set na ho to system "fail"
maanega, "success" nahi (paise wale code me ye bahut zaroori hai).

---

## 12. Do alag status enums — `OrderStatus` + `PaymentStatus`

```
OrderStatus  : CREATED → PAYMENT_PENDING → CONFIRMED / FAILED
PaymentStatus: PENDING / SUCCESS / FAILED
```

**Ek hi enum kyun nahi?** COD dekho: order **CONFIRMED** hai (bhej rahe hain)
par payment **PENDING** hai (paisa delivery pe aayega). Ek enum me ye state likhi hi nahi ja sakti.

⭐ `PAYMENT_PENDING` alag state kyun? Kyunki "paisa maanga, jawab nahi aaya" ek **asli**
state hai — server yahin crash ho jaye to reconciliation job ko pata chalta hai ki
gateway se status poochna hai.

---

## 🐛 Bug jo mila aur fix hua

### Self-referential members ke saath implicit **move** — dangling references

**Confirm kiya tha:** ye line pehle **compile ho jaati thi** —

```cpp
EcommerceCheckoutSystem a;
EcommerceCheckoutSystem b = std::move(a);   // ✅ compile 😱
```

**Problem:** `cartService_` andar `CatalogService&` rakhta hai aur `checkoutService_`
**6 sibling members** ke references rakhta hai. Move memberwise hota hai — references
**copy** hoti hain, **re-bind nahi**. Matlab `b.checkoutService_` abhi bhi `a` ke members
ko point karta rehta:

- `b.checkout(...)` → stock **`a`** ka ghata, order **`a`** me save hua
- `a` scope se bahar gaya → `b` ki saari references **dangling** → use-after-free

**Fix** (`core/EcommerceCheckoutSystem.h`):

```cpp
EcommerceCheckoutSystem(const EcommerceCheckoutSystem &)            = delete;
EcommerceCheckoutSystem &operator=(const EcommerceCheckoutSystem &) = delete;
EcommerceCheckoutSystem(EcommerceCheckoutSystem &&)                 = delete;
EcommerceCheckoutSystem &operator=(EcommerceCheckoutSystem &&)      = delete;
```

**Verify:** ab wahi test `error: call to deleted constructor` deta hai — runtime crash ka
mauka hi nahi milta.

📌 **RULE:** jis class ke members ek dusre ko reference/point karte hon, wo class by-default
na copy honi chahiye na move.

---

## ⚠ Doosre important design points

### `min(discount, subtotal)` — negative payable ka guard

Cart ₹800, coupon FLAT1000 → bina clamp ke `payable = -151` (dukaan customer ko paise degi 😱).
Clamp **do jagah** hai (FlatDiscountStrategy + PricingService) — **defense in depth**, kyunki
discount kahin aur se bhi aa sakta hai (loyalty points, cashback).

### Member **declaration order** — yahan critical hai

C++ me members **declaration** ke order me construct hote hain (init-list ke order me nahi).
`checkoutService_` sabse aakhir me declare hai — warna wo un members ka reference leta
jo abhi bane hi nahi. `-Wreorder` warning ko kabhi ignore mat karna.

### `unique_ptr` member ka chain effect

`Coupon` me `unique_ptr<IDiscountStrategy>` hai → `Coupon` **move-only** ban gaya →
isliye `registerCoupon(Coupon coupon)` **by-value + `std::move`** leta hai (`const&` se
copy chahiye hoti = compile error).

### `unordered_map` ka non-deterministic order

`getOrdersForUser` sort karta hai, warna history har run me alag sequence me dikhti
(aur tests flaky ho jaate).

---

## 📊 Pattern summary

| # | Pattern / Technique | Kahan | Fayda |
|---|---|---|---|
| 1 | **Strategy** | payment (3), discount (2) | naya type = nayi class, purana code untouched |
| 2 | **Factory** | `PaymentStrategyFactory` | object creation ek jagah |
| 3 | **Facade** | `EcommerceCheckoutSystem` | 9 services ka ek darwaza |
| 4 | **Service Layer / SRP** | 9 services | ek class = ek kaam |
| 5 | **Repository** | `OrderService` | storage swap karna aasan |
| 6 | **Saga (reserve/commit/release)** | `InventoryService` | stock kabhi leak nahi hota |
| 7 | **Idempotency key** | `clientRequestId` | double order/charge nahi |
| 8 | **Snapshot** | `OrderLine`, stored breakup | purani invoice badalti nahi |
| 9 | **Result Object** | `PaymentResult` | reason + txnId + status ek saath |
| 10 | **Dependency Injection** | har service constructor | testable, loosely coupled |

---

## 🎯 Extend karna ho to

| Feature | Kya badlega |
|---|---|
| Wallet payment | `PaymentMethod` me `WALLET` + `WalletPaymentStrategy` + factory me ek case |
| BOGO discount | nayi `IDiscountStrategy` class — bas |
| Tax / GST | `PricingService::calculate` me ek line + `PriceBreakup` me ek field |
| Multiple coupons | `CouponService` list le, `PricingService` ka clamp already safe hai |
| Real DB | `OrderService`/`CatalogService` ke andar — callers untouched |
| Concurrency | `InventoryService` me mutex (ya DB atomic decrement) |

---

## ✅ Verification

- `compile.sh` → **zero warnings** (`-Wall -Wextra -std=c++17`)
- Demo output baseline se **identical**
- **ASan + UBSan** → clean (exit 0, koi report nahi)
- Move-fix verify → `error: call to deleted constructor` ✅
- Saare `#include <bits/stdc++.h>` hata ke **targeted includes** (portable)

# L11 Tomato (Food Delivery) — Design Patterns Used (Hinglish)

> "Tomato" = Zomato/Swiggy ka mini LLD. Is project me **4 design patterns** ek saath kaam karte hain taaki ek user aaram se: restaurant dhundhe → cart bhare → order place kare (abhi ya scheduled, delivery ya pickup) → payment kare (UPI/Card) → notification paaye. Neeche har pattern kahan, kyun, aur kaise use hua — sab detail me.

---

## Ek Nazar Me — Saare Patterns

| # | Pattern | Type | Kahan (file) | Ek line me kaam |
|---|---------|------|--------------|-----------------|
| 1 | **Facade** | Structural | [`TomatoApp.h`](./C++%20Code/Tomato/TomatoApp.h) | Poore system ka ek simple entry-point |
| 2 | **Singleton** | Creational | [`managers/RestaurantManager.h`](./C++%20Code/Tomato/managers/RestaurantManager.h), [`managers/OrderManager.h`](./C++%20Code/Tomato/managers/OrderManager.h) | Restaurant catalog + order book ek-ek hi |
| 3 | **Strategy** | Behavioral | [`strategies/`](./C++%20Code/Tomato/strategies/) | Payment mode (UPI/Card) runtime pe swap |
| 4 | **Factory Method** | Creational | [`factories/`](./C++%20Code/Tomato/factories/) | Now vs Scheduled order banane ka creation encapsulate |

---

## 1. Facade — TomatoApp 🚪

**Kahan:** [`TomatoApp.h`](./C++%20Code/Tomato/TomatoApp.h)

**Problem jo solve hua:** Client (main.cpp) ko agar khud RestaurantManager, OrderManager, factories, NotificationService — sabke saath deal karna padta, to har client me wahi 10-step sequence copy-paste hota aur tight coupling banta.

**Kaise:** TomatoApp ek simple method deta hai har kaam ke liye — `searchRestaurants()`, `addToCart()`, `checkoutNow()`, `payForOrder()`. Andar ka jhamela chhupa hai:

```
main.cpp  ─────>  TomatoApp (Facade)  ─────>  RestaurantManager (Singleton)
(client)                                       OrderManager (Singleton)
   │                                           NowOrderFactory / ScheduledOrderFactory
   │ sirf TomatoApp                            PaymentStrategy (UPI/Card)
   │ dikhta hai                                NotificationService
```

**Asli fayda:** `checkout()` method me 4 cheezein coordinate hoti hain (cart validate → factory se order → manager me register → return) — client ko is order ka gyaan hi nahi chahiye. Bas `checkoutNow()` bolo.

**Note:** Facade complexity ko band nahi karta — zaroorat ho to client seedha manager access kar sakta hai. Ye convenience hai, restriction nahi.

---

## 2. Singleton — RestaurantManager & OrderManager 1️⃣

**Kahan:** [`RestaurantManager.h`](./C++%20Code/Tomato/managers/RestaurantManager.h), [`OrderManager.h`](./C++%20Code/Tomato/managers/OrderManager.h)

**Kyun:**
- **RestaurantManager:** Restaurant catalog poore app me EK hona chahiye. Do catalogs hote to ek me restaurant register, dusre me search "not found" = chaos!
- **OrderManager:** Saare orders ka ek hi tracker/order-book.

**Interview-worthy detail — do alag styles ek hi project me:**

| Manager | Style | Thread-safe? |
|---------|-------|--------------|
| `RestaurantManager` | **Double-Checked Locking (DCL)** — do null-check + mutex | ✅ Haan |
| `OrderManager` | **Simple lazy** — bas null-check, koi lock | ❌ Nahi (race possible) |

Ye consistency issue hai — dono ko **Meyers style** (`static local`) karna best hota (RestaurantManager me commented block me wahi likha hai). L10 me in styles ka pura comparison hai.

---

## 3. Strategy — PaymentStrategy 💳

**Kahan:** [`PaymentStrategy.h`](./C++%20Code/Tomato/strategies/PaymentStrategy.h) (interface) + `UpiPaymentStrategy`, `CreditCardPaymentStrategy`

**Problem jo solve hua:** Payment ka TAREEKA badalta rehta hai (UPI/Card/Wallet...), par Order ko har mode ka code apne andar nahi rakhna chahiye — warna naya mode = Order class edit.

**Kaise:** Order sirf `PaymentStrategy*` rakhta hai. `processPayment()` me:

```cpp
paymentStrategy->pay(total);   // Order ko pata nahi UPI hai ya Card!
```

Client decide karta hai kaunsi strategy inject karni hai:

```cpp
tomato->checkoutNow(user, "Delivery", new UpiPaymentStrategy("1234567890"));
```

**Fayda:** Naya payment mode (PayPal, COD) = nayi strategy class. Order, TomatoApp, kuch nahi badalta. OCP ✅

---

## 4. Factory Method — OrderFactory 🏭

**Kahan:** [`OrderFactory.h`](./C++%20Code/Tomato/factories/OrderFactory.h) (interface) + `NowOrderFactory`, `ScheduledOrderFactory`

**Problem jo solve hua:** Order banane me DO-level variation hai:
1. **WHEN:** abhi (`NowOrderFactory`) ya future (`ScheduledOrderFactory`)
2. **HOW:** Delivery ya Pickup (factory ke andar `orderType` se)

Bina factory ke ye sab `if-else` ka jungle banta client me.

**Kaise:** TomatoApp sirf `OrderFactory*` jaanta hai; concrete factory inject hoti hai:

```cpp
checkoutNow()       → checkout(..., new NowOrderFactory())
checkoutScheduled() → checkout(..., new ScheduledOrderFactory(time))
```

Common `checkout()` dono ke liye same — sirf factory alag. Yahi Factory Method ka power: `createOrder()` ek interface, char combinations (Now/Scheduled × Delivery/Pickup) bina if-else ke.

**Fayda:** Naya order-timing (jaise "recurring weekly") = nayi factory class, baaki untouched.

---

## Patterns Ek Saath Kaise Kaam Karte Hain (The Big Picture)

```
main.cpp (client)
   │ sab kuch TomatoApp ke through
   ▼
TomatoApp ───────────────── FACADE (poora orchestration chhupata hai)
   │
   ├── searchRestaurants() ──> RestaurantManager ── SINGLETON
   ├── checkoutNow()       ──> NowOrderFactory    ── FACTORY METHOD
   │        │                   creates Order (Delivery/Pickup)
   │        └──> OrderManager ── SINGLETON (order register)
   └── payForOrder()       ──> order.processPayment()
            │                   └──> PaymentStrategy.pay() ── STRATEGY
            └──> NotificationService (receipt)
```

**Flow ek line me:** Client → Facade → (Singleton managers + Factory-made orders + Strategy-based payment) → notification.

---

## Design Decisions & Bugs (interview me bolne layak) ⭐

1. **Facade ka checkout() = orchestration:** cart validate → factory → manager → return. Client ko sequence ka gyaan nahi chahiye — yahi Facade ka core.

2. **🐛 BUG jo is project me tha aur FIX kiya:** `ScheduledOrderFactory` ke pickup branch me `order = pickupOrder;` **missing** tha — order nullptr reh jaata aur `order->setUser()` pe **crash** (segfault) hota. NowOrderFactory me sahi tha, copy karte waqt chhoot gaya tha. Ab fix hai (comment ke saath).

3. **Do Singleton styles ka mismatch:** RestaurantManager DCL, OrderManager simple lazy — consistency ke liye dono Meyers hone chahiye.

4. **Strategy runtime injection:** payment mode client choose karta hai, Order execute karta hai — clean separation.

5. **Ownership:** User apne Cart ka malik (dtor me delete), Order apni PaymentStrategy ka malik. Raw pointers hain — production me `unique_ptr` better.

---

## Kya Improve Ho Sakta Hai (practice ideas)

- Raw pointers → `unique_ptr`/`shared_ptr` (User, Order, strategies, restaurants) — leaks khatam
- Dono managers ko **Meyers Singleton** — consistency + thread-safety
- `NotificationService` ko **Observer** ya **Strategy** banao (SMS/email/push channels — L14 jaisa)
- `checkoutScheduled` ke liye time validation (past time reject)
- Payment failure handling (abhi hamesha success maanta hai)

---

**Related lessons is repo me:** [L8 Strategy](../L8%20Strategy_Design_Patterns/README.md) · [L9 Factory](../L9%20Factory_Design_Pattern/notes/00_INDEX.md) · [L10 Singleton](../L10%20Singleton_Design_Pattern/) · [L17 Facade](../L17%20Facade_Design_Pattern/) · L14 Notification (Observer/Strategy)

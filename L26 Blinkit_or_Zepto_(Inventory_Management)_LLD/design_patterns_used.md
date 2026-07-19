# L26 Blinkit/Zepto (Inventory Management) — Design Patterns Used (Hinglish)

> Quick-commerce (Blinkit/Zepto) ka inventory + order management system. Dark stores, 5km catalog, aur sabse khaas — **order splitting** (ek store me sab na mile to nearby stores me baant do). **4 design patterns + ek state machine** ek saath. Neeche har ka role detail me.

---

## Ek Nazar Me — Saare Patterns

| # | Pattern | Type | Role | Kahan |
|---|---------|------|------|-------|
| 1 | **Strategy** | Behavioral | Replenishment + storage backend swap-able | [`inventory/ReplenishStrategy.h`](./inventory/ReplenishStrategy.h), [`inventory/Inventory.h`](./inventory/Inventory.h) |
| 2 | **Factory** | Creational | Product + storage backend banana | [`models/Product.h`](./models/Product.h), [`inventory/Inventory.h`](./inventory/Inventory.h) |
| 3 | **Facade** | Structural | BlinkitSystem managers ko coordinate | [`core/BlinkitSystem.h`](./core/BlinkitSystem.h) |
| 4 | **Singleton** | Creational | DarkStoreManager + OrderManager ek-ek | [`core/BlinkitSystem.h`](./core/BlinkitSystem.h) |
| + | **State Machine** | — | OrderStatus transitions (valid flow enforce) | [`core/BlinkitSystem.h`](./core/BlinkitSystem.h) |

---

## Domain Samajh Lo Pehle

- **Dark Store** = chhota local warehouse (customer-facing nahi, sirf delivery ke liye). Blinkit ke paas har area me ek. Har store ki apni location + inventory.
- **5km radius** = user ko sirf 5km ke andar wale stores se hi order mil sakta (quick delivery ke liye).
- **Order Splitting** = agar ek store poora cart na de sake, to nearby stores me baant ke alag-alag deliveries.

---

## 1. Strategy — Do jagah 🔀

### A. Replenishment Strategy ([`inventory/ReplenishStrategy.h`](./inventory/ReplenishStrategy.h))
"Stock kab/kaise refill ho?" — har dark store apni policy:
- `ThresholdReplenishStrategy`: stock threshold se neeche gira to refill (reactive)
- `WeeklyReplenishStrategy`: har week fixed refill (scheduled)

Real quick-commerce me critical — smart stock management se "out of stock" aur "overstock" dono avoid. Naya tareeka (ML-based prediction) = nayi strategy class.

### B. Storage Backend Strategy ([`inventory/Inventory.h`](./inventory/Inventory.h))
`InventoryStore` interface, do concrete: `InMemoryInventoryStore` (RAM) aur `DbInventoryStore` (DB sim). Har store choose kar sakta konsa backend. `InventoryManager` ko farq nahi ki data RAM me hai ya DB me — **Dependency Inversion** (abstract `InventoryStore*` pe depend, concrete pe nahi).

---

## 2. Factory — Do jagah 🏭

### A. ProductFactory ([`models/Product.h`](./models/Product.h))
`createProduct(sku)` se SKU ka sahi product (naam/price). SKU→details mapping EK jagah — Inventory, Cart, OrderManager sab isse product banate hain, koi duplicate mapping nahi. Naya product = ek line yahan.

### B. InventoryStoreFactory ([`inventory/Inventory.h`](./inventory/Inventory.h))
`InventoryStoreType` se sahi storage backend (InMemory/Db) banata. Dark store ko concrete store class ka naam nahi pata.

---

## 3. Facade — BlinkitSystem 🚪

**Kahan:** [`core/BlinkitSystem.h`](./core/BlinkitSystem.h)

Client (main.cpp) sirf simple calls karta:
- `DarkStoreManager::showCatalogWithinRadius()` — catalog dekho
- `OrderManager::placeOrder()` — order place karo
- `OrderManager::updateOrderStatus()` — status aage badhao

Andar ka jhamela (nearby stores dhundhna, distance calculate, order splitting, delivery fee, state validation) sab chhupa. Client ko algorithm ka gyaan nahi chahiye.

---

## 4. Singleton — Central managers 1️⃣

`DarkStoreManager` (saare stores ka registry) aur `OrderManager` (saare orders) — dono singletons (Meyers style: `static local instance`, thread-safe). Poore app me ek-ek — do store-registries ya do order-managers = chaos.

---

## 5. State Machine — Order lifecycle ⚙️

**OrderStatus transitions** ([`isValidTransition()`](./core/BlinkitSystem.h)):

```
PLACED ──> CONFIRMED ──> PACKING ──> OUT_FOR_DELIVERY ──> DELIVERED
   │           │            │              │
   └───────────┴────────────┴──────────────┴──> CANCELLED (kabhi bhi)

DELIVERED / CANCELLED = terminal (aage kuch nahi)
```

Har state se sirf VALID agla state allowed. "PLACED → DELIVERED" jaisa galat jump (packing/delivery skip) **exception** phenkta hai. Real order tracking systems aise hi state machines use karte — invalid states unrepresentable.

---

## ⭐ Order Splitting Algorithm (system ka star feature)

```
User cart: SKU101 x4, SKU102 x3, SKU103 x2

1. Nearby stores (5km, distance-sorted): StoreA (closest), StoreC, StoreB

2. FAST PATH: StoreA poora cart de sakta hai?
   StoreA: 101 x5 ✓, 102 x2 (need 3) ✗  -> NAHI, split karna padega

3. SPLIT PATH (greedy, closest se):
   StoreA -> 101 x4 (poora) + 102 x2 (jitna hai)  [Partner1]
   remaining: 102 x1, 103 x2
   StoreC -> 102 x1 (poora)                        [Partner2]
   remaining: 103 x2
   StoreB -> 103 x2 (poora)                        [Partner3]
   remaining: empty ✓ — fully fulfilled!

Result: 1 cart -> 3 stores -> 3 delivery partners
```

Bilkul asli Blinkit jaisa — jab ek store me sab available nahi, paas ke stores se mangwa ke multiple deliveries!

---

## Design Highlights & Limitations (interview me bolne layak) ⭐

1. **Distance-sorted greedy splitting:** closest store pehle (kam delivery cost/time). Simple greedy — har store se jitna mil sake lo.

2. **Surge pricing:** delivery fee me distance > 3km pe 1.5x surge multiplier (peak-time pricing jaisa).

3. **State machine se invalid transitions rok:** order status galat order me nahi badal sakta — data integrity.

4. **🐛 LIMITATION (bug-ish):** `placeOrder` pehle stock DEDUCT karta hai (removeStock). Agar order aage fail ho (payment) to **rollback nahi hota** — deducted stock leak ho jaata! Real system me "reserve → confirm → commit" pattern chahiye (ya L39 Memento se rollback). Ye conscious limitation hai, interview me bolna good sign.

5. **DIP via InventoryStore:** business logic (InventoryManager) storage backend se decoupled — RAM/DB swap without code change.

---

## Files Structure

```
L26 Blinkit_or_Zepto_(Inventory_Management)_LLD/
├── design_patterns_used.md         ← Ye file
├── main.cpp                        ← demo (setup stores, order, split)
├── models/
│   ├── Product.h                   ← Product + ProductFactory (Factory)
│   └── UserCart.h                  ← Cart + User models
├── inventory/
│   ├── Inventory.h                 ← InventoryStore (Strategy) + Factory + Manager
│   └── ReplenishStrategy.h         ← ReplenishStrategy (Strategy)
└── core/
    └── BlinkitSystem.h             ← Facade + Singletons + splitting + state machine
```

*(C++ Code/ZeptoClone.cpp bhi hai — same system ka alternate single-file version.)*

---

## Kya Improve Ho Sakta Hai (practice ideas)

- **Stock reservation** (biggest one): deduct karne se pehle reserve karo, confirm pe commit, fail pe release — rollback support
- Raw pointers → smart pointers
- Observer pattern for order status (user ko notification bheje status change pe)
- Better splitting (minimize number of deliveries — abhi greedy hai, optimal nahi)
- Payment integration (abhi order place = done, payment nahi)

---

**Related lessons:** [L8 Strategy](../L8%20Strategy_Design_Patterns/README.md) · [L9 Factory](../L9%20Factory_Design_Pattern/notes/00_INDEX.md) · [L10 Singleton](../L10%20Singleton_Design_Pattern/) · [L17 Facade](../L17%20Facade_Design_Pattern/) · [L39 Memento](../L39%20Memento_design_pattern/) (rollback ke liye)

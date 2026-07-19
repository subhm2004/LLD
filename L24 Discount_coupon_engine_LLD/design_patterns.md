# L24 Discount Coupon Engine — Design Patterns Used (Hinglish)

> Is project me **4 design patterns** use hue hain, aur sab milke ek kaam karte hain: *"Cart pe multiple coupons apply karo — sahi order me, sahi rules se, bina kisi class ko baar-baar chhede."* Neeche har pattern ka kya role hai, kahan hai, aur kyun chuna gaya — sab detail me.

---

## Ek Nazar Me — Saare Patterns

| # | Pattern | Type | Kahan (file) | Ek line me kaam |
|---|---------|------|--------------|-----------------|
| 1 | **Chain of Responsibility** | Behavioral | [`coupons/Coupon.h`](./coupons/Coupon.h) | Coupons ki linked chain — har coupon apply/skip/stop khud decide kare |
| 2 | **Strategy** | Behavioral | [`strategies/*.h`](./strategies/) | Discount ka MATH alag classes me (flat / percent / percent-with-cap) |
| 3 | **Factory (Simple)** | Creational | [`factories/DiscountStrategyManager.h`](./factories/DiscountStrategyManager.h) | `StrategyType` enum se sahi strategy object banana |
| 4 | **Singleton** | Creational | [`core/CouponManager.h`](./core/CouponManager.h) + [`factories/DiscountStrategyManager.h`](./factories/DiscountStrategyManager.h) | Dono managers ka poore program me EK hi instance |

**Bonus flavour:** `Coupon::applyDiscount()` me **Template Method** jaisa structure bhi hai — chain chalane ka skeleton base class me fixed hai, sirf rules (isApplicable/getDiscount) subclasses bharti hain.

---

## 1. Chain of Responsibility — Coupons ki Chain ⛓️

**Kahan:** [`coupons/Coupon.h`](./coupons/Coupon.h) (base) + 4 concrete coupons ([`SeasonalOffer`](./coupons/SeasonalOffer.h), [`LoyaltyDiscount`](./coupons/LoyaltyDiscount.h), [`BulkPurchaseDiscount`](./coupons/BulkPurchaseDiscount.h), [`BankingCoupon`](./coupons/BankingCoupon.h))

**Problem jo solve hua:** Multiple coupons hain, har ek ke apne eligibility rules hain, kuch stack hote hain kuch exclusive hain. Bina pattern ke ye ek giant if-else banta jisme har coupon ka logic ghusa hota — naya coupon aane pe wahi function baar-baar edit hota.

**Kaise kaam karta hai:**

```
Cart ──> [Seasonal] ──> [Loyalty] ──> [Bulk] ──> [Banking-EXCLUSIVE]
           lagta hai?     nahi?         lagta?      lagta hai?
           apply+NEXT     skip+NEXT     apply+NEXT  apply + CHAIN STOP 🛑
```

- Har `Coupon` ke paas `next` pointer hai — linked chain
- `applyDiscount()` (base me): applicable? → discount lagao → combinable nahi? → **STOP**; warna `next->applyDiscount()`
- **Exclusive coupon trick:** `BankingCoupon::isCombinable()` false return karta hai → chain wahin ruk jaati hai. Isi liye main.cpp me ise **sabse LAST** register kiya hai!
- **Recursive cleanup:** `~Coupon()` apne `next` ko delete karta hai → head delete karo, poori chain saaf (domino effect)

**Naya coupon add karna:** Bas `Coupon` extend karo, 3 methods do (isApplicable/getDiscount/name), register karo — **koi purani file edit nahi hoti!** ✅ OCP

---

## 2. Strategy — Discount ka Math 🧮

**Kahan:** [`strategies/IDiscountStrategy.h`](./strategies/IDiscountStrategy.h) (interface) + 3 concrete: [`FlatDiscountStrategy`](./strategies/FlatDiscountStrategy.h), [`PercentageDiscountStrategy`](./strategies/PercentageDiscountStrategy.h), [`PercentageWithCapStrategy`](./strategies/PercentageWithCapStrategy.h)

**Problem jo solve hua:** Discount ka FORMULA aur coupon ki ELIGIBILITY do alag cheezein hain jo alag speed se badalti hain. Formula coupon ke andar hard-code karte to same percent-math 2 coupons me copy-paste hota.

**Separation of concerns:**
- **Coupon** decide karta hai: *"main lagta hoon ya nahi?"* (eligibility)
- **Strategy** decide karti hai: *"kitna discount banega?"* (math)

**Formulas:**

| Strategy | Formula | Safety |
|----------|---------|--------|
| `FLAT` | `min(flat, base)` | Total negative nahi hone dega |
| `PERCENT` | `(pct/100) × base` | Plain — koi twist nahi |
| `PERCENT_WITH_CAP` | `min((pct/100) × base, cap)` | Bank ka nuksan capped |

**Reuse ka proof:** `SeasonalOffer` (category subtotal pe) aur `LoyaltyDiscount` (cart total pe) — **dono SAME `PERCENT` strategy** use karte hain, sirf base amount alag dete hain. Formula ek jagah, use do jagah! (L8 Strategy pattern ka real-world roop)

---

## 3. Factory (Simple) — Strategy Banane ki Dukaan 🏭

**Kahan:** [`factories/DiscountStrategyManager.h`](./factories/DiscountStrategyManager.h) + [`enums/StrategyType.h`](./enums/StrategyType.h)

**Problem jo solve hua:** Har coupon ko strategy chahiye, par coupons ko concrete class names (`FlatDiscountStrategy`...) nahi pata hone chahiye — warna coupon ↔ strategy tight coupling.

**Kaise:**

```cpp
// Coupon bas enum bolta hai — class ka naam tak nahi jaanta:
strategy = DiscountStrategyManager::getInstance()
               .createStrategy(StrategyType::PERCENT, percent);
```

- `createStrategy(type, param1, param2)` — switch se sahi object banata hai
- **Type-safe enum** (`enum class StrategyType`) — string ki jagah, taaki typo compile-time pe pakda jaye
- L9 wale Simple Factory ka trade-off yahan bhi: naya type = switch me naya case (OCP weak) — par creation **ek jagah centralized** hai

---

## 4. Singleton — Ek Hi Instance, Har Jagah 1️⃣

**Kahan:** DO jagah — [`core/CouponManager.h`](./core/CouponManager.h) aur [`factories/DiscountStrategyManager.h`](./factories/DiscountStrategyManager.h)

**Kyun:**
- **CouponManager:** Coupon registry poore system me EK honi chahiye — do registries hoti to kisi me Seasonal register hota, kisi me Banking → alag-alag jagah alag discounts. Chaos!
- **DiscountStrategyManager:** Factory stateless hai — do instances ka koi matlab hi nahi.

**Implementation details (interview-worthy):**

```cpp
static CouponManager &getInstance() {
    static CouponManager instance;   // Meyers Singleton
    return instance;
}
```

| Detail | Kya karta hai |
|--------|---------------|
| **Meyers style** (static local) | C++11 se thread-safe initialization ki GUARANTEE — bina lock likhe |
| **Private constructor** | Bahar se `CouponManager m;` impossible |
| **Deleted copy ctor + operator=** (DiscountStrategyManager me) | Galti se copy karke "do instances" banne ka rasta band |
| **Mutex** (CouponManager me) | Registry operations thread-safe — `mutable` mutex taaki const methods me bhi lock ho |

---

## Patterns Ek Saath Kaise Kaam Karte Hain (The Big Picture)

```
main.cpp
  │ getInstance()                        ← SINGLETON
  ▼
CouponManager ──registerCoupon()──> Coupon chain banati hai
  │                                      ← CHAIN OF RESPONSIBILITY
  │ applyAll(cart)
  ▼
[Seasonal]──>[Loyalty]──>[Bulk]──>[Banking]
   │             │           │        │
   │ getDiscount() — har coupon apni strategy se math karwata hai
   ▼             ▼           ▼        ▼
 PERCENT      PERCENT      FLAT   PERCENT_WITH_CAP   ← STRATEGY
   ▲             ▲           ▲        ▲
   └─────────────┴───────────┴────────┘
         DiscountStrategyManager.createStrategy(enum)  ← FACTORY + SINGLETON
```

**Flow ek line me:** Singleton manager → chain chalata hai → har link (coupon) apna rule check karta hai → math strategy se aata hai → strategy factory ne banayi thi.

---

## Design Decisions Jo Interview Me Bolne Layak Hain ⭐

1. **Do totals ka trick** ([`Cart.h`](./models/Cart.h)): `originalTotal` (kabhi nahi badalta — threshold checks iske against) vs `currentTotal` (har discount se ghatta hai — stacking iske upar). Isse pehle lage discounts kisi coupon ko unfairly disqualify nahi karte!

2. **Exclusive coupon ko chain me LAST rakha** (main.cpp): `BankingCoupon` non-combinable hai — pehle daal dete to baaki combinable coupons ko mauka hi nahi milta. **Registration order = application order** — ye design ka conscious decision hai.

3. **Category-level vs Cart-level discounts:** `SeasonalOffer` sirf matching category ke subtotal pe % lagata hai; baaki cart-level. Ek hi `getDiscount()` contract, alag-alag base amounts.

4. **Recursive chain cleanup:** `~Coupon() { delete next; }` — head delete karo, poori chain free. Elegant, par dhyan raho: bahut lambi chain pe stack-overflow ka theoretical risk (production me iterative loop better).

5. **Thread-safety sirf wahan jahan zaroorat:** CouponManager me mutex hai (shared mutable registry), strategies me nahi (stateless/immutable).

---

## Kya Improve Ho Sakta Hai (practice ideas)

- Raw pointers → `unique_ptr` (coupons, strategies, cart items) — manual delete chains khatam
- `getApplicable()` + `applyAll()` ke beech TOCTOU gap — ek hi lock me dono ka combined operation
- Strategy factory me **registry-based map** (L9 notes wala) — switch ka OCP problem solve
- Coupon priority/ordering ko registration order se alag karna (priority queue)
- `Decorator` se coupon ke upar logging/analytics wrap karna

---

**Related notes is repo me:** [L8 Strategy](../L8%20Strategy_Design_Patterns/README.md) · [L9 Factory](../L9%20Factory_Design_Pattern/notes/00_INDEX.md) · L15 Command (chain jaisa linking) · L28 Builder

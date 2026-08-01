# L23 Payment Gateway System — Design Patterns Used (Hinglish)

> Multi-gateway payment system (Paytm/Razorpay/PayPal) jisme **5 design patterns ek saath** kaam karte hain. Ek payment banta hai, retry ke saath robust hota hai, aur subscriptions ke liye reuse hota hai. Ye is repo ka sabse "pattern-dense" project hai — neeche har pattern ka role, kahan, aur kaise, detail me.

---

## Ek Nazar Me — Saare Patterns

| # | Pattern | Type | Role | Kahan |
|---|---------|------|------|-------|
| 1 | **Template Method** | Behavioral | Payment ka fixed skeleton (validate→initiate→confirm) | [`gateways/PaymentGateway.h`](./gateways/PaymentGateway.h) |
| 2 | **Strategy** | Behavioral | Retry timing + banking backend swap-able | [`retry/`](./retry/) + [`banking/BankingSystem.h`](./banking/BankingSystem.h) |
| 3 | **Proxy** | Structural | Real gateway pe retry wrap | [`proxy/PaymentGatewayProxy.h`](./proxy/PaymentGatewayProxy.h) |
| 4 | **Factory** | Creational | Gateway + retry strategy banana | [`factories/`](./factories/) + [`retry/RetryStrategyFactory.h`](./retry/RetryStrategyFactory.h) |
| 5 | **Singleton** | Creational | Controller + services + factories ek-ek instance | [`controllers/`](./controllers/), [`services/`](./services/) |

---

## Poora Flow — 5 Patterns Ek Saath

```
Controller.handlePayment(PAYTM, req)     [SINGLETON — ek entry point]
   │
   ▼
GatewayFactory.getGateway(PAYTM)         [FACTORY — object assembly]
   │  banata hai:
   │  ┌─────────────────────────────────────────────────┐
   │  │ new PaymentGatewayProxy(         [PROXY]         │
   │  │     new PaytmGateway(),          [TEMPLATE METHOD]│
   │  │     RetryStrategyFactory.create()[FACTORY+STRATEGY]│
   │  │ )                                                │
   │  └─────────────────────────────────────────────────┘
   ▼
Proxy.processPayment()                   [PROXY — retry loop]
   │  attempt 1 fail? -> RetryStrategy.getDelayMs() [STRATEGY] -> sleep -> retry
   ▼
PaytmGateway.processPayment()            [TEMPLATE METHOD]
   validate() -> initiate() -> confirm()  (fixed order, Paytm-specific steps)
   │
   ▼
PaytmBankingSystem.processPayment()      [STRATEGY — actual money backend]
   (80% random success)
```

**Ek call, 5 patterns.** Yahi is project ki khoobi — real production systems aise hi layered hote hain.

---

## 1. Template Method — Gateway ka fixed skeleton 📋

**Kahan:** [`gateways/PaymentGateway.h`](./gateways/PaymentGateway.h) (base) + `PaytmGateway`, `RazorpayGateway`, `PayPalGateway`

**Problem jo solve hua:** Har payment provider ka flow SAME 3 steps ka hai (validate → initiate → confirm), par HAR provider ke ye steps alag. Bina pattern ke har gateway apna poora flow likhta (duplicate order logic).

**Kaise:** Base class me `processPayment()` template method hai jo order fix karta:

```cpp
bool processPayment(req) {         // <- template method (order fixed)
    if (!validatePayment(req)) return false;   // STEP 1 (subclass fill)
    if (!initiatePayment(req)) return false;   // STEP 2 (subclass fill)
    if (!confirmPayment(req))  return false;   // STEP 3 (subclass fill)
    return true;
}
```

Har gateway ka `validate` DIFFERENT hai:
- Paytm: `amount > 0 && currency == "INR"` (sirf INR)
- Razorpay: `amount > 0` (any currency)
- PayPal: `amount > 0 && USD/EUR/GBP` (international only)

**Order base ke control me, details subclass me** — yahi Template Method. (L20 dekho.)

---

## 2. Strategy — Retry timing + Banking backend 🔀

**Do jagah Strategy use hua:**

### A. Retry Strategy ([`retry/`](./retry/))
"Fail hone pe kitni der wait karein?" — swap-able:
- `LinearRetryStrategy`: fixed wait (200, 200, 200ms)
- `ExponentialBackoffRetryStrategy`: badhta wait (100, 200, 400ms)

Exponential backoff production standard hai — server overload ho to badhta wait use "saans" deta (AWS/Google isi ko use karte).

### B. Banking Strategy ([`banking/BankingSystem.h`](./banking/BankingSystem.h))
Actual money process karne ka backend — har gateway ke andar composition se:
- `PaytmBankingSystem` (80% success), `RazorpayBankingSystem` (90%), `PayPalBankingSystem` (85%)

Ye random success hi RETRY ko meaningful banata hai (kabhi fail, retry pe pass).

---

## 3. Proxy — Retry wrapping 🔄

**Kahan:** [`proxy/PaymentGatewayProxy.h`](./proxy/PaymentGatewayProxy.h)

**Problem jo solve hua:** Retry logic har gateway me daalte to duplicate + gateway ka kaam (payment) retry ke saath mix ho jaata (cross-cutting concern).

**Kaise:** Proxy real gateway ko wrap karta (SAME `PaymentGateway` interface), par `processPayment()` me retry loop add karta:

```cpp
for (attempt = 0; attempt < maxRetries; attempt++) {
    if (attempt > 0) sleep(retryStrategy->getDelayMs(attempt));  // STRATEGY
    result = realGateway->processPayment(req);   // asli kaam delegate
    if (result) break;
}
```

Client ko proxy aur real gateway me FARQ nahi (dono `PaymentGateway`). Retry ka concern gateway se BAHAR — proxy me. (L21 dekho.)

**Note:** Ye proxy "behavior add" karta jaisa Decorator, par intent access-management ka hai (real gateway tak controlled access) — isliye Proxy.

---

## 4. Factory — Gateway + Retry banana 🏭

**Do factories:**
- [`factories/GatewayFactory.h`](./factories/GatewayFactory.h) — **sabse important**, teen patterns assemble karta: retry strategy banao (Factory) + real gateway banao (Factory) + proxy me wrap (Proxy). Client ko ready proxied gateway milta.
- [`retry/RetryStrategyFactory.h`](./retry/RetryStrategyFactory.h) — RetryStrategyType se sahi retry strategy.

**Fayda:** Complex object assembly (proxy + gateway + strategy ka nested setup) EK jagah chhupi. Client sirf `getGateway(PAYTM)` bolta.

---

## 5. Singleton — Central services 1️⃣

**Kahan:** `PaymentController`, `PaymentService`, `RecurringPaymentService`, `GatewayFactory` — sab singletons.

**Style:** Modern eager static member (`static X instance;` + `inline` C++17 + deleted copy/assignment). Thread-safe by static-init. Ek-ek instance poore system me. (L10 me styles ka comparison.)

**Kyun:** Payment controller, service, factory — inka ek-ek hona logical hai (do payment controllers = confusion).

---

## Design Highlights (interview me bolne layak) ⭐

1. **GatewayFactory = 3 patterns ka assembler:** ek method me Factory (retry) + Factory (gateway) + Proxy (wrap) — real object composition ka best example.

2. **One-time + Recurring reuse:** `RecurringPaymentService` billing cycle me WAHI payment flow use karta jo one-time me tha. Ek infrastructure, do use-cases.

3. **Nested ownership cleanup:** Proxy real gateway + retry strategy ka malik; gateway banking system ka malik. Ek `delete` se poora chain saaf (nested destructors) — no leaks.

4. **Provider-specific validation:** har gateway ka `validate` alag (INR-only, any, international) — Template Method ka "same skeleton, alag steps" perfectly dikhta.

5. **Random banking + retry:** demo har baar thoda alag output de sakta (rand success) — retry ka asli value dikhta hai.

---

## Files Structure

```
L23 Payment_gateway_system_LLD/
├── design_patterns_used.md         ← Ye file
├── main.cpp                        ← demo driver
├── controllers/PaymentController.h ← Singleton entry point
├── services/                       ← PaymentService + RecurringPaymentService
├── gateways/                       ← PaymentGateway (Template Method) + 3 concrete
├── proxy/PaymentGatewayProxy.h     ← Proxy (retry wrap)
├── retry/                          ← RetryStrategy (Strategy) + 2 concrete + factory
├── banking/BankingSystem.h         ← Banking Strategy (3 backends)
├── factories/GatewayFactory.h      ← Main factory (assembler)
├── utils/                          ← receipt helpers (Paytm/Razorpay/PayPal)
├── models/                         ← PaymentRequest, Subscription
├── enums/                          ← GatewayType, RetryStrategyType, BillingInterval
└── C++ Original Code/              ← monolith (single-file reference)
```

---

## Kya Improve Ho Sakta Hai (practice ideas)

- Raw pointers → smart pointers (gateways, strategies, subscriptions)
- `PaymentRequest::reciever` typo fix ("receiver")
- Retry me "jitter" add karo (exponential backoff + randomness — thundering herd se bachav)
- Actual billing scheduling (weekly/monthly interval ko time se link karo — abhi label hai)
- Idempotency keys (double-charge se bachne ke liye)

---

**Related lessons:** [L8 Strategy](../L8%20Strategy_Design_Patterns/README.md) · [L9 Factory](../L9%20Factory_Design_Pattern/notes/00_INDEX.md) · [L10 Singleton](../L10%20Singleton_Design_Pattern/) · [L20 Template Method](../L20%20Template_Method_Pattern/) · [L21 Proxy](../L21%20Proxy_Design_Pattern/)

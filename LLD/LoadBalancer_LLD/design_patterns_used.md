# Design Patterns Used — Load Balancer LLD

Ye system asal me **Strategy Pattern ka textbook example** hai: "requests ko kaunse server pe
bhejein" ka faisla ek pluggable, runtime-swappable algorithm hai. Iske saath ek Facade (poore
system ka single entry point) aur ek chhoti health-aware routing technique hai.

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                            PATTERN OVERVIEW                                    │
├──────────────────┬─────────────────────────────────┬─────────────────────────┤
│  Pattern         │  Kahan (class)                  │  Kis liye                │
├──────────────────┼─────────────────────────────────┼─────────────────────────┤
│  Strategy ⭐     │  LoadBalancingStrategy + 2 impls│  routing algorithm swap  │
│  Facade / Context│  LoadBalancer                   │  system entry + strategy │
│                  │                                 │  hold karta hai          │
│  Health-aware    │  Server.isAvailable() + status  │  DOWN servers skip       │
│  routing         │                                 │  (self-healing)          │
└──────────────────┴─────────────────────────────────┴─────────────────────────┘
```

---

## 1. Strategy Design Pattern ⭐ (system ka dil)

### 📌 Concept
Ek algorithm ko interface ke peeche rakh do taaki wo runtime pe swap ho sake, aur naye
algorithm bina purana code chhue add ho sakein. Client ko sirf interface dikhta hai, concrete
algorithm nahi.

### PATTERN KE ROLES
- **Strategy interface** → [`LoadBalancingStrategy`](./strategies/LoadBalancingStrategy.h) (`selectServer`)
- **Concrete Strategies**:
  - [`RoundRobinStrategy`](./strategies/RoundRobinStrategy.h) — baari-baari (stateful: `nextIndex_`)
  - [`LeastConnectionsStrategy`](./strategies/LeastConnectionsStrategy.h) — sabse kam-load server (stateless)
- **Context** → [`LoadBalancer`](./core/LoadBalancer.h) — `unique_ptr<LoadBalancingStrategy>` hold karta hai

### Implementation
```cpp
LoadBalancer lb(make_unique<RoundRobinStrategy>()); // ek strategy se shuru
lb.routeRequest();                                   // andar: strategy->selectServer(servers)
lb.setStrategy(make_unique<LeastConnectionsStrategy>()); // ⭐ runtime pe SWAP
lb.routeRequest();                                   // ab dusra algorithm chal raha hai
```

### ⭐ KYUN? (Why Strategy)
> Naya algorithm chahiye (`WeightedRoundRobin`, `IpHashStrategy`, `RandomStrategy`)? Bas nayi
> class banao jo `LoadBalancingStrategy` implement kare — `LoadBalancer`, `Server`, `main` ka
> ek line nahi badlega. Aur `setStrategy()` se **chalte-chalte** algorithm badla ja sakta hai.
> Yahi **Open/Closed Principle** + NFR2/NFR6 ("extensible", "switchable at runtime").

### ⭐ Stateful vs Stateless strategy (accha interview point)
> - **RoundRobin STATEFUL hai** — `nextIndex_` calls ke beech yaad rehta hai (kahan se agla shuru ho).
> - **LeastConnections STATELESS hai** — har call fresh scan; load ki state servers ke paas hai.
> Dono same interface follow karti hain, par internal state ka nature alag — Strategy dono ko allow karti hai.

---

## 2. Facade Pattern (+ Strategy's Context) — `LoadBalancer`

### 📌 Concept
Facade ek complex subsystem ke aage ek patla interface rakh deta hai. Yahan `LoadBalancer`
do role nibha raha hai: **Facade** (system ka entry) AUR Strategy pattern ka **Context**
(strategy ko hold + delegate karta hai).

### Implementation
Client ko servers ki registry, connection counts, ya routing algorithm ki koi khabar nahi.
Wo bas 5 method jaanta hai:
```cpp
addServer(id, host);        markServerStatus(id, UP/DOWN);
routeRequest();             completeRequest(id);        setStrategy(...);
```
Andar: server chunna strategy ko delegate hota hai, phir chune gaye server ka connection count
badhta hai (`incrementConnections`) — ye do kaam ek jagah orchestrate hote hain.

### ⭐ KYUN?
> Client aur subsystem decoupled. `routeRequest()` ke peeche "strategy se chuno + load count
> badhao" wala orchestration chhupa hua hai — client ko sirf "mujhe ek server do" chahiye.

---

## 3. Health-aware routing (technique, GoF nahi)

`Server` ka [`ServerStatus`](./enums/ServerStatus.h) (UP/DOWN) + `isAvailable()` routing ko
"self-healing" banata hai:
- Dono strategies `selectServer` me DOWN servers ko **skip** karti hain.
- Ek server DOWN mark karte hi traffic apne aap baaki UP servers pe shift ho jaata hai.
- Saare DOWN? → strategy `nullptr` deti hai → `LoadBalancer` "No available server" error deta hai (crash nahi).

> Real LB me health automatic probes (ping/HTTP) se aati hai; yahan `markServerStatus()` se
> manually simulate ki gayi (requirements Assumption).

---

## Object Relationships (UML samajhne ke liye)

```
LoadBalancer  ◇──  LoadBalancingStrategy   (Aggregation: unique_ptr, injected + swappable)
LoadBalancer  ◆──  Server (vector)         (Composition: LB owns, dtor me delete)
RoundRobinStrategy      ─▷  LoadBalancingStrategy   (implements)
LeastConnectionsStrategy ─▷ LoadBalancingStrategy   (implements)
Strategy       ··▶  Server                 (Dependency: selectServer param me milte, own nahi karti)
Server         ──▶  ServerStatus           (has-a enum)
```
- `◆──` Composition (owns) · `◇──` Aggregation (injected, swappable) · `─▷` Inheritance · `··▶` Dependency

---

## Requirement → Pattern/Technique quick map

| Requirement | Kaise pura hua |
|-------------|----------------|
| onboard servers | `LoadBalancer::addServer` (duplicate id reject) |
| health UP/DOWN | `ServerStatus` + `markServerStatus` |
| route to available server | strategy `selectServer` + DOWN skip |
| active connection count | `Server` counters (increment on route, decrement on complete) |
| strategy-based routing | **Strategy** (`LoadBalancingStrategy`) |
| switchable at runtime | **Strategy** `setStrategy()` (Context swap) |
| extensible (NFR2) | naya algorithm = nayi class, purana code untouched |
| no `bits/stdc++.h` cleanup | ✅ saare files targeted includes pe shift |

> **Cleanup notes:** (1) har file me `#include <bits/stdc++.h>` tha (non-portable, GCC-only) —
> **hata diya**, targeted includes pe. (2) `LoadBalancer` raw `Server*` own karta hai aur dtor
> me delete karta hai, par copy/move explicitly delete nahi the — **explicitly `= delete`** kar
> diye (Rule of Three/Five clarity; double-free se bachav). Behavior same, niyat saaf.

---

## "Ye pattern kyun NAHI use kiya" (interview me pooche jaate hain)

- **Factory for strategy creation?** Abhi client seedha `make_unique<RoundRobinStrategy>()`
  deta hai — 2 hi strategies hain. Config/string se strategy banani ho ("round_robin" → object)
  tab factory add karenge. Abhi **YAGNI**.
- **Observer for health changes?** Abhi health manually set hoti hai aur routing turant dekh
  leti hai (pull). Automatic health-check system + "server down hua to alert" chahiye tab
  Observer aayega.
- **Singleton LoadBalancer?** Ek instance `main` me kaafi. Singleton global state deta jo
  testing mushkil karta — inject karna behtar hai.
- **`vector<unique_ptr<Server>>` kyun nahi (raw pointer ki jagah)?** Better hota — tab manual
  `delete` ki zaroorat hi na padti aur copy/move safe ho jaate. Yahan raw+delete jaan-boojh ke
  "interview-simple" rakha, par production me smart pointer prefer karo (comment me note kiya hai).
- **Thread-safety (mutex)?** Requirements single-process, deterministic in-memory maangte hain.
  Concurrent routing chahiye to `routeRequest`/counters ko mutex/atomic se guard karna padega —
  natural extension (ATM/Cache LLDs me ye pattern dekha ja sakta hai).
```

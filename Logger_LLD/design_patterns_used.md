# Design Patterns Used — Logger LLD

Ye ek Log4j/SLF4J-style logging framework hai. Iska design **paanch** patterns ka sundar
combination hai: Singleton (global logger), Chain of Responsibility (level routing), Strategy
(format + destination), Observer (per-level appenders), aur Bridge-like (appender × formatter
independent). Sab milke ek highly extensible logger banate hain (naya level/format/output = minimal change).

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                            PATTERN OVERVIEW                                    │
├──────────────────────┬─────────────────────────────────┬──────────────────────┤
│  Pattern             │  Kahan (class)                  │  Kis liye             │
├──────────────────────┼─────────────────────────────────┼──────────────────────┤
│  Singleton           │  Logger (Meyers)                │  ek global logger     │
│  Chain of Responsib. │  LogHandler + 5 level handlers  │  level -> sahi handler│
│  Strategy            │  LogFormatter, LogAppender      │  format + destination │
│  Observer            │  LogHandler.subscribe/notify    │  per-level appenders  │
│  Bridge-like         │  Appender holds a Formatter     │  format × output free │
└──────────────────────┴─────────────────────────────────┴──────────────────────┘
```

---

## 1. Singleton — `Logger` (Meyers Singleton)

### 📌 Concept
Poore app me ek hi instance + ek global access point. Logging ek "global service" hai — har
class apna logger na banaye, sabko same chahiye.

### Implementation ([`Logger`](./Logger.h))
- Constructor **private** (bahar se `Logger l;` nahi).
- Copy ctor + assignment **delete** (doosri copy nahi).
- `getInstance()` me `static Logger instance;` — **Meyers Singleton**.

### ⭐ KYUN Meyers? (thread-safe + lazy, bina manual locking)
> `static` local variable ka initialization C++11+ me **guaranteed thread-safe** hai — do
> threads ek saath `getInstance()` bulayein to bhi instance sirf EK baar, safely banta. Koi
> manual mutex/double-checked-locking nahi chahiye. Aur "lazy" bhi — pehli call pe banta.

---

## 2. Chain of Responsibility — level routing ⭐

### 📌 Concept
Request (log message) ko handlers ki ek chain me daalo; har handler decide kare "mera kaam hai?
haan to handle, nahi to agle ko de do."

### PATTERN KE ROLES
- **Handler base** → [`LogHandler`](./handlers/LogHandler.h) (chain link + `handle()` logic)
- **Concrete handlers** → [`DebugHandler`](./handlers/DebugHandler.h), Info, Warn, Error, [`FatalHandler`](./handlers/FatalHandler.h)
- **Chain wiring** → [`LogHandlerConfiguration::build()`](./LogHandlerConfiguration.h)

### ⭐ Chain order aur routing (SAHI behaviour — accurately)
```
Debug -> Info -> Warn -> Error -> Fatal        (build() isi order me wire karta; head = Debug)

ERROR message: Debug(==DEBUG? no) -> Info(no) -> Warn(no) -> Error(==ERROR? YES) -> notify ✅
```
> **⚠ Ye EXACT-LEVEL match (`==`) hai, "≥ threshold" nahi.** Har handler SIRF apna exact level
> handle karta hai — kyunki is design me har level ke apne ALAG appenders hote hain (INFO ka apna
> output, ERROR ka apna). Message us handler ke appenders ko jaata hai jiska level exactly match kare.
> TRACE ka koi handler nahi -> chain ke end tak jaake **silently drop**.
> *(Note: pehli design-doc me chain order aur "≥" galat likha tha — ye version code se verified hai.)*

### ⭐ KYUN? (Why CoR)
> Naya level add karna = ek nayi chhoti handler class + chain me jodo (`setNext`). Purane handlers
> ka code untouched (har handler sirf apna `canHandle` janta). **Open/Closed Principle.**

---

## 3. Strategy — Formatter aur Appender (do jagah)

### 📌 Concept
Badalne wale algorithm ko interface ke peeche rakho, taaki swappable ho.

| Strategy | Interface | Concrete |
|----------|-----------|----------|
| **Format** (kaise dikhe) | [`LogFormatter`](./formatter/LogFormatter.h) | [`PlainTextFormatter`](./formatter/PlainTextFormatter.h), [`JsonFormatter`](./formatter/JsonFormatter.h) |
| **Destination** (kahan jaaye) | [`LogAppender`](./appenders/LogAppender.h) | [`ConsoleAppender`](./appenders/ConsoleAppender.h), [`FileAppender`](./appenders/FileAppender.h) |

### ⭐ KYUN?
> Naya format (XmlFormatter) ya output (NetworkAppender) chahiye? Nayi class banao jo interface
> implement kare — baaki system untouched. Same message plain-text console pe, JSON file me ja
> sakta — bas strategy swap.

---

## 4. Observer — per-level appenders ⭐

### 📌 Concept
Subject (handler) apne registered observers (appenders) ko event (message) pe notify karta hai.

### Implementation ([`LogHandler`](./handlers/LogHandler.h))
- **subscribe(appender)** → us level ke handler pe appender register (observer add).
- **notifyObservers(message)** → us handler ke SAARE appenders ko `append()` bhejo.

Isi se ek level pe MULTIPLE outputs: demo me `ERROR -> Console AUR File` (do appenders subscribed).

### ⭐ Thread-safe "copy-then-notify" (nazuk detail)
> `notifyObservers` pehle LOCK leke appenders ki LOCAL COPY banata hai, phir LOCK CHHOD ke us copy
> pe `append()` bulata. Kyun? `append()` slow ho sakta (file/network I/O) — lock hold karke karte
> to subscribe() atak jaata, aur re-entrant subscribe pe deadlock. Copy banake lock jaldi chhodna =
> kam contention, safe. (Java ka CopyOnWriteArrayList wali soch.)

---

## 5. Bridge-like — Appender × Formatter independent

Appender ek `shared_ptr<LogFormatter>` **hold** karta hai (compose karta, inherit nahi).
> Isse "kahan" (appender) aur "kaise" (formatter) do **independent dimensions** ho gaye. 2 formats
> × 2 destinations = 4 combos bina naya code (console+plain, console+json, file+plain, file+json).
> Ye Bridge pattern ka core idea hai: do orthogonal hierarchies ko composition se jodo, taaki dono
> alag-alag vary kar sakein (2+2 classes se 2×2 behaviours).

---

## Object Relationships (UML samajhne ke liye)

```
Logger              ◆──  LogHandler (chain head)     (Composition)
LogHandler          ──▶  LogHandler (next)           (Association: chain link)
LogHandler          ◇──  LogAppender (subscribers)   (Aggregation: observers)
Debug/Info/.../Fatal ─▷  LogHandler                  (Inheritance)
ConsoleAppender/FileAppender ─▷ LogAppender          (Inheritance)
Appender            ◇──  LogFormatter                (Aggregation: injected — Bridge)
PlainText/Json      ─▷   LogFormatter                (Inheritance)
```
- `◆──` Composition · `◇──` Aggregation (injected) · `──▶` Association · `─▷` Inheritance

---

## Requirement → Pattern quick map

| Requirement | Pattern |
|-------------|---------|
| single global logger (debug/info/...) | **Singleton** (Logger) |
| level ke hisaab se route | **Chain of Responsibility** |
| per-level 1+ appenders | **Observer** (subscribe/notify) |
| plain text + JSON formats | **Strategy** (LogFormatter) |
| console + file outputs | **Strategy** (LogAppender) |
| output format se decoupled | **Bridge-like** (appender ◇ formatter) |
| naya level/format/output minimal change (OCP) | sab patterns milke |

> **Cleanup + fixes:**
> 1. 🧹 `Main.cpp` me `#include <bits/stdc++.h>` tha (non-portable) — hata diya, targeted includes.
> 2. 🐛 **Idempotent `build()` fix:** `build()` har baar chain ko NAYE sire se banata tha. Agar
>    appenders `getInstance()` se PEHLE configure hote, to Logger ka constructor `build()` ko dobara
>    chala ke handlers reset kar deta -> **pehle ki subscriptions kho jaati**. Ab `build()` idempotent
>    hai (chain bani hai to wahi lauta ta) — subscriptions safe. **Verified** ek order-test se.
> 3. 📄 Ye design-doc **rewrite** hua — pehle chain order ("Info->Debug->Warn") aur "≥ threshold"
>    semantics galat likhe the; ye version code se verified (exact `==` routing, Debug->...->Fatal),
>    aur Observer + Bridge patterns bhi add kiye (jo pehle missing the).

---

## "Ye pattern kyun NAHI use kiya" (interview me pooche jaate hain)

- **`≥ threshold` routing (classic Log4j)?** Ye design jaan-boojh ke **per-level appenders** deta hai
  (har level ka apna output set), isliye exact-match. Classic threshold model (message.level >= config
  se emit) alag design hai — dono valid, use-case pe depend. (Threshold chahiye to `canHandle` me `>=`
  aur ek hi terminal handler kaafi hota.)
- **Decorator for formatters (chaining: JSON + encrypt + compress)?** Abhi single formatter kaafi.
  Multiple transformations pipeline chahiye tab Decorator (formatter ko formatter me wrap) add karte.
- **Async/buffered logging (producer-consumer queue)?** Real high-throughput logger me hota (log
  call block na kare), par problem statement me async out of scope. Extension: appender ke aage ek
  lock-free queue + background writer thread.
- **Abstract Factory for appender+formatter bundles?** 2×2 combos abhi manually banate. Bahut sare
  presets ("prod config", "dev config") ho to factory helpful — abhi over-engineering.
```

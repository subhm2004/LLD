# Design Patterns Used — URL Shortener LLD

Ye ek chhota par saaf system hai. Iska core do baaton pe khada hai: ek **Facade** (poore
system ka single entry point) aur **Delegation/SRP** (encoding ka kaam ek alag utility ko).
Saath me kuch important techniques hain — counter-based collision-free short codes, dual-index
maps (dedup + resolve), aur ek memory-safety fix.

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                            PATTERN OVERVIEW                                    │
├──────────────────┬─────────────────────────────────┬─────────────────────────┤
│  Pattern         │  Kahan (class)                  │  Kis liye                │
├──────────────────┼─────────────────────────────────┼─────────────────────────┤
│  Facade ⭐       │  UrlShortnerService             │  system ka single entry  │
│  Delegation / SRP│  Base62Encoder                  │  encoding alag concern   │
│  (technique)     │  counter + base62               │  collision-free codes    │
│  (technique)     │  do index maps                  │  dedup + O(1) resolve    │
└──────────────────┴─────────────────────────────────┴─────────────────────────┘
```

---

## 1. Facade Design Pattern ⭐ — `UrlShortnerService`

### 📌 Concept
Facade ek complex subsystem ke aage ek patla, saaf interface rakh deta hai — client ko andar
ke moving parts se matlab hi nahi rehta.

### PATTERN KE ROLES
- **Facade** → [`UrlShortnerService`](./services/UrlShortnerService.h)
- **Subsystem** → `Base62Encoder` (encoding), 2 index maps (storage), `UrlEntry` (records), validation

### Implementation
Client ([`main.cpp`](./main.cpp)) sirf 3 method jaanta hai:
```cpp
shortner.shortenUrl(longUrl);   // andar: validate + dedup + code-gen + store
shortner.resolveShortUrl(short);// andar: extract code + lookup + click++
shortner.printStats(short);     // andar: lookup + analytics print
```
Base62 encoding, unique-code generation, dedup index, click counters — sab hidden.

### ⭐ KYUN? (Why Facade)
> Client aur subsystem decoupled. Kal ko encoding badle (base62 → hash), ya storage DB me jaaye,
> ya custom-alias feature aaye — `main.cpp` ka ek line nahi badlega. Sirf facade ke andar adjust hoga.

---

## 2. Delegation + Single Responsibility (SRP) — `Base62Encoder`

### 📌 Concept
Har class ka ek hi kaam ho. `UrlShortnerService` khud number→string encoding nahi karta — wo
ek dedicated utility ko **delegate** karta hai.

### Implementation
[`Base62Encoder`](./utils/Base62Encoder.h) ka ek hi kaam: `long long` ko base62 string me badalna.
Ye ek **stateless pure function** hai (`static encode` — same input, same output).
```cpp
Base62Encoder::encode(1000) // -> "g8"
```

### ⭐ KYUN?
> Encoding ek independent, testable, reusable concern hai. Alag rakhne se: (a) ise akele test
> karna trivial hai, (b) service ka code saaf rehta hai, (c) encoding scheme badalna ho to sirf
> yahi class badlegi. **Loose coupling** — problem statement ka "code generation aur storage
> loosely coupled" objective yahi pura karta hai.

---

## 3. Technique: Counter + Base62 → collision-free short codes ⭐

Short code kaise banta hai (`generateUniqueShortCode`):
1. Ek monotonically-increasing counter (`idCounter_`, 1000 se shuru).
2. Us number ko `Base62Encoder::encode()` se chhoti string me badlo.

> **⭐ Ye collision-free BY DESIGN hai** — base62 encoding ek **bijection** hai (har alag number
> ka alag code). Isliye alag counters kabhi same code nahi de sakte. Service me jo `while + count==0`
> check hai wo ek **defensive safety net** hai (counter-based me kabhi trigger nahi hoga) — wo tab
> kaam aata jab code-generation random/hash-based ho (jahan collision mumkin hai).

**Base62 kyun (base10 nahi)?** 62 characters (0-9, a-z, A-Z) se utni hi lambai me bahut zyada
codes: 3 char → 62³ = 238,328 (vs base10 ke 1000). Chhote links, crores URLs. (URL-safe bhi —
base64 ke `/` `+` nahi.)

---

## 4. Technique: Dual-index maps (dedup + O(1) resolve)

| Map | Kaam | Requirement |
|-----|------|-------------|
| `shortToEntry_` : shortCode → `UrlEntry*` | resolve/stats — code se entry | R3, R4, R5 |
| `longToShort_` : longUrl → shortCode | dedup — "ye URL pehle se hai?" | R2 |

> Do map = dono direction ki O(1) lookup. `longToShort_` ki wajah se same long URL dobara
> shorten karne pe **wahi purana code** milta hai (R2 dedup), naya nahi banta. "Thodi extra
> memory, par dono taraf fast" — classic space-for-speed trade-off.

---

## Object Relationships (UML samajhne ke liye)

```
UrlShortnerService  ◆──  UrlEntry (shortToEntry_)  (Composition: service OWNS, dtor deletes)
UrlShortnerService  ··▶  Base62Encoder             (Dependency: static call, own nahi karta)
main                ──▶  UrlShortnerService        (Association: uses facade)
```
- `◆──` Composition (owns) · `··▶` Dependency (uses) · `──▶` Association

---

## Requirement → Pattern/Technique quick map

| Requirement | Kaise pura hua |
|-------------|----------------|
| R1 shorten valid long URL | `shortenUrl` + `validateLongUrl` |
| R2 same URL → same short (dedup) | `longToShort_` reverse index |
| R3 resolve short → long | `shortToEntry_` lookup + `extractShortCode` |
| R4 track clicks | `UrlEntry::incrementClickCount` on every resolve |
| R5 expose stats | `printStats` (clicks + createdAt) |
| R6 reject invalid URL | `validateLongUrl` (empty / http(s) check) |
| collision-safe (NFR1) | counter + base62 **bijection** |
| O(1) average (NFR2) | `unordered_map` dual index |
| modular/extensible (NFR3) | Facade + delegated encoder |

> **Cleanup + fix notes:** (1) har file me `#include <bits/stdc++.h>` tha (non-portable, GCC-only)
> — **hata diya**, targeted includes pe. (2) 🐛 **Double-free fix:** `UrlShortnerService` raw
> `UrlEntry*` own karke dtor me delete karta hai, par isme koi `unique_ptr` member nahi tha jo
> implicit copy ko rok le — to compiler ka default copy constructor pointers ko **shallow copy**
> karta aur do copies same pointer delete karke crash karte (double free). **Copy/move ko `= delete`**
> kar diya — ab galti se copy compile-error degi, silent crash nahi. (Rule of Three/Five.)

---

## "Ye pattern kyun NAHI use kiya" (interview me pooche jaate hain)

- **Strategy for code generation?** Ho sakta hai (`CounterStrategy` vs `HashStrategy` vs
  `RandomStrategy`), par abhi ek hi scheme (counter+base62) chahiye. Multiple pluggable
  schemes ki zaroorat pade tab Strategy add karenge. Abhi **YAGNI**.
- **Singleton service?** Ek instance `main` me kaafi. Singleton global state deta jo testing
  mushkil karta — inject/local object behtar.
- **Hash of URL as short code?** Tempting, par hash **collisions** possible hain (do alag URL,
  same hash) — tab collision-handling chahiye. Counter+base62 **guaranteed unique** hai bina extra
  logic ke. (Distributed setup me counter ki jagah Snowflake-ID / ZooKeeper range aata — extension.)
- **`unique_ptr`/by-value storage abhi kyun nahi?** Better hota (manual delete hi na padta).
  Yahan raw+delete "interview-simple" rakha, par `= delete` copy/move se safe bana diya.
  Production me smart pointer ya by-value prefer karo (comment me note kiya hai).
- **Thread-safety (mutex)?** Requirements single-instance, deterministic in-memory maangte hain.
  Concurrent shorten/resolve chahiye to maps ko mutex se guard karna padega — natural extension.
```

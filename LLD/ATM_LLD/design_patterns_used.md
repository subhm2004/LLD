# Design Patterns Used — ATM LLD

ATM ka poora code chhota par saaf-suthra hai. Ise **clean, decoupled aur extensible**
rakhne ke liye ek main structural pattern (Facade), ek layered service architecture
(SRP + Delegation), aur ek simplified session-state idea use hui hai. Saath me note
dispensing me ek **greedy bug ko backtracking se fix** kiya gaya hai.

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                            PATTERN OVERVIEW                                    │
├──────────────────┬─────────────────────────────────┬─────────────────────────┤
│  Pattern         │  Kahan (class)                  │  Kis liye                │
├──────────────────┼─────────────────────────────────┼─────────────────────────┤
│  Facade          │  ATMSystem                      │  Poore system ka entry   │
│  Service Layer + │  AuthenticationService,         │  SRP: auth aur cash      │
│  Delegation (SRP)│  CashDispenser                  │  logic alag              │
│  Session State   │  currentCardNumber_             │  login/logout session    │
│  (simplified)    │  (ATMSystem ke andar)           │  track                    │
└──────────────────┴─────────────────────────────────┴─────────────────────────┘
```

---

## 1. Facade Design Pattern (system ka single entry point)

### 📌 Concept
Facade ek complex subsystem (kai chhoti classes/services) ke aage ek **patla, saaf
interface** rakh deta hai — client ko andar ke moving parts se matlab hi nahi rehta.

### PATTERN KE ROLES
- **Facade** → [`ATMSystem`](./core/ATMSystem.h)
- **Subsystem** → `AuthenticationService`, `CashDispenser`, `BankAccount`, `Card`, `Transaction`

### Implementation
Client ([`main.cpp`](./main.cpp)) sirf 4 method jaanta hai — `login`, `checkBalance`,
`withdrawCash`, `logout`. Andar kya hota hai, use pata nahi:
```cpp
atm->withdrawCash(2300);
//  └─ andar: session-check -> balance-check -> notes-check (backtracking)
//            -> account.debit() -> dispenser.dispense() -> transaction log
```

### ⭐ KYUN? (Why Facade)
> Facade khud **saara data ka maalik** hai (accounts, cards, transactions) aur services
> ko orchestrate karta hai. Client aur subsystem poore decoupled — kal ko auth ka tareeka
> ya dispense ka algorithm badle, `main.cpp` ka ek line nahi badlega.

---

## 2. Service Layer + Delegation (SRP)

### 📌 Concept
Har class ka **ek hi kaam** ho (Single Responsibility). ATMSystem khud sab kuch nahi
karta — specific kaam dedicated services ko **delegate** kar deta hai.

### PATTERN KE ROLES
| Class | Ek hi zimmedari | Requirement |
|-------|-----------------|-------------|
| [`AuthenticationService`](./services/AuthenticationService.h) | Card + PIN verify | #1 |
| [`CashDispenser`](./services/CashDispenser.h) | Notes vault + dispensing algorithm | #4, #5 |
| [`BankAccount`](./models/BankAccount.h) | Balance rakhna + safe `debit()` | #3 |
| [`Transaction`](./models/Transaction.h) | Har action ka audit record | #6 |

### ⭐ KYUN? (Why delegate)
> `ATMSystem` khud PIN compare ya notes-math nahi karta — wo services ko bulata hai.
> Fayda: kal ko authentication me "3 galat PIN pe block" ya OTP aaye, to sirf
> `AuthenticationService` badlegi. Dispensing algorithm badle to sirf `CashDispenser`.
> Facade patla rehta hai, har concern apni jagah — **test karna aur badalna aasaan.**

---

## 3. Session State (Simplified State Pattern)

### 📌 Concept
ATM do haalat me hota hai — **"card nahi daala" (no session)** ya **"card daala hai"
(active session)**. Behavior in dono me alag hota hai (session ke bina balance/withdraw
allowed hi nahi).

### Implementation
[`ATMSystem`](./core/ATMSystem.h) ek `string currentCardNumber_` se session track karta hai:
```
login()  ─►  currentCardNumber_ set    (session ON)
logout() ─►  currentCardNumber_ clear  (session OFF)
```
Har protected action (`checkBalance`, `withdrawCash`) se pehle `getCurrentAccountOrThrow()`
chalta hai — jo session ON hai ya nahi, ye ek jagah check karta hai (single guard).

### ⭐ KYUN? (Full State pattern kyun nahi?)
> States sirf do hain aur transitions simple (login/logout). Ek full State pattern
> (alag `LoggedInState`/`LoggedOutState` classes) yahan **over-engineering** hota. Ek
> string + ek guard method kaafi saaf aur samajhne layak hai. 📌 "Jitna chahiye utna
> banao" — zaroorat badhe (multi-step transaction states) tab full State pattern laayenge.

---

## 4. ⭐ Bug Fix: Greedy → Backtracking (note dispensing)

### 🐛 Bug jo tha
Purana `CashDispenser` **greedy** tha: "hamesha sabse bada note pehle, jitna zyada ho sake."
Ye limited notes ke saath **valid withdrawal ko galti se reject** kar sakta hai.

**Failure example:** vault me `500 x1, 200 x3, 100 x0`, user maange **600**:
- **Greedy:** ek 500 uthaya → remaining 100 → 100 ka note hai hi nahi (x0) → *"notes not available"* ❌
- **Sach:** `200 + 200 + 200 = 600` aaram se ban sakta tha! Greedy ne pehla 500 uthaake khud ko phasa liya.

### ✅ Fix
[`CashDispenser`](./services/CashDispenser.h) ab **backtracking** karta hai: har denomination
pe "kitne notes lein" ke saare options (zyada se kam) try karta hai, aur ek choice fail hone
par **peeche aake** kam-notes wali choice try karta hai. Agar koi bhi valid combination possible
hai to wo **guaranteed** mil jaayegi.
```
600, vault {500x1, 200x3, 100x0}:
  500 ka 1 note try -> aage fail -> BACKTRACK
  500 ka 0 note     -> 200 ka 3 note -> 600 ✅  (200x3)
```

### ⭐ Teaching point
> Greedy "abhi ka best" leta hai, "poore ka best" nahi. `{100,200,500}` denominations
> **unlimited** supply ke saath greedy theek chalta (canonical system), par **limited**
> supply ke saath fail ho sakta hai. Isi liye ATM/coin-change me backtracking (ya DP)
> zyada bharosemand hai. (Verify bhi kiya: 100s khatam karke 600 maanga — ab `200x3` deta hai.)

---

## Object Relationships (UML samajhne ke liye)

```
ATMSystem  ◆──  AuthenticationService   (Composition: facade owns)
ATMSystem  ◆──  CashDispenser           (Composition)
ATMSystem  ◆──  accounts_ / cards_ / transactions_  (Composition: single source of truth)
Card       ──▶  BankAccount (accountNumber se)      (Association: id se refer)
AuthService ··▶ Card                    (Dependency: verify ke liye padhta hai)
```
- `◆──` Composition (owns-a) · `──▶` Association (refers-to) · `··▶` Dependency (uses)

---

## Requirement → Pattern quick map

| Requirement | Pattern / Mechanism |
|-------------|---------------------|
| #1 card + PIN auth | **Delegation** → AuthenticationService |
| #2 balance inquiry | Facade `checkBalance` + `BankAccount` |
| #3 cash withdrawal | Facade `withdrawCash` (multi-gate flow) |
| #4 validate session/balance/notes | Session guard + `debit()` + `canDispense()` |
| #5 dispense by denomination | **Backtracking** in `CashDispenser` |
| #6 transaction status | `Transaction` audit log (SUCCESS/FAILED dono) |
| Simple client API | **Facade** (ATMSystem) |

---

## "Ye pattern kyun NAHI use kiya" (interview me pooche jaate hain)

- **Full State pattern (LoggedIn/LoggedOut classes)?** States 2 hi hain, transitions
  trivial — ek string + guard kaafi. Zyada states (PIN-entry / transaction-in-progress /
  card-blocked) aayein tab justify hota. Abhi **YAGNI**.
- **Strategy for dispensing?** Ho sakta hai (`GreedyStrategy` vs `BacktrackStrategy`),
  par abhi ek hi sahi algorithm chahiye (backtracking). Multiple pluggable algorithms ki
  zaroorat nahi — over-engineering.
- **Observer for notifications?** ATM abhi kisi ko "ping" nahi karta (sirf return karta
  hai). Receipt/SMS chahiye tab Observer aayega.
- **Singleton for ATMSystem?** `main()` me ek object kaafi. Singleton global state deta
  jo testing mushkil karta — zaroorat na ho to mat lagao.
```

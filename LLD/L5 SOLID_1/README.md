# SOLID Principles — Part 1: SRP, OCP, LSP

> **Lesson L5** — the first three **SOLID** principles with **violated vs followed** C++17 examples you can compile and compare. SOLID is the foundation of every good LLD: it keeps classes focused, open to extension, and safely substitutable.

**Continues in:** [`L6 SOLID_2`](../L6%20SOLID_2/) (ISP, DIP, advanced LSP rules).
**Deep dive:** [`SOLID_HIGH_LOW_MODULES_DETAILED.md`](../SOLID_HIGH_LOW_MODULES_DETAILED.md) — high vs low-level modules, OCP/DIP trade-offs, SRP break patterns.

---

## Table of Contents

1. [What is SOLID?](#1-what-is-solid)
2. [How These Three Connect](#2-how-these-three-connect)
3. [SRP — Single Responsibility Principle](#3-srp--single-responsibility-principle)
4. [OCP — Open/Closed Principle](#4-ocp--openclosed-principle)
5. [LSP — Liskov Substitution Principle](#5-lsp--liskov-substitution-principle)
6. [Folder Map](#6-folder-map)
7. [Build & Run](#7-build--run)
8. [Smells Cheat Sheet](#8-smells-cheat-sheet)
9. [Study Order](#9-study-order)
10. [Interview Talking Points](#10-interview-talking-points)
11. [Summary](#11-summary)

---

## 1. What is SOLID?

SOLID is five design principles that together make object-oriented code **maintainable, extensible, and testable**:

| Letter | Principle | One-line intent | Lesson |
| ------ | --------- | --------------- | ------ |
| **S** | Single Responsibility | One class, one reason to change | L5 (here) |
| **O** | Open/Closed | Open for extension, closed for modification | L5 (here) |
| **L** | Liskov Substitution | Subtypes must be usable through the base type | L5 (here) |
| **I** | Interface Segregation | Many small interfaces beat one fat one | [L6](../L6%20SOLID_2/) |
| **D** | Dependency Inversion | Depend on abstractions, not concretions | [L6](../L6%20SOLID_2/) |

This lesson covers **S, O, L** — each with a `*_violated.cpp` (the problem) and a `*_followed.cpp` (the fix).

---

## 2. How These Three Connect

The principles reinforce one another. The most common failure chain:

```
SRP break  →  OCP stress  →  hard extension  →  fragile, untestable code
```

- When a class has **multiple reasons to change** (SRP broken), every feature edits that same file.
- Editing the same core file repeatedly means you can't extend **without modifying** (OCP broken).
- And if subtypes silently break the base contract (LSP broken), polymorphic extension becomes unsafe.

Keep SRP clean and OCP usually follows; respect LSP and your abstractions stay trustworthy.

---

## 3. SRP — Single Responsibility Principle

> **A class should have only one reason to change.**

### The violation — `SRP_violated.cpp`

`ShoppingCart` does three unrelated jobs at once:

```cpp
class ShoppingCart {
    vector<Product*> products;
public:
    void addProduct(Product* p);   // 1. cart/business logic
    void printInvoice();           // 2. presentation/formatting
    void saveToDatabase();         // 3. persistence
};
```

Three responsibilities = **three reasons to change** (pricing rules, invoice layout, DB schema) all colliding in one class.

### The fix — `SRP_followed.cpp`

Split by **change axis** — each class owns exactly one responsibility:

| Class | Single responsibility |
| ----- | --------------------- |
| `ShoppingCart` | Hold products and cart logic only |
| `InvoicePrinter` | Format/print the invoice |
| `CartRepository` (DB) | Persist the cart |

Now a change to invoice formatting touches only `InvoicePrinter`; the cart logic is untouched.

**SRP break smells:** business rules + persistence + external API + formatting in one class; every feature edits the same class; unit tests need many mocks for a tiny behavior.

---

## 4. OCP — Open/Closed Principle

> **Software entities should be open for extension but closed for modification.**

### The violation — `OCP_violated.cpp`

Discounts are an `if/else` ladder inside the cart. Every new discount type **edits** existing, tested code:

```cpp
double getDiscount(string type) {
    if (type == "festival") return ...;
    else if (type == "loyalty") return ...;
    // add "bank offer" → modify this method again (risk of regression)
}
```

### The fix — `OCP_followed.cpp`

Introduce a `Discount` abstraction; each discount is its own class. Adding a new one means **adding a class**, not editing the cart:

```cpp
class Discount { public: virtual double apply(double amount) = 0; };
class FestivalDiscount : public Discount { ... };
class LoyaltyDiscount  : public Discount { ... };
// new discount = new subclass; cart code never changes
```

This is the **Strategy** pattern in service of OCP. Extension without modification = fewer regressions.

---

## 5. LSP — Liskov Substitution Principle

> **Objects of a subtype must be replaceable for the base type without breaking correctness.**

### The violation — `LSP_Violated.cpp`

A single `Account` base declares `withdraw()`, but a fixed deposit can't be withdrawn from — so the subtype **throws** where the base promised it would work:

```cpp
class Account { public: virtual void withdraw(double amt); };
class FixedDeposit : public Account {
    void withdraw(double amt) override { throw runtime_error("not allowed"); } // ❌ breaks substitution
};
```

Any code using `Account*` polymorphically can crash when handed a `FixedDeposit`.

### The fix — `LSP_followed.cpp`

**Segregate the hierarchy by capability** so every subtype honors its base contract:

```cpp
class DepositOnlyAccount   { virtual void deposit(double) = 0; };
class WithdrawableAccount : public DepositOnlyAccount { virtual void withdraw(double) = 0; };

class SavingAccount    : public WithdrawableAccount { ... };  // deposit + withdraw
class CurrentAccount   : public WithdrawableAccount { ... };  // deposit + withdraw
class FixedTermAccount : public DepositOnlyAccount  { ... };  // deposit ONLY — no false promise
```

A `BankClient` that only deposits takes a `DepositOnlyAccount`; one that withdraws takes a `WithdrawableAccount`. No subtype throws "not supported."

### Also study — `LSP_followed_wrongly.cpp`

A subtle case that *looks* fixed but still violates substitution — worth reading to train your eye for hidden contract breaks (weakened post-conditions / strengthened pre-conditions). The formal rules are covered in [L6 LSP-Rules](../L6%20SOLID_2/).

---

## 6. Folder Map

```
L5 SOLID_1/C++ Code/
├── SRP/
│   ├── SRP_violated.cpp     # cart + invoice + DB in one class
│   └── SRP_followed.cpp     # split into focused classes
├── OCP/
│   ├── OCP_violated.cpp     # if/else discount ladder
│   └── OCP_followed.cpp     # Discount hierarchy (Strategy)
├── LSP/
│   ├── LSP_Violated.cpp           # FixedDeposit.withdraw() throws
│   ├── LSP_followed.cpp           # segregated account interfaces
│   └── LSP_followed_wrongly.cpp   # subtle hidden violation
└── summary.txt
```

> Each subfolder also has a `.png` class diagram alongside the `.cpp`.

---

## 7. Build & Run

```bash
cd "L5 SOLID_1/C++ Code"

g++ -std=c++17 -o srp_violated SRP/SRP_violated.cpp && ./srp_violated
g++ -std=c++17 -o srp_followed SRP/SRP_followed.cpp && ./srp_followed

g++ -std=c++17 -o ocp_violated OCP/OCP_violated.cpp && ./ocp_violated
g++ -std=c++17 -o ocp_followed OCP/OCP_followed.cpp && ./ocp_followed

g++ -std=c++17 -o lsp_violated LSP/LSP_Violated.cpp && ./lsp_violated
g++ -std=c++17 -o lsp_followed LSP/LSP_followed.cpp && ./lsp_followed
```

> Compare the *violated* and *followed* outputs side by side — the behavior is similar, but the *structure* (and changeability) is very different.

---

## 8. Smells Cheat Sheet

| Principle | Smell (something is wrong) | Minimal fix |
| --------- | -------------------------- | ----------- |
| **SRP** | One class handles logic + persistence + formatting | Split by reason-to-change; orchestrator keeps only workflow |
| **OCP** | A `switch`/`if-else` grows with every new variant | Introduce an interface; one class per variant (Strategy) |
| **LSP** | A subtype throws "not supported" or weakens the contract | Segregate the hierarchy by capability |

---

## 9. Study Order

1. `SRP_violated.cpp` → `SRP_followed.cpp`
2. `OCP_violated.cpp` → `OCP_followed.cpp`
3. `LSP_Violated.cpp` → `LSP_followed.cpp` → `LSP_followed_wrongly.cpp`
4. Deep read: [`SOLID_HIGH_LOW_MODULES_DETAILED.md`](../SOLID_HIGH_LOW_MODULES_DETAILED.md)
5. Continue to [`L6 SOLID_2`](../L6%20SOLID_2/) for ISP, DIP, and the formal LSP rules

---

## 10. Interview Talking Points

1. **SRP:** "One reason to change. If a class has business logic *and* persistence *and* formatting, it has three."
2. **OCP:** "Add behavior by adding a class, not by editing a tested one — usually via Strategy or polymorphism."
3. **LSP:** "If a subtype throws on a method the base promised, callers using the base type will break — segregate the hierarchy instead."
4. **The chain:** "SRP keeps boundaries clean, which makes OCP natural; LSP keeps polymorphic extension safe."
5. **Real example:** "A `FixedDeposit` shouldn't inherit `withdraw()` — model deposit-only and withdrawable accounts separately."

---

## 11. Summary

| Principle | Violation example | Fix | Key idea |
| --------- | ----------------- | --- | -------- |
| **SRP** | `ShoppingCart` does cart + invoice + DB | Split into focused classes | One reason to change |
| **OCP** | `if/else` discount ladder | `Discount` hierarchy | Extend without modifying |
| **LSP** | `FixedDeposit::withdraw()` throws | Segregated account interfaces | Subtypes stay substitutable |

> **Remember:** SRP, OCP, and LSP form a chain — **focused classes** (SRP) make **extension without modification** (OCP) easy, and **honest subtypes** (LSP) make that extension safe. Break the first link and the rest fray. 🔗

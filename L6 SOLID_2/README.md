# SOLID Principles — Part 2: ISP, DIP & Advanced LSP Rules

> **Lesson L6** — the last two **SOLID** principles (Interface Segregation, Dependency Inversion) plus the **formal LSP substitution rules** that tell you *exactly* when an override is safe. Each comes with **violated vs followed** C++17 examples.

**Builds on:** [`L5 SOLID_1`](../L5%20SOLID_1/) (SRP, OCP, LSP basics).
**Deep dive:** [`SOLID_HIGH_LOW_MODULES_DETAILED.md`](../SOLID_HIGH_LOW_MODULES_DETAILED.md) — high vs low-level modules, dependency direction strategy.

---

## Table of Contents

1. [Where L6 Fits in SOLID](#1-where-l6-fits-in-solid)
2. [ISP — Interface Segregation Principle](#2-isp--interface-segregation-principle)
3. [DIP — Dependency Inversion Principle](#3-dip--dependency-inversion-principle)
4. [Advanced LSP Rules](#4-advanced-lsp-rules)
5. [Folder Map](#5-folder-map)
6. [Build & Run](#6-build--run)
7. [The Five Principles in Sync](#7-the-five-principles-in-sync)
8. [Smells Cheat Sheet](#8-smells-cheat-sheet)
9. [Study Order](#9-study-order)
10. [Interview Talking Points](#10-interview-talking-points)
11. [Summary](#11-summary)

---

## 1. Where L6 Fits in SOLID

L5 covered **S, O, L** basics. This lesson completes the set:

| Letter | Principle | Covered in |
| ------ | --------- | ---------- |
| S, O, L | SRP, OCP, LSP basics | [L5](../L5%20SOLID_1/) |
| **I** | Interface Segregation | L6 (here) |
| **D** | Dependency Inversion | L6 (here) |
| **L (formal)** | LSP method/signature/property rules | L6 (here) |

---

## 2. ISP — Interface Segregation Principle

> **No client should be forced to depend on methods it does not use.** Prefer many small, role-specific interfaces over one fat one.

### The violation — `ISP_violated.cpp`

A single fat `Shape` interface declares both `area()` and `volume()`. A 2D shape has no volume, so it's **forced** to implement a method that makes no sense:

```cpp
class Shape {
public:
    virtual double area()   = 0;
    virtual double volume() = 0;   // ❌ meaningless for 2D shapes
};

class Square : public Shape {
    double area()   override { return s * s; }
    double volume() override { throw runtime_error("2D has no volume"); } // forced + lying
};
```

### The fix — `ISP_followed.cpp`

Split into role-specific interfaces; a class implements only what applies to it:

```cpp
class Two_Dimensional_Shape   { virtual double area() = 0; };
class Three_Dimensional_Shape { virtual double area() = 0; virtual double volume() = 0; };

class Square    : public Two_Dimensional_Shape   { double area() override; };           // no volume()
class Rectangle : public Two_Dimensional_Shape   { double area() override; };
class Cube      : public Three_Dimensional_Shape { double area() override; double volume() override; };
```

Now no class implements a method it doesn't need, and there are no `throw "not supported"` traps. ISP and LSP reinforce each other here.

---

## 3. DIP — Dependency Inversion Principle

> **High-level modules should not depend on low-level modules; both should depend on abstractions.** And: abstractions should not depend on details — details depend on abstractions.

### The violation — `DIP_violated.cpp`

The high-level `UserService` (policy) directly creates and depends on a concrete `MySQLDatabase` (detail):

```cpp
class UserService {
    MySQLDatabase db;              // ❌ hard-wired to one concrete DB
public:
    void addUser(string data) { db.save(data); }
};
// Switching to MongoDB means editing UserService; testing needs a real MySQL.
```

### The fix — `DIP_followed.cpp`

Introduce a `Database` abstraction. The service depends on the interface; the concrete DB is **injected**:

```cpp
class Database { public: virtual void save(string data) = 0; virtual ~Database() {} };
class MySQLDatabase : public Database { void save(string) override; };
class MongoDBDatabase : public Database { void save(string) override; };

class UserService {
    Database* db;                  // depends on abstraction
public:
    UserService(Database* d) : db(d) {}   // injected
    void addUser(string data) { db->save(data); }
};
```

Now `UserService` works with **any** `Database`, swapping MySQL ↔ MongoDB needs no service change, and tests can pass a mock. `DIP_followed_new_pointer_style.cpp` shows the same idea with a modern pointer style.

> **DIP enables OCP and clean testing** — it's the principle that "inverts" the dependency arrow so policy no longer points at infrastructure.

---

## 4. Advanced LSP Rules

L5 showed *that* subtypes must be substitutable. L6's `LSP-Rules/` folder makes it **precise** — an override is only safe if it obeys these contract rules:

### Method Rules (`MethodRules/`)

| Rule | File | Requirement |
| ---- | ---- | ----------- |
| **Pre-conditions** | `PreConditions.cpp` | A subtype may **weaken** (not strengthen) pre-conditions — don't demand *more* from callers |
| **Post-conditions** | `PostConditions.cpp` | A subtype may **strengthen** (not weaken) post-conditions — don't promise *less* |

### Signature Rules (`SingatureRules/`)

| Rule | File | Requirement |
| ---- | ---- | ----------- |
| **Return type** | `ReturnTypeRule.cpp` | Return type must be the same or a subtype (covariance) |
| **Method argument** | `MethodArgumentRule.cpp` | Parameter types must be the same or a supertype (contravariance) |
| **Exceptions** | `ExceptionRule.cpp` | A subtype must not throw new/broader checked exceptions the base didn't |

### Property Rules (`PropertiesRules/`)

| Rule | File | Requirement |
| ---- | ---- | ----------- |
| **Class invariants** | `ClassInvariants.cpp` | Invariants of the base must be preserved by the subtype |
| **History constraint** | `HistoryConstraint.cpp` | A subtype must not allow state changes the base forbids (e.g. mutating an "immutable" field) |

> Memory hook: **pre-conditions can only relax, post-conditions can only tighten** — "demand no more, promise no less."

---

## 5. Folder Map

```
L6 SOLID_2/C++ Code/
├── ISP/
│   ├── ISP_violated.cpp     # fat Shape (area + volume)
│   └── ISP_followed.cpp     # 2D / 3D segregated interfaces
├── DIP/
│   ├── DIP_violated.cpp           # UserService → concrete MySQL
│   ├── DIP_followed.cpp           # UserService → Database interface (injected)
│   └── DIP_followed_new_pointer_style.cpp
└── LSP-Rules/
    ├── MethodRules/      PreConditions.cpp · PostConditions.cpp
    ├── SingatureRules/   ReturnTypeRule.cpp · MethodArgumentRule.cpp · ExceptionRule.cpp
    └── PropertiesRules/  ClassInvariants.cpp · HistoryConstraint.cpp
```

> Each principle folder also ships a `.png` class diagram next to the code.

---

## 6. Build & Run

```bash
cd "L6 SOLID_2/C++ Code"

g++ -std=c++17 -o isp_violated ISP/ISP_violated.cpp && ./isp_violated
g++ -std=c++17 -o isp_followed ISP/ISP_followed.cpp && ./isp_followed

g++ -std=c++17 -o dip_violated DIP/DIP_violated.cpp && ./dip_violated
g++ -std=c++17 -o dip_followed DIP/DIP_followed.cpp && ./dip_followed

# LSP rules (run individually)
g++ -std=c++17 -o pre  "LSP-Rules/MethodRules/PreConditions.cpp"  && ./pre
g++ -std=c++17 -o post "LSP-Rules/MethodRules/PostConditions.cpp" && ./post
```

---

## 7. The Five Principles in Sync

```
SRP        OCP            DIP              ISP             LSP
stable  +  extension  +   correct      +  minimal     +   valid
bounds     points         dependency       contracts       substitution
                          direction
```

- **SRP** keeps each class's responsibility clean.
- **OCP** gives extension points so you don't edit tested code.
- **DIP** points dependencies at abstractions, not concretions.
- **ISP** keeps those abstractions lean so clients aren't forced into unused methods.
- **LSP** guarantees the abstractions are actually substitutable.

Together they produce architecture that is **extensible, testable, and safe to refactor**.

---

## 8. Smells Cheat Sheet

| Principle | Smell | Minimal fix |
| --------- | ----- | ----------- |
| **ISP** | A class implements methods it doesn't need (or throws "not supported") | Split the fat interface by role |
| **DIP** | High-level class imports/constructs a concrete SDK | Introduce an interface; inject the concrete via constructor; bind SDK in an adapter |
| **LSP** | Override strengthens a pre-condition or weakens a post-condition | Realign the override to the base contract, or re-model the hierarchy |

---

## 9. Study Order

1. `ISP_violated.cpp` → `ISP_followed.cpp`
2. `DIP_violated.cpp` → `DIP_followed.cpp` → `DIP_followed_new_pointer_style.cpp`
3. `LSP-Rules/` — Method rules → Signature rules → Property rules
4. Synthesis: [`SOLID_HIGH_LOW_MODULES_DETAILED.md`](../SOLID_HIGH_LOW_MODULES_DETAILED.md)

---

## 10. Interview Talking Points

1. **ISP:** "Don't force a 2D shape to implement `volume()` — split interfaces by role."
2. **DIP:** "High-level policy depends on an abstraction; inject the concrete detail so you can swap and mock it."
3. **DIP vs DI:** "DIP is the principle (point at abstractions); dependency injection is one technique to achieve it."
4. **LSP pre/post:** "A subtype may weaken pre-conditions and strengthen post-conditions — never the reverse."
5. **Exception rule:** "An override must not throw broader exceptions than the base, or callers break."
6. **Synthesis:** "ISP keeps abstractions small; DIP points dependencies at them; LSP keeps them substitutable."

---

## 11. Summary

| Principle | Violation example | Fix | Key idea |
| --------- | ----------------- | --- | -------- |
| **ISP** | Fat `Shape` with `area()` + `volume()` | 2D / 3D interfaces | Lean, role-specific interfaces |
| **DIP** | `UserService` depends on `MySQLDatabase` | Depend on `Database`, inject it | Point dependencies at abstractions |
| **LSP rules** | Override breaks pre/post/signature/invariant | Honor the base contract | Demand no more, promise no less |

> **Remember:** ISP and DIP are about **the shape and direction of dependencies** — keep interfaces *small* (ISP) and make high-level code depend on *abstractions* (DIP). The LSP rules are the fine print that guarantees those abstractions can actually be trusted: **weaken pre-conditions, strengthen post-conditions.** 🧩

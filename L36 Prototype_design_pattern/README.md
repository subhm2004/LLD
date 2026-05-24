# Prototype Design Pattern — Detailed Guide

> **Creational Pattern** jo naye objects **clone** karke banata hai — expensive setup (DB, calc) **ek baar** template par, phir `clone()` se copies. **Copy constructor** + virtual `clone()` = prototype.

**Domain example (is repo mein):** Game `NPC` templates — `WithoutPrototype.cpp` vs `PrototypePattern.cpp`.

---

## Table of Contents

1. [Problem — Rebuild from scratch](#1-problem--rebuild-from-scratch)
2. [Prototype Pattern kya hai?](#2-prototype-pattern-kya-hai)
3. [Shallow vs Deep Copy](#3-shallow-vs-deep-copy)
4. [Code Walkthrough](#4-code-walkthrough)
5. [Build & Run](#5-build--run)
6. [vs Factory & Summary](#6-vs-factory--summary)

---

## 1. Problem — Rebuild from scratch

`WithoutPrototype.cpp`:

```cpp
NPC* alien = new NPC("Alien", 30, 5, 2);       // heavy setup
NPC* alien2 = new NPC("Powerful Alien", 50, 5, 5);  // repeat ALL work
// 100 enemies = 100× DB + calc
```

Easy to **mistype stats**; no shared template.

---

## 2. Prototype Pattern kya hai?

```cpp
class Cloneable {
public:
    virtual Cloneable* clone() const = 0;
};

NPC* template = new NPC("Alien", 30, 5, 2);  // once
NPC* copy = dynamic_cast<NPC*>(template->clone());
copy->setHealth(50);  // tweak clone only
```

| Piece | Role |
| ----- | ---- |
| **Prototype interface** | `Cloneable::clone()` |
| **Concrete prototype** | `NPC` — copy ctor + clone |
| **Client** | Clone + mutate instead of `new` full setup |

---

## 3. Shallow vs Deep Copy

| Copy | When |
| ---- | ---- |
| **Shallow** | All fields value types or immutable |
| **Deep** | Pointer members — clone pointed objects too |

Is demo mein primitives + `string` — shallow copy ctor sufficient. Production NPC with `vector<Skill>*` → deep clone skills.

---

## 4. Code Walkthrough

Source: [`C++ Code/PrototypePattern.cpp`](./C%20%2B%2B%20Code/PrototypePattern.cpp)

```cpp
NPC(const NPC& other) {
    name = other.name; health = other.health;
    cout << "Cloning NPC '" << name << "'\n";
}

Cloneable* clone() const override {
    return new NPC(*this);
}
```

**Output sequence:** `Setting up template` once → `Cloning` per copy.

---

## 5. Build & Run

```bash
cd "L36 Prototype_design_pattern/C++ Code"
g++ -std=c++17 -o proto_demo PrototypePattern.cpp && ./proto_demo
g++ -std=c++17 -o without_demo WithoutPrototype.cpp && ./without_demo
```

---

## 6. vs Factory & Summary

| Prototype | Factory |
| --------- | ------- |
| **Copy existing** | **Construct new** from rules |
| Known good instance | Type string / config driven |

**C++ note:** `= default` copy, `std::clone` (C++23), or explicit copy ctor.

| Pehlu | Detail |
| ----- | ------ |
| **Type** | Creational |
| **Files** | `PrototypePattern.cpp`, `WithoutPrototype.cpp` |

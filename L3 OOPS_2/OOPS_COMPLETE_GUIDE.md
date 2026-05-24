# OOP Complete Guide — Part 2 (L3): Inheritance + Polymorphism

<p align="center">
  <img src="https://img.shields.io/badge/Lesson-L3-purple?style=for-the-badge" />
  <img src="https://img.shields.io/badge/Pillars-4%2F4-success?style=for-the-badge" />
  <img src="https://img.shields.io/badge/C%2B%2B-17-informational?style=for-the-badge" />
</p>

> **Part 2** — 4 pillars complete. Pehle [L2 OOPS_COMPLETE_GUIDE](../L2%20OOPS_1/OOPS_COMPLETE_GUIDE.md) (fundamentals + encapsulation + abstraction).

---

## Table of Contents

1. [4 Pillars — L3 Role](#1-4-pillars--l3-role)
2. [Inheritance — Full Detail](#2-inheritance--full-detail)
3. [Types of Inheritance (C++)](#3-types-of-inheritance-c)
4. [Composition vs Inheritance](#4-composition-vs-inheritance)
5. [Static Polymorphism (Overloading)](#5-static-polymorphism-overloading)
6. [Dynamic Polymorphism (Overriding)](#6-dynamic-polymorphism-overriding)
7. [vtable & Virtual Destructor](#7-vtable--virtual-destructor)
8. [Overloading vs Overriding](#8-overloading-vs-overriding)
9. [Advanced — Full Guide Link](#9-advanced--full-guide-link)
10. [Code Files Map](#10-code-files-map)
11. [Interview Question Bank (L3)](#11-interview-question-bank-l3)
12. [Cheat Sheet + Learning Path](#12-cheat-sheet--learning-path)

---

## 1. 4 Pillars — L3 Role

| Pillar | L2 | L3 |
| ------ | -- | -- |
| Encapsulation | ✅ | `protected` for children |
| Abstraction | ✅ | pure virtual + override |
| **Inheritance** | — | ✅ **IS-A** |
| **Polymorphism** | — | ✅ static + dynamic |

---

## 2. Inheritance — Full Detail

### Definition

Child **inherits** parent variables + methods + adds **specific** behaviour. **Code reuse.**

```cpp
class ManualCar : public Car {
public:
    ManualCar(string b, string m) : Car(b, m) { currentGear = 0; }
    void shiftGear(int g) { ... }  // only manual
};
```

### IS-A test

`ManualCar` **is a** `Car` ✅ | `Car` **is a** `Engine` ❌ → use composition.

### Constructor chaining

1. Parent ctor runs first (`Car(b,m)`)  
2. Then child body  

### `protected`

Parent fields child access kar sake, **outside** nahi.

**Code:** [`01_Inheritance.cpp`](./C%20%2B%2B%20Code/01_Inheritance.cpp)

---

## 3. Types of Inheritance (C++)

| Type | Diagram | Example |
| ---- | ------- | ------- |
| **Single** | A → B | `ManualCar : Car` |
| **Multilevel** | A → B → C | L4 fruit chain |
| **Multiple** | A,B → C | Rare in LLD |
| **Hierarchical** | A → B, A → C | `ManualCar`, `ElectricCar` : `Car` |
| **Hybrid** | Mix | — |

**Default choice in LLD:** `public` inheritance.

| Mode | public parent → in child |
| ---- | ------------------------ |
| `public` | public |
| `protected` | protected |
| `private` | private |

---

## 4. Composition vs Inheritance

| | Inheritance (IS-A) | Composition (HAS-A) |
| - | ------------------ | ------------------- |
| **Relation** | is a | has a |
| **Example** | `ManualCar is Car` | `Car has Engine` |
| **Coupling** | Tighter | Looser, flexible |
| **Prefer** | true subtype | reuse without hierarchy |

```cpp
class Car {
    Engine engine;  // HAS-A — composition
};
```

**Interview:** "Favour composition over inheritance" — when behaviour reuse without IS-A.

**Code:** [`05_Composition_Vs_Inheritance.cpp`](./C%20%2B%2B%20Code/05_Composition_Vs_Inheritance.cpp)  
**More:** [L4 INHERITANCE_AND_COMPOSITION](../L4%20UML_Diagrams/INHERITANCE_AND_COMPOSITION.md)

---

## 5. Static Polymorphism (Overloading)

**Compile-time** — same function name, **different parameters**.

```cpp
void accelerate();           // +20
void accelerate(int speed);  // +speed
```

| Rule | Detail |
| ---- | ------ |
| Same name | ✅ |
| Different param list | ✅ |
| Return type only | ❌ not enough |

**Operator overloading** bhi isi family me.

**Code:** [`02_Static_Polymorphism.cpp`](./C%20%2B%2B%20Code/02_Static_Polymorphism.cpp)

---

## 6. Dynamic Polymorphism (Overriding)

**Runtime** — base pointer, **child** method runs.

```cpp
class Car {
public:
    virtual void accelerate() = 0;
    virtual ~Car() {}
};
class ElectricCar : public Car {
    void accelerate() override { /* battery logic */ }
};
Car* c = new ElectricCar("Tesla", "S");
c->accelerate();  // ElectricCar version
```

### `virtual` / `override` / `final`

| Keyword | Purpose |
| ------- | ------- |
| `virtual` | Enable dynamic dispatch |
| `override` | Compiler check correct override |
| `final` | No further override |

### Without `virtual`

```cpp
Car* p = new ElectricCar(...);
p->accelerate();  // ❌ binds to Car if non-virtual
```

**Code:** [`03_Dynamic_Polymorphism.cpp`](./C%20%2B%2B%20Code/03_Dynamic_Polymorphism.cpp)

---

## 7. vtable & Virtual Destructor

```
object → vptr → vtable → ElectricCar::accelerate
```

| Term | Meaning |
| ---- | ------- |
| **vtable** | Per-class virtual function table |
| **vptr** | Hidden pointer in object |

**Virtual destructor (mandatory for poly base):**

```cpp
Car* p = new ElectricCar(...);
delete p;  // ~ElectricCar then ~Car — without virtual → UB/leak
```

---

## 8. Overloading vs Overriding

| | Overloading | Overriding |
| - | ----------- | ---------- |
| **Time** | Compile | Runtime |
| **Class** | Usually same | Parent + child |
| **Signature** | Must differ | Same as virtual parent |
| **Keyword** | none | virtual/override |

**Both:** [`04_Static_And_Dynamic_Polymorphism.cpp`](./C%20%2B%2B%20Code/04_Static_And_Dynamic_Polymorphism.cpp)

---

## 9. Advanced — Full Guide Link

**Must read for interviews:** [`OOPS_ADVANCED_INHERITANCE.md`](./OOPS_ADVANCED_INHERITANCE.md)

- Virtual function & **vtable** / vptr  
- **Virtual destructor** (why mandatory)  
- **Diamond problem** + **virtual inheritance**  
- **Overloading vs overriding** (full table)  

Memory / RAII / smart pointers: [L2 OOPS_ADVANCED_CPP](../L2%20OOPS_1/OOPS_ADVANCED_CPP.md)

---

## 10. Code Files Map

```bash
cd "L3 OOPS_2" && ./compile.sh
./bin/08_Diamond_Problem
./bin/06_Virtual_Destructor
```

| File | Learn |
| ---- | ----- |
| `01_Inheritance.cpp` | protected, hierarchical |
| `02_Static_Polymorphism.cpp` | overload |
| `03_Dynamic_Polymorphism.cpp` | virtual dispatch |
| `04_Static_And_Dynamic_Polymorphism.cpp` | combined |
| `05_Composition_Vs_Inheritance.cpp` | HAS-A |
| `06_Virtual_Destructor.cpp` | non-virtual vs virtual delete |
| `07_Virtual_Table_Demo.cpp` | runtime dispatch |
| `08_Diamond_Problem.cpp` | diamond + `virtual` base |
| `09_Overloading_Vs_Overriding.cpp` | static vs dynamic |

---

## 11. Interview Question Bank (L3)

<details><summary><b>Inheritance?</b></summary>IS-A; reuse; child extends parent.</details>

<details><summary><b>Static vs dynamic poly?</b></summary>Overload compile-time; override virtual runtime.</details>

<details><summary><b>virtual keyword?</b></summary>Runtime binding via vtable when called through base pointer.</details>

<details><summary><b>Pure virtual vs virtual?</b></summary>=0 abstract, must override; virtual with body optional override.</details>

<details><summary><b>protected vs private?</b></summary>protected = child OK; private = only own class.</details>

<details><summary><b>Composition vs inheritance?</b></summary>HAS-A vs IS-A; prefer composition when not true subtype.</details>

<details><summary><b>virtual destructor?</b></summary>Safe delete through base pointer.</details>

<details><summary><b>Multiple inheritance issue?</b></summary>Diamond problem; ambiguity — prefer composition/interface.</details>

---

## 12. Cheat Sheet + Learning Path

```
INHERITANCE      class Child : public Parent
                 Child(args) : Parent(args) { }

COMPOSITION      class Car { Engine e; };

STATIC POLY      overload — same name, diff params

DYNAMIC POLY     virtual + override
                 Base* p = new Derived(); p->f();

virtual ~Base()  always for polymorphic hierarchy
= 0               pure virtual → abstract class
```

```
L2 Fundamentals + Encap + Abstraction
        ↓
L3 Inheritance + Polymorphism  ← you are here
        ↓
L4 UML → L5/L6 SOLID → L8+ Patterns
```

⬅️ [L2 Complete Guide](../L2%20OOPS_1/OOPS_COMPLETE_GUIDE.md) · [L2 Advanced C++](../L2%20OOPS_1/OOPS_ADVANCED_CPP.md) · [L3 Advanced Inheritance](./OOPS_ADVANCED_INHERITANCE.md)

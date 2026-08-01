# Virtual Base Class — Diamond Problem (Advanced)

> **Basics:** [`08_Diamond_Problem.cpp`](./C%20%2B%2B%20Code/08_Diamond_Problem.cpp) · [`MULTIPLE_INHERITANCE_AMBIGUITY.md`](./MULTIPLE_INHERITANCE_AMBIGUITY.md)  
> **Advanced demo:** [`17_Virtual_Base_Class_Advanced.cpp`](./C%20%2B%2B%20Code/17_Virtual_Base_Class_Advanced.cpp)

---

## Table of Contents

1. [Recap — diamond problem](#1-recap--diamond-problem)
2. [Virtual base = shared subobject](#2-virtual-base--shared-subobject)
3. [Who constructs the virtual base?](#3-who-constructs-the-virtual-base)
4. [Construction & destruction order](#4-construction--destruction-order)
5. [Memory layout & vbptr](#5-memory-layout--vbptr)
6. [Costs & when to use](#6-costs--when-to-use)
7. [Interview Q&A](#7-interview-qa)

---

## 1. Recap — diamond problem

```
        Device
       /      \
   Printer    Scanner
       \      /
    MultiFunction
```

Without `virtual`, `MultiFunction` has **two** `Device` subobjects → ambiguous `id`, wasted memory.

---

## 2. Virtual base = shared subobject

```cpp
class Printer : public virtual Device { };
class Scanner : public virtual Device { };
class MultiFunction : public Printer, public Scanner { };
```

| | Normal MI | `virtual` base |
|---|-----------|----------------|
| `Device` copies in most-derived | 2 | **1** |
| `mf.id = "x"` | Ambiguous | **One field** |

---

## 3. Who constructs the virtual base?

**Rule (C++):** The **most-derived class** is responsible for calling the **virtual base** constructor.

```cpp
class MultiFunction : public Printer, public Scanner {
public:
    MultiFunction()
        : Device("shared-MF-device"),  // ✅ ONLY this runs for virtual Device
          Printer(),
          Scanner() { }
};
```

| Constructor tries | Result |
|-------------------|--------|
| `Printer` ctor calls `Device("x")` | **Ignored** for virtual base if `MultiFunction` lists `Device(...)` |
| `MultiFunction` omits `Device(...)` | `Device` default ctor used (if exists) |

**Interview line:** Middle classes **must not** assume they alone own the virtual base — most-derived initializes it.

---

## 4. Construction & destruction order

### Construction

1. **Virtual base classes** (declaration order in most-derived)
2. **Direct non-virtual bases** (left-to-right)
3. **Member objects** (declaration order)
4. **Most-derived constructor body**

### Destruction

**Exact reverse** of construction.

Demo output (`17_Virtual_Base_Class_Advanced.cpp`):

```
[Device] ctor
[Printer] ctor
[Scanner] ctor
[MultiFunction] ctor
...
~Printer → ~Scanner → ~Device
```

---

## 5. Memory layout & vbptr

Compiler often adds a **hidden pointer** (vbptr) so `Printer` / `Scanner` parts can locate the **shared** `Device` subobject at runtime.

| Effect | Detail |
|--------|--------|
| `sizeof(MultiFunction)` | Usually **larger** than naive single inheritance |
| Access to virtual base member | May need **extra indirection** |
| Compare | `JoinBad` (2× small base) vs `JoinGood` (1× virtual base) |

**Implementation-defined** — don't memorize exact bytes; know **concept** for interviews.

---

## 6. Costs & when to use

| Use virtual inheritance | Prefer composition instead |
|-------------------------|----------------------------|
| Legacy MI hierarchies (streams, widgets) | New LLD systems |
| Must share one base state in diamond | "Has-a" with single owner object |

**Modern LLD:** `MultiFunction` **has** `Printer` + `Scanner` members — no diamond, no virtual base.

---

## 7. Interview Q&A

<details>
<summary><strong>virtual inheritance kis par lagate hain?</strong></summary>

**Shared common base** (`Device`) — middle classes: `class Printer : public virtual Device`.

</details>

<details>
<summary><strong>Duplicate ctor calls for virtual base?</strong></summary>

Only **most-derived** ctor's `Device(...)` in mem-initializer list runs for the virtual subobject.

</details>

<details>
<summary><strong>virtual base vs virtual function?</strong></summary>

Unrelated. **virtual function** = runtime dispatch. **virtual inheritance** = shared base subobject in MI.

</details>

---

## Cheat sheet

```
virtual public Base     on middle classes in diamond
Most-derived ctor         must initialize virtual Base
Order                     virtual bases → bases → members → body
Cost                      vbptr + indirection; prefer composition in new design
```

---

⬅️ [MI Ambiguity](./MULTIPLE_INHERITANCE_AMBIGUITY.md) · ➡️ [Covariant Returns](./COVARIANT_RETURN_TYPES.md)

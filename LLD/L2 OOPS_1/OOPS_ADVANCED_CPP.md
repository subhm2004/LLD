# Advanced C++ for OOP Interviews (L2) — Memory, RAII, Smart Ptr, Move, Rules

> Runnable code in [`C++ Code/`](./C%20%2B%2B%20Code/) files **08–14**.  
> Inheritance advanced: [L3 OOPS_ADVANCED_INHERITANCE](../L3%20OOPS_2/OOPS_ADVANCED_INHERITANCE.md)

---

## Table of Contents

1. [Shallow Copy vs Deep Copy](#1-shallow-copy-vs-deep-copy)
2. [Operator Overloading](#2-operator-overloading)
3. [new vs malloc vs calloc](#3-new-vs-malloc-vs-calloc)
4. [RAII](#4-raii)
5. [Smart Pointers](#5-smart-pointers)
6. [Move Semantics & rvalue](#6-move-semantics--rvalue)
7. [Rule of 3 / 5 / 0](#7-rule-of-3--5--0)
8. [Conversion Functions](#8-conversion-functions)
9. [Object Pool Pattern](#9-object-pool-pattern)
10. [Interview Q&A](#10-interview-qa)

---

## 1. Shallow Copy vs Deep Copy

| | Shallow | Deep |
| - | ------- | ---- |
| **Kya copy hota hai** | Pointer **address** copy | **Naya heap** + content copy |
| **Risk** | Double-free, dangling | Safe independent objects |
| **Default** | Compiler copy ctor (pointers) | Custom copy ctor/assignment |

```cpp
int* q = p;           // shallow
DeepString b = a;     // deep — separate buffer
```

**Code:** [`10_Shallow_Deep_Copy.cpp`](./C%20%2B%2B%20Code/10_Shallow_Deep_Copy.cpp)

**Interview:** Class me raw pointer ho → define **Rule of Three** (deep copy + dtor).

---

## 2. Operator Overloading

Compile-time — **same operator, different operands**.

| Form | Example |
| ---- | ------- |
| Member | `Complex operator+(const Complex& o) const` |
| Friend | `friend ostream& operator<<(ostream&, const Complex&)` |

**Rules:** cannot overload `::`, `.*`, `?:`, `sizeof`; at least one user-defined type.

**Code:** [`11_Operator_Overloading.cpp`](./C%20%2B%2B%20Code/11_Operator_Overloading.cpp)

---

## 3. new vs malloc vs calloc

| | `new` / `delete` | `malloc` / `free` | `calloc` |
| - | ---------------- | ----------------- | -------- |
| **Language** | C++ | C | C |
| **Constructor** | ✅ calls ctor | ❌ raw memory | ❌ zero-init, no ctor |
| **Type** | Typed `T*` | `void*` → cast | `void*` → cast |
| **Failure** | `bad_alloc` (default) | `nullptr` | `nullptr` |
| **Size** | `new T[n]` | `malloc(n*sizeof)` | `calloc(count, size)` |

```cpp
Pod* p = new Pod{1, 2.0};   // ctor runs
delete p;

void* m = malloc(sizeof(Pod));  // no ctor
free(m);

int* z = calloc(10, sizeof(int));  // all bytes 0
free(z);
```

**Code:** [`12_New_Malloc_Calloc.cpp`](./C%20%2B%2B%20Code/12_New_Malloc_Calloc.cpp)

**Interview:** C++ code me prefer **new/delete** or better **smart pointers**; malloc jab C interop ho.

---

## 4. RAII

**Resource Acquisition Is Initialization** — resource **constructor** me acquire, **destructor** me release.

| Resource | RAII wrapper |
| -------- | ------------ |
| Heap | `unique_ptr` |
| FILE* | `FileGuard` / `fstream` |
| mutex | `lock_guard`, `unique_lock` |

```cpp
{
    FileGuard f("x.txt", "w");
    f.write("data");
}  // fclose in dtor — even if exception
```

**Code:** [`13_RAII.cpp`](./C%20%2B%2B%20Code/13_RAII.cpp)

**Interview:** RAII = C++ ka #1 memory/safety idea; exceptions safe cleanup.

---

## 5. Smart Pointers

| Type | Ownership | Copy? | Use |
| ---- | --------- | ----- | --- |
| **unique_ptr** | Exclusive | ❌ move only | default choice |
| **shared_ptr** | Shared count | ✅ | shared ownership |
| **weak_ptr** | None (observe) | ✅ from shared | break cycles |

```cpp
auto u = make_unique<Widget>(1);
auto s = make_shared<Widget>(2);
weak_ptr<Widget> w = s;
```

**Code:** [`14_Smart_Pointers.cpp`](./C%20%2B%2B%20Code/14_Smart_Pointers.cpp)

**Cycle:** A↔B both `shared_ptr` → leak; fix with `weak_ptr` on one side.

---

## 6. Move Semantics & rvalue

| | lvalue | rvalue |
| - | ------ | ------ |
| **Identity** | Named, persistent | Temporary, about to die |
| **Example** | `x` | `42`, return value, `std::move(x)` |

**Move ctor** — **steal** resources, leave source empty/null.

```cpp
Buffer(Buffer&& o) noexcept : data(o.data), n(o.n) {
    o.data = nullptr;
}
Buffer b = std::move(a);
```

`std::move` = cast to rvalue — **does not move by itself**, enables move ctor/assignment.

**Code:** [`15_Move_Semantics.cpp`](./C%20%2B%2B%20Code/15_Move_Semantics.cpp)

---

## 7. Rule of 3 / 5 / 0

| Rule | When | What to define |
| ---- | ---- | -------------- |
| **Rule of Three** | Raw pointer/resource | dtor, copy ctor, copy `operator=` |
| **Rule of Five** | C++11 + resource | + move ctor, move `operator=` |
| **Rule of Zero** | Members handle resources (`string`, `vector`, `unique_ptr`) | **define none** — compiler defaults OK |

```cpp
class RuleOfZero {
    string name;
    vector<int> v;  // manage memory for you
};
```

**Code:** [`16_Rule_Of_Three_Five_Zero.cpp`](./C%20%2B%2B%20Code/16_Rule_Of_Three_Five_Zero.cpp) · also [`02_Constructors_Destructors.cpp`](./C%20%2B%2B%20Code/02_Constructors_Destructors.cpp)

---

## 8. Conversion Functions

| | Implicit | `explicit` |
| - | -------- | ---------- |
| Single-arg ctor | `Meters m = 5;` allowed | Only `Meters m(5)` or `static_cast` |
| Conversion operator | `double x = frac;` | `explicit operator bool()` |

**Code:** [`18_Conversion_Functions.cpp`](./C%20%2B%2B%20Code/18_Conversion_Functions.cpp)  
**Full guide:** [`CONVERSION_FUNCTIONS.md`](./CONVERSION_FUNCTIONS.md)

---

## 9. Object Pool Pattern

Pre-allocate objects → `acquire()` / `release()` — **no delete per use**; cuts allocation churn.

**Code:** [`19_Object_Pool_Pattern.cpp`](./C%20%2B%2B%20Code/19_Object_Pool_Pattern.cpp)  
**Full guide:** [`OBJECT_POOL_PATTERN.md`](./OBJECT_POOL_PATTERN.md)

---

## 10. Interview Q&A

<details><summary>Shallow vs deep?</summary>Shallow shares pointer; deep duplicates heap. Raw pointer class needs deep copy.</details>

<details><summary>RAII?</summary>Ctor acquires, dtor releases; scope-bound cleanup.</details>

<details><summary>unique vs shared?</summary>unique exclusive; shared refcount; weak non-owning.</details>

<details><summary>std::move?</summary>Cast to rvalue to invoke move operations; source may be empty after.</details>

<details><summary>Rule of zero?</summary>Use smart containers — no custom big three/five.</details>

<details><summary>new vs malloc?</summary>new calls ctor, typed, C++; malloc raw C bytes.</details>

---

➡️ [L3 Advanced Inheritance](../L3%20OOPS_2/OOPS_ADVANCED_INHERITANCE.md) — virtual, vtable, diamond, overload vs override

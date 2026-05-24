# Conversion Functions — Implicit vs `explicit` (Complete Guide)

> **Code:** [`C++ Code/18_Conversion_Functions.cpp`](./C%20%2B%2B%20Code/18_Conversion_Functions.cpp)

---

## Table of Contents

1. [Do conversion functions kya hain](#1-do-conversion-functions-kya-hain)
2. [Implicit conversion — compiler khud karta hai](#2-implicit-conversion--compiler-khud-karta-hai)
3. [`explicit` keyword](#3-explicit-keyword)
4. [Conversion operators](#4-conversion-operators)
5. [Interview traps](#5-interview-traps)
6. [Cheat sheet](#6-cheat-sheet)

---

## 1. Do conversion functions kya hain

C++ objects ko **dusre types** me convert karne ke 2 common tareeke:

| Direction | Mechanism | Example |
|-----------|-----------|---------|
| **T → MyClass** | Single-argument constructor | `Meters(5.0)` from `double` |
| **MyClass → T** | Conversion operator | `operator double()` |

Dono **user-defined conversions** hain — compiler inhe automatic apply kar sakta hai.

---

## 2. Implicit conversion — compiler khud karta hai

```cpp
class Meters {
public:
    Meters(double m);  // NOT explicit
};

Meters m = 10;           // OK — double → Meters
void f(Meters);
f(5.0);                  // OK — implicit conversion at call site
```

**Kahan useful:** literals / simple APIs jahan conversion obvious ho.

**Risk:** Accidental conversions — `f(0)` ambiguity, `if (ptr)` with `operator bool`, etc.

---

## 3. `explicit` keyword

### On constructors

```cpp
class MetersExplicit {
public:
    explicit MetersExplicit(double m);
};

// MetersExplicit m = 10;     // ❌ ERROR
MetersExplicit m(10);          // ✅ OK
MetersExplicit m2 = static_cast<MetersExplicit>(10);  // ✅ explicit cast
```

### On conversion operators (C++11)

```cpp
explicit operator bool() const;
```

| Without `explicit` | With `explicit` |
|------------------|-----------------|
| `if (obj)` compiles | Need `static_cast<bool>(obj)` |

**Rule (modern C++):** Single-arg ctors aur conversion operators par default **`explicit`** unless you really want implicit.

---

## 4. Conversion operators

```cpp
class Fraction {
    operator double() const;  // Fraction → double implicit
};

double x = half;   // calls operator double()
```

**`explicit operator T()`** — target type par implicit conversion band.

---

## 5. Interview traps

| Trap | Answer |
|------|--------|
| `explicit` copy ctor? | C++17: copy/move ctors cannot be `explicit` in some contexts; usually not needed |
| `explicit` + `initializer_list`? | `explicit` ctor blocks `T x = {a,b}` style |
| vs `static_cast` | `static_cast` = manual; `explicit` = compiler won't auto-convert |
| Smart pointers | `explicit` single-arg ctors — `unique_ptr` from raw pointer |

---

## 6. Cheat sheet

```
IMPLICIT     single-arg ctor without explicit
             conversion operator without explicit

explicit     blocks implicit conversion
             explicit operator bool() — no silent if(ptr)

FIX          static_cast<MyType>(value)
```

---

⬅️ [L2 OOPS_ADVANCED_CPP](./OOPS_ADVANCED_CPP.md) · ➡️ [L3 RTTI](../L3%20OOPS_2/RTTI_COMPLETE.md)

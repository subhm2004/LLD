# Runtime Type Information (RTTI) — `typeid`, `dynamic_cast`, Enable/Disable

> **Code:** [`C++ Code/16_RTTI_Typeid_Dynamic_Cast.cpp`](./C%20%2B%2B%20Code/16_RTTI_Typeid_Dynamic_Cast.cpp)  
> **Related:** [`13_Upcasting_Downcasting.cpp`](./C%20%2B%2B%20Code/13_Upcasting_Downcasting.cpp)

---

## Table of Contents

1. [RTTI kya hai](#1-rtti-kya-hai)
2. [`typeid()`](#2-typeid)
3. [`dynamic_cast` — RTTI use karta hai](#3-dynamic_cast--rtti-use-karta-hai)
4. [Polymorphic class zaroori](#4-polymorphic-class-zaroori)
5. [RTTI enable / disable](#5-rtti-enable--disable)
6. [Alternatives](#6-alternatives)
7. [Interview Q&A](#7-interview-qa)

---

## 1. RTTI kya hai

**Runtime Type Information** = program **runtime** par object ka **actual dynamic type** inspect kar sake.

| Facility | RTTI use? |
|----------|-----------|
| `typeid(expr)` | ✅ (polymorphic pointee ke liye dynamic) |
| `dynamic_cast` | ✅ |
| `static_cast` | ❌ compile-time only |
| Virtual functions | vtable — related but not full RTTI |

---

## 2. `typeid()`

```cpp
Dog d;
Animal* p = &d;

typeid(d).name();     // Dog (implementation-defined mangled name)
typeid(*p).name();    // Dog — dereference → dynamic type
typeid(p).name();     // Animal* — pointer ka static type
```

| Expression | Usually gives |
|------------|---------------|
| `typeid(obj)` | Static type of `obj` |
| `typeid(*ptr)` on polymorphic | **Dynamic** (actual object) type |
| `typeid(ptr)` | Static type of pointer |

```cpp
if (typeid(*a) == typeid(Dog)) { /* ... */ }
```

**Note:** `.name()` compiler-specific (`"6Circle"` vs demangled). Production me often `dynamic_cast` prefer.

---

## 3. `dynamic_cast` — RTTI use karta hai

```cpp
Animal* a = getAnimal();
if (Dog* d = dynamic_cast<Dog*>(a)) {
    d->fetch();
}
```

| Form | Failure |
|------|---------|
| Pointer | `nullptr` |
| Reference | `std::bad_cast` exception |

**Requires:** base class me **at least one `virtual`** function (polymorphic type).

---

## 4. Polymorphic class zaroori

```cpp
class Base {
    virtual void foo();  // or virtual ~Base()
};
```

Bina virtual ke hierarchy par `dynamic_cast` / dynamic `typeid` **reliable nahi**.

Non-polymorphic:

```cpp
typeid(plain).name();  // static type only
```

---

## 5. RTTI enable / disable

### Default

| Compiler | Default |
|----------|---------|
| g++ / clang | **RTTI ON** (`-frtti` implicit) |

### Disable

```bash
g++ -std=c++17 -fno-rtti main.cpp
```

| Effect |
|--------|
| `dynamic_cast` on polymorphic types — typically **compile error** |
| `typeid` on polymorphic — restricted |
| Binary **smaller** — embedded / game engines |

### Check in code

```cpp
#if defined(__GXX_RTTI) || defined(_CPPRTTI)
// RTTI enabled
#endif
```

**When to disable:** Size/speed critical, no runtime downcast; use `std::variant`, tagged enum, Visitor.

---

## 6. Alternatives

| Approach | When |
|----------|------|
| Virtual method | Behaviour vary — no type check |
| `std::variant` + `std::visit` | Closed set of types |
| `enum class Type` + switch | Manual dispatch |
| `static_cast` | **Proven** type at compile time |

---

## 7. Interview Q&A

<details>
<summary><strong>RTTI ka cost?</strong></summary>

vtable + type info; `dynamic_cast` slower than `static_cast`; `-fno-rtti` saves binary size.

</details>

<details>
<summary><strong>typeid vs dynamic_cast?</strong></summary>

`typeid` = type compare/name. `dynamic_cast` = safe pointer/reference conversion.

</details>

<details>
<summary><strong>LSP violation typeid check?</strong></summary>

`if (typeid(*acc) == typeid(FixedTerm))` — anti-pattern; use virtual behaviour instead.

</details>

---

## Cheat sheet

```
RTTI ON (default)     typeid, dynamic_cast on polymorphic types
RTTI OFF              g++ -fno-rtti

typeid(*p)            dynamic type of pointee
dynamic_cast<T*>(p)   nullptr if wrong type

Need virtual in base for dynamic_cast
```

---

⬅️ [MI Ambiguity](./MULTIPLE_INHERITANCE_AMBIGUITY.md) · ➡️ [Conversion Functions (L2)](../L2%20OOPS_1/CONVERSION_FUNCTIONS.md)

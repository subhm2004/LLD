# Covariant Return Types — Override with Different Return Type

> **Code:** [`18_Covariant_Return_Types.cpp`](./C%20%2B%2B%20Code/18_Covariant_Return_Types.cpp)

---

## Table of Contents

1. [Normal override rule](#1-normal-override-rule)
2. [Covariant return — exception](#2-covariant-return--exception)
3. [Pointer vs reference](#3-pointer-vs-reference)
4. [Clone / factory pattern](#4-clone--factory-pattern)
5. [Limits](#5-limits)
6. [Interview Q&A](#6-interview-qa)

---

## 1. Normal override rule

Overridden virtual function must match:

- Name
- Parameter list (const/ref qualifiers matter)
- **Return type** — usually **exact same**

```cpp
class Base  { virtual void f(); };
class Derived { void f() override; };  // same return void
```

---

## 2. Covariant return — exception

C++ allows **covariant** return type in override:

> Return type is pointer or reference to a class **derived from** the base return type.

```cpp
class Document {
public:
    virtual Document* clone() const;
};

class PdfDocument : public Document {
public:
    PdfDocument* clone() const override;  // ✅ covariant
};
```

| Base returns | Derived may return |
|--------------|-------------------|
| `Document*` | `PdfDocument*` |
| `Document&` | `PdfDocument&` |
| `Document` by value | ❌ **Not allowed** (different sizes) |

**Why "covariant":** Return type varies **with** the class (`Document*` → `PdfDocument*`).

---

## 3. Pointer vs reference

```cpp
PdfDocument* clone() const override;   // common — factories

// Reference form also valid:
// PdfDocument& factory() override;  // if base returns Document&
```

Calling through base:

```cpp
Document* p = new PdfDocument();
Document* copy = p->clone();           // still Document* statically
PdfDocument* pdf = dynamic_cast<PdfDocument*>(p->clone());  // if need derived API
```

Direct on derived object:

```cpp
PdfDocument pdf;
PdfDocument* c = pdf.clone();          // ✅ no cast — compile-time type
```

---

## 4. Clone / factory pattern

```cpp
virtual Document* clone() const = 0;  // prototype-style copy
```

Each derived returns **its own type** — caller holding `PdfDocument` can `clone()` without losing type information when using derived reference.

**Related patterns:** Prototype (L36), virtual copy constructors idiom.

---

## 5. Limits

| Allowed | Not allowed |
|---------|-------------|
| `Derived*` from `Base*` | `int` from `void` (unrelated types) |
| `Derived&` from `Base&` | `unique_ptr<Derived>` covariant with `unique_ptr<Base>` — **special rules** (not classic covariant; use different design) |
| One level covariant | Arbitrary unrelated pointers |

**C++11 `override`** still checks covariant compatibility.

---

## 6. Interview Q&A

<details>
<summary><strong>Override me alag return type?</strong></summary>

Sirf **covariant** pointer/reference to derived class — e.g. `Base*` → `Derived*`.

</details>

<details>
<summary><strong>By-value covariant?</strong></summary>

No — object slicing / size mismatch.

</details>

<details>
<summary><strong>vs dynamic_cast after clone?</strong></summary>

Covariant `clone()` on **derived** reference avoids cast; through `Base*` you still often store as base pointer.

</details>

---

## Cheat sheet

```
COVARIANT     Derived* / Derived&  overrides  Base* / Base&
USE CASE      clone(), typed factories
NOT ALLOWED   by-value return, unrelated types
```

---

⬅️ [Virtual Base Advanced](./VIRTUAL_BASE_CLASS_ADVANCED.md) · ➡️ [L2 Object Pool](../L2%20OOPS_1/OBJECT_POOL_PATTERN.md)

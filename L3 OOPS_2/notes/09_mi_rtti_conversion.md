# Multiple Inheritance, RTTI, Virtual Base & Covariant Returns

> **EN:** MI scope resolution, RTTI casts, virtual base construction, covariant `clone()`.

> **Runnable demo:** [`15_Multiple_Inheritance_Ambiguity.cpp`](../C++ Code/15_Multiple_Inheritance_Ambiguity.cpp)
> **Runnable demo:** [`16_RTTI_Typeid_Dynamic_Cast.cpp`](../C++ Code/16_RTTI_Typeid_Dynamic_Cast.cpp)
> **Runnable demo:** [`17_Virtual_Base_Class_Advanced.cpp`](../C++ Code/17_Virtual_Base_Class_Advanced.cpp)
> **Runnable demo:** [`18_Covariant_Return_Types.cpp`](../C++ Code/18_Covariant_Return_Types.cpp)
> **Parent guides:** [MULTIPLE_INHERITANCE_AMBIGUITY](../MULTIPLE_INHERITANCE_AMBIGUITY.md) · [RTTI_COMPLETE](../RTTI_COMPLETE.md) · [VIRTUAL_BASE_CLASS_ADVANCED](../VIRTUAL_BASE_CLASS_ADVANCED.md) · [COVARIANT_RETURN_TYPES](../COVARIANT_RETURN_TYPES.md)

---

## Table of Contents

1. [MI ambiguity](#1-mi)
2. [Diamond fix](#2-diamond)
3. [RTTI](#3-rtti)
4. [Virtual base advanced](#4-vbase)
5. [Covariant](#5-cov)
6. [Parent guides](#6-guides)
7. [Interview Q&A](#7-qa)
8. [Cheat sheet](#8-cheat)

## 1. MI ambiguity

<a id="1-mi-ambiguity"></a>

`AllInOne : Printer, Scanner` — `print()` ambiguous → `Printer::print()`.
## 2. Diamond fix

<a id="2-diamond-fix"></a>

`TeachingAssistant` — two `Person` without virtual; `TAVirtual` — one `PersonV` with `virtual` inheritance.
## 3. RTTI

<a id="3-rtti"></a>

`typeid(*pDog)` dynamic type; `dynamic_cast<Dog*>` in loop — see `16_RTTI_Typeid_Dynamic_Cast.cpp`. `-fno-rtti` disables for embedded.
## 4. Virtual base advanced

<a id="4-virtual-base-advanced"></a>

`MultiFunction` must init shared `Device` — most-derived rule in `17_Virtual_Base_Class_Advanced.cpp`.
## 5. Covariant

<a id="5-covariant"></a>

```cpp
PdfDocument* clone() const override;  // from Document*
```

Factory `clone()` returns derived pointer without cast — `18_Covariant_Return_Types.cpp`.
## 6. Parent guides

<a id="6-parent-guides"></a>

| Topic | Deep guide |
|---|---|
| MI | ../MULTIPLE_INHERITANCE_AMBIGUITY.md |
| RTTI | ../RTTI_COMPLETE.md |
| Virtual base | ../VIRTUAL_BASE_CLASS_ADVANCED.md |
| Covariant | ../COVARIANT_RETURN_TYPES.md |

## 7. Interview Q&A

<a id="7-interview-q-a"></a>

<details>
<summary><strong>Resolve MI same name?</strong></summary>

Qualifier: `Base1::foo()` vs `Base2::foo()`.


</details>

<details>
<summary><strong>virtual base who constructs?</strong></summary>

Most derived class initializes virtual base.


</details>

<details>
<summary><strong>typeid *p vs *p?</strong></summary>

typeid(p) static pointer type; typeid(*p) dynamic object.


</details>

<details>
<summary><strong>Covariant return rule?</strong></summary>

Override may return pointer/ref to **derived** type.


</details>

## 8. Cheat sheet

<a id="8-cheat-sheet"></a>

```text
Printer::print() | virtual public Base | dynamic_cast | PdfDocument* clone()
```

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

### Run

./bin/15_Multiple_Inheritance_Ambiguity through 18_Covariant_Return_Types

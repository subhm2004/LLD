# Object Slicing, Upcasting & Downcasting

> **EN:** Value-copy slices derived; upcast safe; downcast with dynamic_cast. **HI:** `Animal a = dog` slice; upcast safe; downcast dynamic_cast.

> **Runnable demo:** [`12_Object_Slicing.cpp`](../C++ Code/12_Object_Slicing.cpp)
> **Runnable demo:** [`13_Upcasting_Downcasting.cpp`](../C++ Code/13_Upcasting_Downcasting.cpp)
> **Parent guides:** [RTTI_COMPLETE](../RTTI_COMPLETE.md)

---

## Table of Contents

1. [Slicing](#1-slice)
2. [12 demo](#2-12)
3. [Upcast](#3-up)
4. [Downcast](#4-down)
5. [13 demo](#5-13)
6. [Cast table](#6-cast)
7. [Interview Q&A](#7-qa)
8. [Cheat sheet](#8-cheat)

## 1. Slicing

<a id="1-slicing"></a>

```cpp
Dog d;
Animal sliced = d;  // loses breed + Dog vtable type
```

## 2. 12 demo

<a id="2-12-demo"></a>

Lambda by-value `Animal a` slices; `const Animal&` preserves `Dog::speak`.
## 3. Upcast

<a id="3-upcast"></a>

`Shape* p = &c` — implicit, always safe.
## 4. Downcast

<a id="4-downcast"></a>

`dynamic_cast<Circle*>(p)` — nullptr on failure; reference throws `bad_cast`.
## 5. 13 demo

<a id="5-13-demo"></a>

Shape array polymorphic draw; downcast for `Circle::area()` only when type matches.
## 6. Cast table

<a id="6-cast-table"></a>

| Cast | Runtime check | Downcast |
|---|---|---|
| dynamic_cast | yes | safe |
| static_cast | no | risky |
| reinterpret_cast | no | avoid |

## 7. Interview Q&A

<a id="7-interview-q-a"></a>

<details>
<summary><strong>What is object slicing?</strong></summary>

Copy derived into base by value — derived part dropped.

**हिंदी:** Value copy = slice.

</details>

<details>
<summary><strong>How to avoid slicing?</strong></summary>

Use pointer, reference, or smart pointer to base.

**हिंदी:** *& use karo.

</details>

<details>
<summary><strong>Upcasting safe?</strong></summary>

Always — implicit public inheritance.

**हिंदी:** Hamesha safe.

</details>

<details>
<summary><strong>static_cast downcast?</strong></summary>

No runtime check — UB if wrong.

**हिंदी:** Galat type = UB.

</details>

<details>
<summary><strong>dynamic_cast needs?</strong></summary>

Polymorphic base (≥1 virtual).

**हिंदी:** virtual chahiye.

</details>

## 8. Cheat sheet

<a id="8-cheat-sheet"></a>

```text
NO: Base b = derived;
YES: Base& / Base*
downcast: dynamic_cast + nullptr
```

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.

### Exception

catch (exception e) by value also slices — catch by const&.


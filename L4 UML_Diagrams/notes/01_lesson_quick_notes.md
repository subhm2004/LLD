# L4 — Lesson Quick Notes (from class notes)

> Full guides: [`../UML_DIAGRAMS_AND_NOTATION.md`](../UML_DIAGRAMS_AND_NOTATION.md) · [`../INHERITANCE_AND_COMPOSITION.md`](../INHERITANCE_AND_COMPOSITION.md)

---

## UML diagram families

UML diagrams **2 types** ke hote hain:

| Family | Count | Repo focus |
| ------ | ----- | ---------- |
| **Structural (static)** | 7 | **Class diagram** — must for LLD |
| **Behavioral (dynamic)** | 7 | **Sequence diagram** — object communication |

Total **14** types — interview me zyada tar **Class + Sequence** enough.

---

## Class diagram visibility

| Modifier | Symbol |
| -------- | ------ |
| public | `+` |
| protected | `#` |
| private | `-` |

---

## Abstract vs concrete

- **Abstract class** — kam se kam ek **pure virtual** method; body child me.
- **Concrete class** — instantiate ho sakti hai.

---

## Has-A family (object relationships)

| Type | Hindi one-liner | C++ hint |
| ---- | --------------- | -------- |
| **Association** | Jaante ho, use karte ho — **no ownership** | Reference / pointer, independent life |
| **Aggregation** | Weak has-a — **independently exist** kar sakte hain | Pointer, shared lifetime possible |
| **Composition** | Strong has-a — **part whole ke saath** | Object / `unique_ptr` inside owner |

**Deep dive + 4 demos:** [`L1 Composition`](../../%20L1%20Composition/)

---

## Is-A vs Has-A (L4 code demos)

| Relation | UML | Phrase | Demo |
| -------- | --- | ------ | ---- |
| **Inheritance** | Hollow triangle `△` | **Is-A** | [`01_Inheritance_Five_Types.cpp`](../C%20%2B%2B%20Code/01_Inheritance_Five_Types.cpp) |
| **Composition** | Filled diamond `◆` | **Has-A** (strong) | [`02_Composition_UniquePtr.cpp`](../C%20%2B%2B%20Code/02_Composition_UniquePtr.cpp) |

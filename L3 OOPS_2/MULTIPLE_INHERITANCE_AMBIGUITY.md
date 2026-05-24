# Multiple Inheritance & Ambiguity — Complete Guide

> **Code:** [`15_Multiple_Inheritance_Ambiguity.cpp`](./C%20%2B%2B%20Code/15_Multiple_Inheritance_Ambiguity.cpp) · [`08_Diamond_Problem.cpp`](./C%20%2B%2B%20Code/08_Diamond_Problem.cpp)  
> **L4 five types:** [`inheritance.cpp`](../L4%20UML_Diagrams/inheritance.cpp)

---

## Table of Contents

1. [Multiple inheritance basics](#1-multiple-inheritance-basics)
2. [Ambiguity type 1 — same name in two bases](#2-ambiguity-type-1--same-name-in-two-bases)
3. [Ambiguity type 2 — diamond problem](#3-ambiguity-type-2--diamond-problem)
4. [Solutions](#4-solutions)
5. [Interview Q&A](#5-interview-qa)

---

## 1. Multiple inheritance basics

```cpp
class C : public A, public B { };
```

Child ke paas **dono** base subobjects hote hain — `A` part + `B` part.

**Valid use:** Mix two independent interfaces (`Printer` + `Scanner`).

**Risk:** Name clashes + diamond.

---

## 2. Ambiguity type 1 — same name in two bases

```cpp
class AllInOne : public Printer, public Scanner { };

// device.print();   // ❌ ambiguous
device.Printer::print();
device.Scanner::print();
```

**Fix:** **Scope resolution** — batayo kaunsa base.

---

## 3. Ambiguity type 2 — diamond problem

```
      Person
     /      \
 Student   Employee
     \      /
   TeachingAssistant
```

`TeachingAssistant` inherits **two `Person` subobjects** — duplicate data + ambiguous `name`, `introduce()`.

```cpp
// ta.name = "x";     // ❌ ambiguous
ta.Student::name = "A";
ta.Employee::name = "B";  // can differ!
```

| Symptom | Cause |
|---------|-------|
| `sizeof(D)` bada | Do base copies |
| Ambiguous member | Two paths to same base |

---

## 4. Solutions

### A. Scope resolution (diamond without virtual — workaround)

```cpp
Student::introduce();
Employee::introduce();
```

### B. Virtual inheritance (proper fix)

```cpp
class Student : public virtual PersonV { };
class Employee : public virtual PersonV { };
class TA : public Student, public Employee { };

ta.name = "Bob";   // ✅ single PersonV
```

| | Non-virtual MI | Virtual inheritance |
|---|----------------|---------------------|
| Base copies | 2× Person | 1× shared |
| Complexity | Lower | Higher ctor rules |

**Repo:** `08_Diamond_Problem.cpp` — `sizeof` + virtual base demo.

---

## 5. Interview Q&A

<details>
<summary><strong>MI vs interface (Java)?</strong></summary>

C++ MI = **implementation + data** inherit. Java interface = no state (until default methods). C++ prefer composition + single inheritance when possible.

</details>

<details>
<summary><strong>Diamond fix?</strong></summary>

`virtual` inheritance on **middle** classes (`Student`, `Employee`) so most-derived shares one base.

</details>

---

## Cheat sheet

```
Same name, two bases     Printer::print() vs Scanner::print()
Diamond                  virtual public Base on middle classes
Avoid                    deep MI trees — composition often better
```

---

⬅️ [OOPS_ADVANCED_INHERITANCE](./OOPS_ADVANCED_INHERITANCE.md) · ➡️ [RTTI](./RTTI_COMPLETE.md)

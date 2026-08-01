# Advanced Inheritance & Polymorphism (L3) — Virtual, vtable, Diamond, Overload vs Override

> Code: [`06_Virtual_Destructor.cpp`](./C%20%2B%2B%20Code/06_Virtual_Destructor.cpp) … [`16_RTTI_Typeid_Dynamic_Cast.cpp`](./C%20%2B%2B%20Code/16_RTTI_Typeid_Dynamic_Cast.cpp)  
> **Interview topics:** [`notes/OOPS_INHERITANCE_INTERVIEW_TOPICS.md`](./notes/OOPS_INHERITANCE_INTERVIEW_TOPICS.md)  
> **MI ambiguity:** [`MULTIPLE_INHERITANCE_AMBIGUITY.md`](./MULTIPLE_INHERITANCE_AMBIGUITY.md) · **RTTI:** [`RTTI_COMPLETE.md`](./RTTI_COMPLETE.md)  
> **Conversions (L2):** [CONVERSION_FUNCTIONS](../L2%20OOPS_1/CONVERSION_FUNCTIONS.md) · Memory: [OOPS_ADVANCED_CPP](../L2%20OOPS_1/OOPS_ADVANCED_CPP.md)

---

## 1. Virtual Function & vtable

### Virtual function

`virtual` = **runtime binding** — correct override through base pointer.

```cpp
Animal* a = new Dog();
a->speak();  // Dog::speak
```

### vtable (conceptual)

```
object [ vptr | fields... ]
          ↓
       vtable: [&Dog::speak, &Dog::~Dog, ...]
```

| Term | Meaning |
| ---- | ------- |
| **vptr** | Hidden pointer per object (compiler) |
| **vtable** | Per-class table of virtual function addresses |
| **Cost** | Extra indirection + memory per object |

**Code:** [`07_Virtual_Table_Demo.cpp`](./C%20%2B%2B%20Code/07_Virtual_Table_Demo.cpp)

---

## 2. Virtual Destructor — Kyun Zaroori?

> **Full dedicated guide:** [`notes/Virtual_Destructor_Kyun.md`](./notes/Virtual_Destructor_Kyun.md) — problem, vtable, order, interview Q&A.

```cpp
Base* p = new Derived();
delete p;
```

| ~Base | Result |
| ----- | ------ |
| **Non-virtual** | ❌ Only `~Base` — `~Derived` skip → **leak** / UB |
| **virtual** | ✅ `~Derived` then `~Base` |

**Rule:** Agar class me **koi bhi virtual** hai ya base pointer se delete hoga → `virtual ~Base()`.

**Code:** [`06_Virtual_Destructor.cpp`](./C%20%2B%2B%20Code/06_Virtual_Destructor.cpp)

---

## 3. Diamond Problem

```
    A
   / \
  B   C
   \ /
    D
```

`D` inherits **two copies** of `A` → ambiguity: `D::value`, `D::show()`.

| Symptom | Cause |
| ------- | ----- |
| Ambiguous member access | Two `A` subobjects |
| Larger `sizeof(D)` | Duplicate bases |

**Product companies love this** — tests MI + `virtual` knowledge.

---

## 4. Virtual Inheritance — Solution

```cpp
class B : public virtual A {};
class C : public virtual A {};
class D : public B, public C { };  // ONE shared A
```

| | Normal MI | virtual inheritance |
| - | --------- | --------------------- |
| **A copies in D** | 2 | **1** |
| **Ambiguity** | Yes | Resolved |
| **Cost** | Lower | Extra indirection for virtual base |

**Code:** [`08_Diamond_Problem.cpp`](./C%20%2B%2B%20Code/08_Diamond_Problem.cpp)

**Interview:** "Use virtual inheritance on **common base** when diamond; prefer composition over deep MI in LLD."

**Advanced:** [`VIRTUAL_BASE_CLASS_ADVANCED.md`](./VIRTUAL_BASE_CLASS_ADVANCED.md) — most-derived initializes virtual base, ctor order, vbptr · [`17_Virtual_Base_Class_Advanced.cpp`](./C%20%2B%2B%20Code/17_Virtual_Base_Class_Advanced.cpp)

---

## 4b. Covariant Return Types

Override me return type **derived class ka pointer/reference** ho sakta hai:

```cpp
class Document { virtual Document* clone() const; };
class PdfDocument : public Document {
    PdfDocument* clone() const override;  // covariant
};
```

**Guide:** [`COVARIANT_RETURN_TYPES.md`](./COVARIANT_RETURN_TYPES.md) · **Code:** [`18_Covariant_Return_Types.cpp`](./C%20%2B%2B%20Code/18_Covariant_Return_Types.cpp)

---

## 5. Function Overloading vs Overriding

| | **Overloading** | **Overriding** |
| - | --------------- | -------------- |
| **Polymorphism** | Static (compile-time) | Dynamic (runtime) |
| **Where** | Same class (usually) | Parent + child |
| **Signature** | **Must differ** | **Must match** virtual parent |
| **Keywords** | None | `virtual`, `override` |
| **Resolution** | Compiler picks overload | vtable at runtime |
| **Inheritance** | Not required | Required |

```cpp
// Overload
void print(int);
void print(string);

// Override
class Base { virtual void f(); };
class Derived { void f() override; };
```

**Code:** [`09_Overloading_Vs_Overriding.cpp`](./C%20%2B%2B%20Code/09_Overloading_Vs_Overriding.cpp)  
Also: [`02_Static_Polymorphism.cpp`](./C%20%2B%2B%20Code/02_Static_Polymorphism.cpp), [`03_Dynamic_Polymorphism.cpp`](./C%20%2B%2B%20Code/03_Dynamic_Polymorphism.cpp)

---

## 6. Mega Comparison Table (Interview)

| Topic | Binding | When |
| ----- | ------- | ---- |
| Overloading | Compile | Same name, diff params |
| Overriding | Runtime | virtual + same signature |
| Default args | Compile | Not polymorphic — avoid virtual + default arg gotchas |
| `final` | — | No further override |
| Pure virtual `=0` | — | Abstract class |

---

## 7. Interview Q&A

<details><summary>Diamond problem?</summary>Two paths to same base → duplicate subobjects → ambiguity. Fix: virtual inheritance on shared base.</details>

<details><summary>Virtual inheritance?</summary>Makes base shared in MI diamond; one A in D.</details>

<details><summary>vtable cost?</summary>One vptr per object + indirect call; usually fine for LLD clarity.</details>

<details><summary>Overload vs override one line?</summary>Overload: same class, diff signature, compile time. Override: virtual, same signature, runtime.</details>

---

⬅️ [L2 Advanced C++](../L2%20OOPS_1/OOPS_ADVANCED_CPP.md) · [L3 OOPS_COMPLETE_GUIDE](./OOPS_COMPLETE_GUIDE.md)

# Multiple Inheritance, RTTI, Virtual Base, Covariant Return

> L3 ke advanced interview topics ek place par: ambiguity resolution, runtime casting, virtual base construction, and covariant clone pattern.

**Demos:**  
[`15_Multiple_Inheritance_Ambiguity.cpp`](../C++%20Code/15_Multiple_Inheritance_Ambiguity.cpp)  
[`16_RTTI_Typeid_Dynamic_Cast.cpp`](../C++%20Code/16_RTTI_Typeid_Dynamic_Cast.cpp)  
[`17_Virtual_Base_Class_Advanced.cpp`](../C++%20Code/17_Virtual_Base_Class_Advanced.cpp)  
[`18_Covariant_Return_Types.cpp`](../C++%20Code/18_Covariant_Return_Types.cpp)

---

## 1) Multiple Inheritance Ambiguity

Same method name do base classes me ho to call ambiguous hoti hai.

```cpp
AllInOne aio;
aio.Printer::print();
aio.Scanner::print();
```

Fix: scope resolution operator use karo.

---

## 2) Diamond and Virtual Inheritance

Diamond me duplicate base subobjects problem aata hai.

```cpp
class B : public virtual A {};
class C : public virtual A {};
class D : public B, public C {};
```

`virtual` inheritance ke baad `D` me `A` ka single shared subobject hota hai.

---

## 3) RTTI: `typeid` and `dynamic_cast`

- `typeid(*basePtr)` dynamic object type deta hai (polymorphic base required).
- `dynamic_cast<Derived*>(basePtr)` safe downcast deta hai.
- Failed pointer cast -> `nullptr`; failed reference cast -> `std::bad_cast`.

```cpp
if (Dog* d = dynamic_cast<Dog*>(animal)) {
    d->bark();
}
```

---

## 4) Virtual Base Construction Rule

Virtual inheritance case me shared virtual base ko **most-derived class** construct karta hai.

```cpp
class MultiFunction : public virtual Device {
public:
    MultiFunction(string id) : Device(id) {}
};
```

---

## 5) Covariant Return Type

Base pointer/ref return method ko derived override me more specific return type diya ja sakta hai.

```cpp
class Document {
public:
    virtual Document* clone() const = 0;
};

class PdfDocument : public Document {
public:
    PdfDocument* clone() const override;
};
```

Isse callers ko unnecessary casts kam lagte hain.

---

## 6) Quick Interview Drill

- MI ambiguity resolve kaise karoge?
- `dynamic_cast` aur `static_cast` me safety difference?
- `typeid(p)` vs `typeid(*p)`?
- Virtual base constructor ka owner kaun?
- Covariant return kaha useful hai (clone/factory APIs)?

---

## 7) Run

```bash
cd "L3 OOPS_2"
./compile.sh
./bin/15_Multiple_Inheritance_Ambiguity
./bin/16_RTTI_Typeid_Dynamic_Cast
./bin/17_Virtual_Base_Class_Advanced
./bin/18_Covariant_Return_Types
```

---

## 8) One-line Cheat Sheet

`Base1::foo()` + `virtual` inheritance + `dynamic_cast` + `typeid(*p)` + covariant `clone()`.

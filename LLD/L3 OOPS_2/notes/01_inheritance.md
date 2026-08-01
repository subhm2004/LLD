# Inheritance Quick Notes (L3)

**Demo:** [`01_Inheritance.cpp`](../C++%20Code/01_Inheritance.cpp)

---

## 1) Core Idea

Inheritance models **IS-A** relationship.

- `ManualCar` is a `Car`
- `ElectricCar` is a `Car`

Base class common behavior hold karta hai, derived classes specialization add karti hain.

---

## 2) Syntax

```cpp
class ManualCar : public Car {
public:
    ManualCar(string brand, string model) : Car(brand, model) {}
};
```

- `public` inheritance most common for true subtype relation.
- `protected` members child use kar sakta hai, outside world nahi.

---

## 3) Access Summary

| Base member | Derived access? | Outside access? |
| --- | --- | --- |
| `public` | yes | yes |
| `protected` | yes | no |
| `private` | no (direct) | no |

---

## 4) Constructor/Destructor Flow

- Construction: Base -> Derived
- Destruction: Derived -> Base

```cpp
class Car {
public:
    virtual ~Car() = default;
};
```

Polymorphic base me virtual destructor must-have hai.

---

## 5) Inheritance vs Composition

- Inheritance: IS-A
- Composition: HAS-A

If relation true subtype nahi hai, composition prefer karo.

---

## 6) Interview One-liners

- Inheritance ka primary goal: reuse + extensibility + polymorphic substitution.
- Private members inherited state ka part hote hain but directly accessible nahi.
- Overuse inheritance can increase coupling; composition safer default hota hai.

---

## 7) Run

```bash
cd "L3 OOPS_2"
./compile.sh
./bin/01_Inheritance
```

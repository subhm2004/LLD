# Access Specifiers in Inheritance and Constructor Chaining

**Demos:**  
[`10_Access_Specifiers_Inheritance.cpp`](../C++%20Code/10_Access_Specifiers_Inheritance.cpp)  
[`11_Constructor_Chaining.cpp`](../C++%20Code/11_Constructor_Chaining.cpp)

---

## 1) Member Access Basics

| Base member | Child can access? | Outside can access? |
| --- | --- | --- |
| `public` | yes | yes |
| `protected` | yes | no |
| `private` | no (directly) | no |

---

## 2) Inheritance Mode Impact

| Base member | `public` inheritance | `protected` inheritance | `private` inheritance |
| --- | --- | --- | --- |
| `public` | public | protected | private |
| `protected` | protected | protected | private |
| `private` | not directly accessible | not directly accessible | not directly accessible |

Rule of thumb: interview/design me default `public inheritance` use hota hai (true IS-A relation).

---

## 3) Constructor Chaining

- Base constructor always pehle run hota hai.
- Fir derived constructor body run hoti hai.
- Destruction reverse order me hota hai.

```cpp
class Derived : public Base {
public:
    Derived(int id) : Base(id) {}
};
```

If base has no default constructor, derived ko initializer list me base ctor call dena mandatory hai.

---

## 4) Quick Interview Points

- `private` members inherited hote hain as object state, but direct access nahi hota.
- `public` vs `protected` inheritance ka difference visibility mapping ka hota hai.
- Constructor chaining order: Base -> Derived.
- Destructor order: Derived -> Base.

---

## 5) Run

```bash
cd "L3 OOPS_2"
./compile.sh
./bin/10_Access_Specifiers_Inheritance
./bin/11_Constructor_Chaining
```

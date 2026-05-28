# Cohesion, Coupling, and SRP

**Demo:** [`14_Cohesion_Coupling.cpp`](../C++%20Code/14_Cohesion_Coupling.cpp)

---

## 1) Cohesion

Cohesion batata hai class ke andar responsibilities kitni focused hain.

- High cohesion: class ka ek clear kaam.
- Low cohesion: random unrelated kaam same class me.

---

## 2) Coupling

Coupling batata hai classes kitni tightly dependent hain.

- Low coupling: interfaces/abstractions ke through dependencies.
- High coupling: concrete internals pe hard dependency.

---

## 3) SRP Link

Single Responsibility Principle follow karne se:

- Cohesion improve hoti hai
- Coupling generally reduce hota hai
- Testing easy hota hai

---

## 4) Practical LLD Guidance

- God class avoid karo (validation + DB + payment + notifications all-in-one nahi).
- Orchestration class banao, focused services delegate karo.
- Constructor injection use karo so mocks/stubs plug ho sakein.

```cpp
class OrderService {
public:
    OrderService(PaymentService& p, OrderRepository& r, EmailNotifier& n)
        : payment_(p), repo_(r), notifier_(n) {}
};
```

---

## 5) Interview Quick Answers

- Cohesion vs coupling?  
  Cohesion = within class quality, coupling = between classes dependency level.
- High cohesion and low coupling kyun?  
  Maintainability, readability, testability improve hoti hai.
- SRP relation?  
  One reason to change -> focused classes -> better cohesion.

---

## 6) Run

```bash
cd "L3 OOPS_2"
./compile.sh
./bin/14_Cohesion_Coupling
```

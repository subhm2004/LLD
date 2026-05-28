# OOP API Error Handling (Exception vs Status)

L3 context: inheritance/polymorphism based APIs design karte time error contract clear hona chahiye.

---

## 1) Two Common Approaches

### A. Exception-based API

Use when caller usually success expect karta hai, aur failure exceptional hai.

```cpp
class PaymentGateway {
public:
    virtual void pay(double amount) = 0;  // throws on failure
    virtual ~PaymentGateway() = default;
};
```

Pros:
- Happy path clean.
- Detailed error propagation.

Cons:
- Unhandled exceptions crash kar sakte hain.
- Cross-boundary APIs me risky (threads/FFI).

### B. Status/Result-based API

Use when failure expected hai (validation/network/business rules).

```cpp
struct Status {
    bool ok;
    string code;
    string message;
};

class PaymentGateway {
public:
    virtual Status pay(double amount) = 0;
    virtual ~PaymentGateway() = default;
};
```

Pros:
- Explicit contract.
- Predictable control flow.

Cons:
- Boilerplate checks (`if (!status.ok)`).

---

## 2) L3 Design Rule

- Library/core polymorphic interfaces: prefer status/result for predictable handling.
- Internal infra code where invariant break is exceptional: exceptions acceptable.
- Never mix silently; document one contract per method.

---

## 3) Hybrid Pattern (Recommended)

- Public API returns status/result.
- Internal implementation may throw.
- Boundary layer catches and converts exception -> status.

```cpp
Status GatewayService::safePay(double amount) {
    try {
        gateway_->pay(amount);   // internal may throw
        return {true, "OK", "Payment success"};
    } catch (const std::exception& ex) {
        return {false, "GATEWAY_ERROR", ex.what()};
    }
}
```

---

## 4) Interview Answer Template

"Agar failure business flow ka normal part hai to status/result return karta hoon. Agar truly exceptional invariant/system issue hai to exception use karta hoon. Public API boundary par exception ko controlled status me convert karta hoon."

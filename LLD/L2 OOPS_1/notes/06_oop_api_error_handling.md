# OOP API Error Handling (Exception vs Status)

L2 level par API design karte waqt yeh decide karna zaroori hai ki error ko throw karna hai ya return object me dena hai.

---

## 1) Exception-based Style

```cpp
class BankAccount {
public:
    void withdraw(double amount) {
        if (amount <= 0) {
            throw std::invalid_argument("amount must be positive");
        }
        // ...
    }
};
```

Use when:
- Failure rare ho.
- Caller ke paas try/catch policy defined ho.

---

## 2) Status/Result-based Style

```cpp
struct Result {
    bool ok;
    string code;
    string message;
};

class BankAccount {
public:
    Result withdraw(double amount) {
        if (amount <= 0) return {false, "INVALID_AMOUNT", "amount must be positive"};
        return {true, "OK", "withdraw successful"};
    }
};
```

Use when:
- Validation/business failures common hain.
- API ko explicit predictable flow dena ho.

---

## 3) Which One in Interviews?

- Input validation APIs: status/result usually better.
- Constructor/resource critical failure: exception acceptable.
- Public APIs me mixed style avoid karo; contract consistent rakho.

---

## 4) Hybrid Practical Pattern

- Internal methods exceptions throw kar sakte hain.
- Boundary/API layer catch karke status return kare.

Isse internals expressive rehte hain aur callers ko stable contract milta hai.

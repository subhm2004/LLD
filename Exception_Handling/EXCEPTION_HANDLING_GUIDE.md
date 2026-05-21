# Exception Handling — Complete Guide (C++17)

> **Runnable code:** [`C++ Code/`](./C++%20Code/) · **Build:** `./compile.sh` · **Run all:** `./run_all.sh`

---

## 1. Core flow

```
throw  →  stack unwind  →  matching catch  →  continue after catch
                ↓
         destructors (RAII) run on the way up
```

| Keyword | Role |
|---------|------|
| `throw` | Signal error; starts unwinding |
| `try` | Region that may throw |
| `catch` | Handler for type (or `...`) |

**Demo:** `01_basics_try_catch.cpp`

---

## 2. Standard hierarchy (use these in LLD)

```
std::exception
├── std::logic_error      → bug / invalid args before/during logic
│   ├── invalid_argument
│   └── out_of_range
└── std::runtime_error    → runtime failure (most LLD projects)
```

| Situation | Throw |
|-----------|--------|
| Empty id, negative amount | `invalid_argument` |
| Seat already booked, not found | `runtime_error` or custom |
| Unknown / wrap at facade | `catch (...)` last resort |

**Demo:** `02_standard_exception_hierarchy.cpp`, `07_lld_service_validation.cpp`

---

## 3. Custom exceptions

```cpp
class InsufficientBalanceException : public std::runtime_error {
public:
    InsufficientBalanceException(std::string id, double bal, double amt)
        : std::runtime_error("Insufficient balance"), id_(std::move(id)) { ... }
    const std::string& getAccountId() const { return id_; }
};
```

**Why:** Extra fields + clear `catch (const InsufficientBalanceException&)`.

**Demo:** `03_custom_exception.cpp`

---

## 4. Catch rules

| Rule | Reason |
|------|--------|
| `catch (const T& e)` | Avoid slicing + copy cost |
| Specific → general | `out_of_range` before `exception` |
| `throw;` | Rethrow same exception after log |
| Avoid `catch (...)` unless boundary | Swallows real type info |

**Demo:** `04_catch_order_and_rethrow.cpp`, `12_catch_all_and_bad_throw.cpp`

---

## 5. RAII & exception safety

- **Basic guarantee:** no leak; valid state
- **Strong guarantee:** all-or-nothing (harder)
- **Nothrow guarantee:** destructor `noexcept`

C++ has **no `finally`** — use destructors / `ScopeGuard` / `unique_ptr`.

**Demo:** `05_raii_exception_safety.cpp`, `10_nested_try_finally_raii.cpp`, `14_stack_unwinding_destructors.cpp`

---

## 6. `noexcept`

- Destructor implicitly `noexcept` in C++11+
- `noexcept` function → throw = `std::terminate`
- Use when failure is impossible or must not throw

**Demo:** `06_noexcept.cpp`

---

## 7. C++17 alternatives

| Tool | When |
|------|------|
| `std::optional<T>` | Expected failure (not found) — no stack cost |
| `std::expected` (C++23) | Error code + value |
| Exceptions | Exceptional / crosses layers / rich error |

**Demo:** `09_cpp17_features.cpp`, `13_lld_facade_error_boundary.cpp`

---

## 8. Advanced

| Topic | Demo |
|-------|------|
| `std::exception_ptr` | `11_exception_ptr.cpp` |
| Nested try + audit log | `10_nested_try_finally_raii.cpp` |
| LSP exception rule | `08_lsp_exception_rule.cpp` |
| Facade error boundary | `13_lld_facade_error_boundary.cpp` |

---

## 9. LLD repo usage

Projects like **Parking**, **Library**, **File Manager** use:

```cpp
if (!spot) throw std::runtime_error("Spot not found");
```

**Facade pattern:**

```cpp
optional<string> tryBook(...) {
  try { service.book(...); return "OK"; }
  catch (const exception& e) { lastError_ = e.what(); return nullopt; }
}
```

---

## 10. Interview cheat sheet

| Question | Answer |
|----------|--------|
| Difference logic vs runtime error? | Invalid arg vs runtime failure |
| Why catch by reference? | Slicing + performance |
| What if destructor throws? | `std::terminate` during unwind |
| Exception vs optional? | Expected miss → optional; exceptional → throw |
| LSP exceptions? | Child throws narrower, not broader |
| Java `finally` in C++? | RAII destructor / ScopeGuard |

---

## All demos

| # | File |
|---|------|
| 01–09 | Basics → C++17 optional |
| 10 | Nested try + finally-style RAII |
| 11 | `exception_ptr` |
| 12 | `catch(...)`, bad `throw int` |
| 13 | Facade + `optional` (Movie seat) |
| 14 | Stack unwinding trace |

---

## Related

- [`L6 ExceptionRule.cpp`](../L6%20SOLID_2/C++%20Code/LSP-Rules/SingatureRules/ExceptionRule.cpp)
- [`File_Manager_LLD`](../File_Manager_LLD/)
- [`SOLID.md`](../SOLID.md)

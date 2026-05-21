# Exception Handling — C++17

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue?style=for-the-badge" alt="C++17" />
  <img src="https://img.shields.io/badge/Demos-14-success?style=for-the-badge" alt="14 demos" />
</p>

> **Source:** [`C++ Code/`](./C++%20Code/) — har file `.cpp` (root / `bin/` = compiled output)  
> **Full guide:** [`EXCEPTION_HANDLING_GUIDE.md`](./EXCEPTION_HANDLING_GUIDE.md)

---

## Quick run

```bash
cd Exception_Handling
./compile.sh              # g++ -std=c++17 -Wall -Wextra -pedantic → bin/
./bin/01_basics_try_catch
./run_all.sh              # compile + run all 14 demos
```

---

## All source files (`C++ Code/*.cpp`)

| # | File | Topic |
|---|------|--------|
| 01 | `01_basics_try_catch.cpp` | `throw`, `try`, `catch` |
| 02 | `02_standard_exception_hierarchy.cpp` | `logic_error`, `runtime_error`, `exception` |
| 03 | `03_custom_exception.cpp` | Custom domain exception |
| 04 | `04_catch_order_and_rethrow.cpp` | Catch order + `throw;` rethrow |
| 05 | `05_raii_exception_safety.cpp` | RAII + `unique_ptr` |
| 06 | `06_noexcept.cpp` | `noexcept` |
| 07 | `07_lld_service_validation.cpp` | File-style service errors |
| 08 | `08_lsp_exception_rule.cpp` | LSP — narrower exceptions |
| 09 | `09_cpp17_features.cpp` | `optional`, `string_view`, structured bindings |
| 10 | `10_nested_try_finally_raii.cpp` | Nested try + **finally** via `ScopeGuard` |
| 11 | `11_exception_ptr.cpp` | `std::exception_ptr` store / rethrow |
| 12 | `12_catch_all_and_bad_throw.cpp` | `catch(...)`, why not `throw 42` |
| 13 | `13_lld_facade_error_boundary.cpp` | Facade + `optional` (Movie seat booking) |
| 14 | `14_stack_unwinding_destructors.cpp` | Stack unwind + destructor order |

---

## Folder layout

```
Exception_Handling/
├── C++ Code/          ← EDIT HERE (.cpp sources)
├── bin/               ← compiled binaries (gitignored)
├── compile.sh
├── run_all.sh
├── EXCEPTION_HANDLING_GUIDE.md
├── problem_statement.md
├── summary.txt
└── README.md
```

---

## LLD repo links

| Project | Exception style |
|---------|-----------------|
| File Manager, Library, Parking | `throw std::runtime_error(...)` |
| L6 SOLID | [`ExceptionRule.cpp`](../L6%20SOLID_2/C++%20Code/LSP-Rules/SingatureRules/ExceptionRule.cpp) |

---

## Interview rules (short)

1. Catch `const std::exception&`
2. Specific handlers before `catch (...)`
3. RAII for cleanup — no Java `finally`
4. Facade catches → `optional` / error string to UI
5. `invalid_argument` = bad input; `runtime_error` = business rule fail

---

## Related

- [`SOLID.md`](../SOLID.md)
- [`INHERITANCE_AND_COMPOSITION.md`](../L4%20UML_Diagrams/INHERITANCE_AND_COMPOSITION.md) — RAII / smart pointers

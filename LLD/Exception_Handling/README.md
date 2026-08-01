# Exception Handling — C++17

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue?style=for-the-badge" alt="C++17" />
  <img src="https://img.shields.io/badge/Demos-14-success?style=for-the-badge" alt="14 demos" />
</p>

## 📖 Main study file (detailed — yahi padho)

### ➡️ [`EXCEPTION_HANDLING_COMPLETE.md`](./EXCEPTION_HANDLING_COMPLETE.md)

Isme **poori detail** hai:

- Kitne **types** ki exception handling hoti hai (mechanism, catch, LLD patterns)
- **Har standard exception** — kab use karein
- **Saare important functions** — `what()`, `current_exception`, `rethrow_exception`, `noexcept`, …
- `try` / `catch` / `throw`, stack unwinding, RAII, LSP, facade, interview Q&A
- 14 demos ka map

---

## Quick run (code)

```bash
cd Exception_Handling
./compile.sh              # C++ Code/*.cpp → bin/
./bin/01_basics_try_catch
./run_all.sh              # sab demos
```

**Source files:** [`C++ Code/`](./C++%20Code/) — `.cpp` yahan edit karo (root/`bin/` = binaries)

| # | File |
|---|------|
| 01–14 | See table in [`EXCEPTION_HANDLING_COMPLETE.md` §17](./EXCEPTION_HANDLING_COMPLETE.md#17-repo-demos--line-by-line-map) |

---

## Other files

| File | Purpose |
|------|---------|
| [`EXCEPTION_HANDLING_COMPLETE.md`](./EXCEPTION_HANDLING_COMPLETE.md) | **Master guide (detailed)** |
| [`EXCEPTION_HANDLING_GUIDE.md`](./EXCEPTION_HANDLING_GUIDE.md) | Shorter summary |
| [`summary.txt`](./summary.txt) | 1-page Hinglish notes |
| [`problem_statement.md`](./problem_statement.md) | Module goals |

---

## Related

- [`SOLID.md`](../docs/SOLID.md) · [`File_Manager_LLD`](../File_Manager_LLD/) · [L6 `ExceptionRule.cpp`](../L6%20SOLID_2/C++%20Code/LSP-Rules/SingatureRules/ExceptionRule.cpp)

# Problem Statement — Exception Handling Module

Learn **C++17 exception handling** for LLD interviews and this repository's coding style.

## Learning goals

1. Use `try` / `catch` / `throw` correctly.
2. Pick `invalid_argument` vs `runtime_error` vs custom exceptions.
3. Apply **RAII** so resources cleanup on failure.
4. Implement **facade error boundaries** (don't leak exceptions to `main`).
5. Understand **LSP** exception rules (L6).
6. Know when **`std::optional`** is better than exceptions.

## Deliverables

- 14 runnable demos in `C++ Code/`
- `EXCEPTION_HANDLING_GUIDE.md` — theory
- `./compile.sh` — builds all with `-std=c++17`

## Out of scope

- SEH (Windows structured exceptions)
- Coroutines error handling
- Full error-code vs exception debate (only pointers in guide)

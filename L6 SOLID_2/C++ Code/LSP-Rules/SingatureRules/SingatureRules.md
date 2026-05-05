# LSP Signature Rules

This folder demonstrates LSP rules related to method signatures:

- `MethodArgumentRule.cpp`
- `ReturnTypeRule.cpp`
- `ExceptionRule.cpp`

## 1) Method Argument Rule
File: `MethodArgumentRule.cpp`

- Parent method: `print(string msg)`.
- Child override keeps the same signature: `print(string msg)`.
- Client depends on `Parent*` and can safely call child object.

LSP insight:
- Overridden method must remain compatible with the parent contract.
- In C++, `override` helps enforce signature compatibility.

## 2) Return Type Rule
File: `ReturnTypeRule.cpp`

- Parent returns `Animal*` in `getAnimal()`.
- Child override also returns `Animal*`, but actual object is `Dog`.
- This matches covariance-oriented substitution behavior.

LSP insight:
- Child return must be compatible with parent-declared return.
- Client written against parent type should continue to work.

## 3) Exception Rule
File: `ExceptionRule.cpp`

- Parent throws `logic_error`.
- Child throws `out_of_range` (derived from `logic_error`).
- Comment also shows broader `runtime_error` as an invalid design choice for this contract.

LSP insight:
- Child should throw same or narrower exceptions than parent promise.
- C++ does not strictly enforce checked exceptions, so this is a design-level rule.

## Signature Rule Summary
- Keep override signature compatible with parent.
- Keep return type compatible/covariant.
- Avoid broader or unexpected exceptions in child methods.
- Preserve what parent-based clients expect.

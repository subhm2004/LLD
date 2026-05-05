# LSP Method Rules

This folder demonstrates LSP method-level contract rules:

- `PreConditions.cpp`
- `PostConditions.cpp`

## 1) Precondition Rule
File: `PreConditions.cpp`

- Parent `User::setPassword()` requires minimum 8 characters.
- Child `AdminUser::setPassword()` requires minimum 6 characters.
- Child is **weakening** the precondition, which is LSP-compliant.

Why valid:
- Any caller that satisfies parent condition (8+) also works with child.
- Child accepts a broader input range, so substitution remains safe.

## 2) Postcondition Rule
File: `PostConditions.cpp`

- Parent `Car::brake()` guarantees speed reduction.
- Child `HybridCar::brake()` still reduces speed and additionally increases charge.
- Child is **strengthening** the postcondition, which is LSP-compliant.

Why valid:
- Parent promise is preserved.
- Extra guarantees do not break existing clients.

## Method Rule Summary
- Child methods should not demand stricter input than parent.
- Child methods may provide same or stronger output guarantees.
- If parent contract is preserved, substitutability remains intact.

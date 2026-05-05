# LSP Property Rules

This folder focuses on LSP property-level rules and related notes:

- `ClassInvariants.cpp`
- `HistoryConstraint.cpp`
- `note.cpp`
- `note2.cpp`
- `notes.txt`

## 1) Class Invariant Rule
File: `ClassInvariants.cpp`

- Parent `BankAccount` invariant: balance should not become negative.
- `withdraw()` in parent enforces this invariant.
- Child `CheatAccount` overrides `withdraw()` and allows negative balance.

Result:
- Parent property/invariant is broken in child.
- This violates LSP because client expectations no longer hold.

## 2) History Constraint Rule
File: `HistoryConstraint.cpp`

- Parent behavior allows withdrawal when valid.
- Child `FixedDepositAccount` changes behavior and blocks withdrawal completely.

Result:
- Child changes allowed state transitions promised by parent.
- This violates LSP since substituting child breaks client workflow.

## 3) `final` Notes
Files: `note.cpp`, `note2.cpp`, `notes.txt`

- `final` class cannot be inherited.
- `final` method cannot be overridden.
- These are inheritance restrictions in C++, useful for design control.
- They are not direct LSP compliance proofs, but they prevent some unsafe extension patterns.

## Property Rule Summary
- Child must preserve parent invariants.
- Child must preserve valid behavior history expected by clients.
- If child removes promised behavior or breaks object properties, LSP fails.

# LSP Rules Detailed Study

This folder explains **Liskov Substitution Principle (LSP)** through multiple C++ examples.

LSP says: child objects should be safely substitutable wherever parent objects are expected, without breaking client behavior.

## Folder Structure Covered
- `MethodRules/PreConditions.cpp`
- `MethodRules/PostConditions.cpp`
- `SingatureRules/MethodArgumentRule.cpp`
- `SingatureRules/ReturnTypeRule.cpp`
- `SingatureRules/ExceptionRule.cpp`
- `PropertiesRules/ClassInvariants.cpp`
- `PropertiesRules/HistoryConstraint.cpp`
- `PropertiesRules/note.cpp`
- `PropertiesRules/note2.cpp`
- `PropertiesRules/notes.txt`

## 1) Method Rules

### Precondition Rule (`MethodRules/PreConditions.cpp`)
- Parent `User::setPassword()` requires minimum 8 characters.
- Child `AdminUser::setPassword()` requires minimum 6 characters.
- Child is **weakening** precondition, which is valid for LSP.
- Reason: caller expecting 8+ still works; child accepts at least that and more.

### Postcondition Rule (`MethodRules/PostConditions.cpp`)
- Parent `Car::brake()` guarantees speed reduction.
- Child `HybridCar::brake()` also reduces speed and additionally increases charge.
- Child is **strengthening** postcondition, which is valid for LSP.
- Reason: parent guarantee is preserved; extra behavior does not break substitution.

## 2) Signature Rules

### Method Argument Rule (`SingatureRules/MethodArgumentRule.cpp`)
- Parent and child keep same signature: `print(string msg)`.
- In C++, `override` enforces compatible signature.
- Client using `Parent*` can call child implementation safely.

### Return Type Rule (`SingatureRules/ReturnTypeRule.cpp`)
- Parent method returns `Animal*`.
- Child returns `Animal*` while actually returning `new Dog()`.
- This demonstrates return-type compatibility/covariance behavior in OOP design terms.
- Client using `Parent*` still works without contract break.

### Exception Rule (`SingatureRules/ExceptionRule.cpp`)
- Parent throws `logic_error`.
- Child throws `out_of_range` (derived from `logic_error`) and comments show broader exception would be wrong.
- LSP view: child should not throw broader/unexpected exceptions than parent contract.
- C++ does not strongly enforce checked exceptions like some languages, so this is mostly design discipline.

## 3) Property Rules

### Class Invariant Rule (`PropertiesRules/ClassInvariants.cpp`)
- Parent invariant: account balance cannot become negative.
- `CheatAccount` bypasses validation and allows negative balance.
- This **breaks LSP** because child violates parent invariant expected by clients.

### History Constraint Rule (`PropertiesRules/HistoryConstraint.cpp`)
- Parent allows `withdraw()`.
- Child `FixedDepositAccount` overrides `withdraw()` to always throw error.
- This changes valid state transitions and behavior expected from parent type.
- Result: **LSP break** because substituting child can break client flow.

### `final` Notes (`PropertiesRules/note.cpp`, `note2.cpp`, `notes.txt`)
- `final` class: cannot be inherited.
- `final` method: cannot be overridden.
- These notes are language constraints; they are related to inheritance control, not direct proof of LSP compliance.

## Quick Takeaway
- **Valid for LSP:** weaker preconditions, stronger postconditions, compatible signatures/returns, narrower exceptions.
- **Invalid for LSP:** breaking invariants, removing/invalidating previously allowed behavior (history constraint break).

## Practical Checklist
- Keep parent guarantees intact in child.
- Never force stricter input requirements in child than parent promised.
- Never remove parent-supported operations in child.
- Maintain invariant and state consistency.
- Keep method contracts predictable for existing clients.

# L6 SOLID_2 - ISP, DIP, Advanced LSP Rules

This lesson continues SOLID with interface design, dependency inversion, and detailed LSP rule checks.

## Canonical Deep-Dive

For full architecture context (high-level vs low-level modules, OCP/DIP strategy, SRP connection), read:
- [`SOLID_HIGH_LOW_MODULES_DETAILED.md`](../SOLID_HIGH_LOW_MODULES_DETAILED.md)

---

## Folder Map

- `C++ Code/ISP/ISP_violated.cpp`
- `C++ Code/ISP/ISP_followed.cpp`
- `C++ Code/DIP/DIP_violated.cpp`
- `C++ Code/DIP/DIP_followed.cpp`
- `C++ Code/DIP/DIP_followed_new_pointer_style.cpp`
- `C++ Code/LSP-Rules/MethodRules/PreConditions.cpp`
- `C++ Code/LSP-Rules/MethodRules/PostConditions.cpp`
- `C++ Code/LSP-Rules/SingatureRules/ReturnTypeRule.cpp`
- `C++ Code/LSP-Rules/SingatureRules/MethodArgumentRule.cpp`
- `C++ Code/LSP-Rules/SingatureRules/ExceptionRule.cpp`
- `C++ Code/LSP-Rules/PropertiesRules/ClassInvariants.cpp`
- `C++ Code/LSP-Rules/PropertiesRules/HistoryConstraint.cpp`

---

## SRP/OCP/DIP Sync View

- SRP (L5) ensures class responsibility boundaries clean रहें.
- OCP (L5) ensures extension without repeatedly touching core.
- DIP (L6) ensures dependency direction policy -> abstraction ho, policy -> concrete na ho.
- ISP (L6) ensures interfaces lean रहें so clients forced methods implement na karein.
- LSP rules (L6) ensure abstraction contracts actually substitutable रहें.

Practical relationship:

`SRP stable boundaries + OCP extension points + DIP direction + ISP minimal contracts + LSP valid substitution`

---

## DIP Smells (Quick)

- High-level class directly imports SDK/client concrete.
- Constructor me concrete implementation hardcoded.
- Testing ke liye real infra dependency required hoti hai.

Minimal fix:
- Abstraction/interface introduce karo.
- Dependency inject karo (constructor injection simple and enough).
- Adapter layer me concrete SDK bind karo.

---

## Interview Pack (Fast)

- **SRP break sign:** one class, many reasons to change.
- **OCP failure sign:** every variant adds edits in same switch file.
- **DIP failure sign:** policy service depends directly on low-level concrete.
- **ISP failure sign:** class forced to implement methods it does not need.
- **LSP failure sign:** derived class weakens contract and breaks substitution.

---

## Recommended Flow

1. ISP violated/followed
2. DIP violated/followed
3. LSP rules folders (method, signature, property rules)
4. Final synthesis using `../SOLID_HIGH_LOW_MODULES_DETAILED.md`

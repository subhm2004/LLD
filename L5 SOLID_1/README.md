# L5 SOLID_1 - SRP, OCP, LSP (Foundations)

This lesson contains runnable C++ examples for first 3 SOLID principles.

## Canonical Deep-Dive

For full high-level vs low-level module discussion, OCP/DIP trade-offs, and SRP break patterns, read:
- [`SOLID_HIGH_LOW_MODULES_DETAILED.md`](../SOLID_HIGH_LOW_MODULES_DETAILED.md)

---

## Folder Map

- `C++ Code/SRP/SRP_violated.cpp`
- `C++ Code/SRP/SRP_followed.cpp`
- `C++ Code/OCP/OCP_violated.cpp`
- `C++ Code/OCP/OCP_followed.cpp`
- `C++ Code/LSP/LSP_Violated.cpp`
- `C++ Code/LSP/LSP_followed_wrongly.cpp`
- `C++ Code/LSP/LSP_followed.cpp`
- `C++ Code/summary.txt`

---

## SRP -> OCP -> DIP Cross-Map (Practical)

- SRP break hota hai jab ek class ke multiple reasons-to-change hote hain.
- SRP break hone par OCP weak hota hai, kyunki same core file bar-bar edit hoti hai.
- OCP weak hone par extension expensive hoti hai, regressions badhte hain.
- DIP ka objective dependency direction improve karna hai, taaki high-level policy concrete detail pe direct dependent na ho.

Short chain:

`SRP break -> OCP stress -> hard extension -> DIP pressure`

---

## SRP Break Smells (Quick)

- Same class me business rules + persistence + external API + formatting + notification.
- Har feature pe wahi class modify hoti hai.
- Unit tests me bahut mocks lagte hain even for tiny behavior.

Minimal fix:
- Responsibilities split by change-axis.
- Orchestrator class me sirf workflow/policy.
- External details adapters/services me.

---

## Study Order (Recommended)

1. `SRP_violated.cpp` -> `SRP_followed.cpp`
2. `OCP_violated.cpp` -> `OCP_followed.cpp`
3. `LSP_Violated.cpp` -> `LSP_followed.cpp`
4. Deep read: `../SOLID_HIGH_LOW_MODULES_DETAILED.md` Section 34 and nearby sections

---

## Interview One-Liner

"SRP maintain karoge to OCP naturally improve hota hai; aur OCP + DIP milke architecture ko extension-friendly aur testable banate hain."

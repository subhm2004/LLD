# Requirements: Amazon Locker Service

## Functional

| ID | Requirement |
|----|-------------|
| F1 | Register locker station with address and compartments |
| F2 | Deposit package → allocate compartment by size |
| F3 | Generate numeric pickup code with expiry (default 72h) |
| F4 | Notify customer with station, compartment, code (simulated) |
| F5 | Pickup with valid code → release compartment |
| F6 | Wrong code → increment attempts; lock at max (default 3) |
| F7 | Expired / used code → reject pickup |
| F8 | No-fit compartment → reject deposit |
| F9 | Print station availability |

## Non-functional

| ID | Requirement |
|----|-------------|
| NF1 | C++17, compile via `./compile.sh` |
| NF2 | Header-only implementation, namespace `amazon_locker_lld` |
| NF3 | Strategy pattern for allocation (extensible Best-Fit, etc.) |
| NF4 | Clear console demo flows in `main.cpp` |

## Size fit rules (First-Fit)

- SMALL package → any available compartment
- MEDIUM package → MEDIUM or LARGE compartment
- LARGE package → LARGE compartment only

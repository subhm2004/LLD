# Problem Statement: Amazon Locker Service

Design a low-level **Amazon Hub Locker** system where couriers deposit packages and customers pick them up using a one-time access code.

## Actors

- **Customer** — receives pickup code, collects package
- **Courier / Delivery agent** — deposits package into an allocated compartment
- **Locker station** — physical bank of compartments (S / M / L sizes)

## Functional requirements

1. Register **locker stations** with multiple compartments (size + status).
2. **Deposit** a package: allocate first available compartment that fits the package size, mark occupied, generate **pickup code**, notify customer (simulated).
3. **Pickup** with valid code: verify code, free compartment, mark package picked up.
4. Reject **wrong code** (track failed attempts; lock after max attempts).
5. Reject **expired** or **already used** codes.
6. Reject deposit when **no compartment** is available for the required size.
7. Support compartment states: `AVAILABLE`, `OCCUPIED`, `OUT_OF_SERVICE`.

## Non-functional

- In-memory storage (interview scope).
- Pluggable **compartment allocation** strategy (First-Fit provided).
- C++17, modular headers, runnable demo in `main.cpp`.

## Out of scope

- Real Amazon APIs, GPS routing, payment
- Hardware door control / IoT
- Multi-station routing optimization

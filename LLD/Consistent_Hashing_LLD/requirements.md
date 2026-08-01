# Requirements: Consistent Hashing System

## Functional Requirements

1. System should allow adding nodes with a unique id, host and weight.
2. System should allow removing nodes permanently.
3. System should support health updates per node (`ACTIVE` / `DOWN`).
4. System should return the owner node for any key.
5. System should return a replica set of `RF` **distinct physical** nodes for any key.
6. System should skip `DOWN` nodes during lookup without removing them from the ring.
7. System should support pluggable hash functions, swappable at runtime.
8. System should notify registered listeners on node join, leave, health change and ring rebuild.
9. System should report per-node ring ownership share for tuning.
10. System should report ring balance quality (standard deviation and worst-node ratio)
    so virtual node count can be tuned by measurement rather than guesswork.

## Non-Functional Requirements

1. Key lookup should be `O(log V)` where `V` is total virtual node count.
2. Adding or removing a node should move approximately `1/N` of keys, not all of them.
3. Key distribution across nodes should be near-uniform for equal-weight nodes.
4. A node's load share should be proportional to its configured weight.
5. Hash function must be deterministic across processes and runs.
6. Design should be extensible for new hash functions and listeners.
7. Components should follow separation of concerns.
8. Behavior should be deterministic for interview-scale in-memory simulation.

## Assumptions

1. Actual data storage, network transfer and migration execution are out of scope —
   the ring only decides **placement**; listeners only **record** what would migrate.
2. Health checks are manually simulated (real systems use gossip / probes).
3. Persistence and cross-process coordination are out of scope.
4. Single process, in-memory, single-threaded execution model.
5. Hash collisions between two virtual nodes are rare and resolved by keeping the first placement.

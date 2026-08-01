# Problem Statement: Load Balancer System

Design an in-memory load balancer that routes incoming requests across a pool of backend servers.

The system should support:

1. Adding backend servers.
2. Marking server health state (`UP` / `DOWN`).
3. Routing requests using configurable strategy.
4. Tracking active connections on each server.
5. Switching routing strategy at runtime.

Design should be modular so new balancing strategies can be added easily.

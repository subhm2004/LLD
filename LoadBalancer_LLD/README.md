# Load Balancer LLD (C++)

Modular low-level design for an in-memory load balancer supporting pluggable routing strategies.

## Structure

- `main.cpp`
- `core/LoadBalancer.h`
- `models/Server.h`
- `strategies/LoadBalancingStrategy.h`
- `strategies/RoundRobinStrategy.h`
- `strategies/LeastConnectionsStrategy.h`
- `enums/ServerStatus.h`

## Features

- Add backend servers
- Mark server `UP` / `DOWN`
- Route incoming requests
- Track active connections per server
- Switch strategy at runtime:
  - Round Robin
  - Least Connections

## Run

```bash
g++ -std=c++17 main.cpp -o load_balancer_app
./load_balancer_app
```

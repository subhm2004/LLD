# Design Patterns Used in Load Balancer LLD

Load Balancer LLD system me routing algorithms dynamically switch karne, server status nodes coordinate karne, aur server registry ko manage karne ke liye niche diye gaye patterns ka use kiya gaya hai:

---

## 1. Strategy Design Pattern

### Concept
Strategy design pattern algorithms family ko separate interface standard definition me define karta hai, aur client class context me strategies ko runtime par dynamically change karne ki functionality deta hai.

### Implementation
- [LoadBalancingStrategy](file:///Users/shubham/Desktop/LLD/LoadBalancer_LLD/strategies/LoadBalancingStrategy.h) abstract base strategy interface hai.
- Do concrete load balancing strategies is interface ko implement karti hain:
  - [RoundRobinStrategy](file:///Users/shubham/Desktop/LLD/LoadBalancer_LLD/strategies/RoundRobinStrategy.h) (Round robin incremental order sequence allocation)
  - [LeastConnectionsStrategy](file:///Users/shubham/Desktop/LLD/LoadBalancer_LLD/strategies/LeastConnectionsStrategy.h) (Sabse kam active connections wale server ki routing allocation)
- [LoadBalancer](file:///Users/shubham/Desktop/LLD/LoadBalancer_LLD/core/LoadBalancer.h) strategy handler pointer hold karta hai. client methods `setStrategy` call karke dynamic routing logic dynamically switch kar sakte hain.

---

## 2. Facade Design Pattern

### Concept
Facade pattern subsystems aur different helper modules ko wrap karke client applications ke samne ek single simple entrance interface provide karta hai.

### Implementation
- `LoadBalancer` class main facade class ki tarah behave karti hai.
- Client applications direct multiple routing data structure tables check or servers active connections checks handle nahi karti. Client directly LoadBalancer facade ke simplified APIs call karta hai:
  - `addServer(...)`
  - `routeRequest()`
  - `completeRequest(...)`
  - `markServerStatus(...)`

# Requirements: Blinkit-Style Quick Commerce System

## Functional Requirements

1. System should support dark store onboarding with location.
2. Each dark store should maintain inventory by SKU and quantity.
3. User should be able to add products into cart.
4. Order manager should pick a nearby dark store within service radius.
5. During order placement:
   - validate stock
   - reserve/remove stock
   - calculate total amount
6. System should print unfulfilled items if stock is insufficient.
7. System should calculate dynamic delivery fee using distance and surge multiplier.
8. System should support order state transitions:
   - PLACED
   - CONFIRMED
   - PACKING
   - OUT_FOR_DELIVERY
   - DELIVERED
   - CANCELLED

## Non-Functional Requirements

1. Inventory operations should be fast for in-memory usage.
2. Components should be modular and easy to extend.
3. Product creation should be centralized (factory).
4. Design should be deterministic and interview-friendly.

## Assumptions

1. Payment and delivery tracking are out of scope.
2. Real-time traffic-based ETA is out of scope.
3. Persistence/database integration is out of scope.
4. Single process execution model is sufficient.

# Requirements: Vending Machine System

## Functional Requirements

1. System should support multiple item types.
2. Admin/system should be able to load items with price and quantity.
3. User can select an item.
4. User can insert money in multiple steps.
5. System should validate stock availability before dispensing.
6. System should dispense selected item on sufficient balance.
7. System should return change if paid amount is more than item price.
8. User can cancel transaction and get refund.
9. System should expose current inventory details.

## Non-Functional Requirements

1. Design should be modular and easy to extend.
2. Payment and inventory modules should be decoupled.
3. Basic validations should exist for invalid operations.
4. In-memory operations should be fast for demo-level scale.

## Assumptions

1. Single user transaction at a time.
2. Payment gateway/card reader integration is out of scope.
3. Hardware-level concerns (motors, sensors) are out of scope.
4. Data persistence is not required.

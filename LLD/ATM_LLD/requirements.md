# Requirements: ATM System

## Functional Requirements

1. User should be able to authenticate using card number and PIN.
2. User should be able to check account balance.
3. User should be able to withdraw cash.
4. ATM should validate:
   - active card session
   - sufficient account balance
   - sufficient ATM notes
5. ATM should dispense notes by denomination.
6. System should track transaction status (`SUCCESS` / `FAILED`).

## Non-Functional Requirements

1. Operations should be deterministic and testable in-memory.
2. Components should follow clear separation of concerns.
3. System should be easy to extend for deposit, mini statement, and transfer.
4. Validation and error handling should be explicit.

## Assumptions

1. Inter-bank settlement is out of scope.
2. Encryption/security hardening is out of scope.
3. Hardware integration with real ATM machine is out of scope.
4. Persistence/database is out of scope.

# Splitwise LLD - Requirements

## Functional Requirements

1. Create and manage users.
2. Create groups and add/remove members.
3. Add group expenses with split types:
   - Equal
   - Exact
   - Percentage
4. Track balances between all involved users.
5. Allow settlements within a group.
6. Support individual (non-group) expenses.
7. Show user balance summary and group balance matrix.
8. Simplify group debts to minimize transactions.

## Non-Functional Requirements

1. Modular folder structure with header files.
2. `#ifndef / #define / #endif` include guards in all headers.
3. Strategy pattern for split calculation.
4. Factory pattern for split strategy selection.
5. Observer pattern for group notifications.
6. Facade + Singleton for top-level manager.
7. Custom namespace `splitwise_lld`.

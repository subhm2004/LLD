# Requirements: Library Management System

## Functional Requirements

1. Librarian should be able to add books into catalog.
2. System should allow member registration.
3. Member should be able to search books by title.
4. Member should be able to borrow an available book.
5. Member should be able to return a borrowed book.
6. System should calculate fine for late return.
7. System should enforce per-member borrow limit.

## Non-Functional Requirements

1. Design should be modular and easy to extend.
2. Business rules should be deterministic.
3. In-memory operations should be efficient for lookup by IDs and title.
4. Components should have clear separation of concerns.

## Assumptions

1. Authentication and authorization are out of scope.
2. Only in-memory storage is required.
3. Date is represented by integer day count.
4. Payment collection is out of scope, only fine calculation is done.

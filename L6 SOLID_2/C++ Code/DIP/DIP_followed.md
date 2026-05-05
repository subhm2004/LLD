# DIP Followed (Dependency Inversion Principle)

## Overview
This example demonstrates **Dependency Inversion Principle (DIP)**:

- High-level module: `UserService`
- Abstraction: `Database` (interface)
- Low-level modules: `MySQLDatabase`, `MongoDBDatabase`

`UserService` depends on the `Database` abstraction, not on concrete database classes directly.

## Key Points
- `Database` defines a common contract: `save(string data)`.
- `MySQLDatabase` and `MongoDBDatabase` provide different implementations of `save`.
- `UserService` accepts a `Database*` in constructor (Dependency Injection).
- `setDatabase(Database* db)` allows switching dependency at runtime.
- `storeUser(string user)` delegates persistence to `db->save(user)`.

## Why This Follows DIP
- High-level policy (`UserService`) is independent of storage details.
- Low-level classes can be added/changed without modifying business logic.
- New database types can be introduced by implementing `Database`.

## Flow in `main()`
1. Create `MySQLDatabase` and `MongoDBDatabase`.
2. Inject `MySQLDatabase` into one `UserService` object and store `"Aditya"`.
3. Inject `MongoDBDatabase` into another `UserService` object and store `"Rohit"`.
4. Cleanup allocated objects.

## Benefit
This design improves flexibility, testability, and maintainability by reducing tight coupling.

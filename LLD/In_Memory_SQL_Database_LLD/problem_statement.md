# Problem Statement: In-Memory SQL-like Database

Design and implement an **in-memory SQL-like database** that supports table lifecycle, typed columns with constraints, record insert, and simple queries.

## Functional requirements

1. **Create, update, or delete tables** in a database.
2. A **table definition** comprises columns with types and optional constraints:
   - Supported types: **string**, **int**
   - **String** constraint: optional maximum length (e.g. 20 characters)
   - **Int** constraint: optional minimum value (e.g. 1024)
   - **Required** columns (mandatory fields on insert)
3. **Insert** records into a table (all validations enforced).
4. **Print** all records in a table.
5. **Filter** and display records whose column value matches a given value.

## Non-functional (interview scope)

- In-memory only (no disk, no SQL parser).
- C++17, header modules, `compile.sh`.
- Clear separation: schema service, record service, query service, facade.

## Out of scope

- JOIN, GROUP BY, indexes, transactions, persistence, full SQL grammar.

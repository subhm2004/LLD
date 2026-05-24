# Requirements for In-Memory SQL-like Database System

## Functional Requirements

1. **Database and Table Lifecycle**
   - System shall allow creating a table with a unique name and a column definition list.
   - System shall allow updating an existing table schema when the table has no rows.
   - System shall allow deleting a table by name.
   - System shall reject duplicate table names on create.
   - System shall reject operations on non-existent tables.

2. **Column Definition**
   - System shall support column types `string` and `int`.
   - System shall allow marking columns as required (mandatory on insert).
   - System shall allow an optional maximum length constraint on `string` columns.
   - System shall allow an optional minimum value constraint on `int` columns.
   - System shall reject schema definitions with duplicate column names in the same table.

3. **Record Insert**
   - System shall allow inserting a record as a map of column name to value.
   - System shall reject insert when a required column is missing.
   - System shall reject insert when an unknown column is provided.
   - System shall reject `string` values longer than the column maximum length when that constraint is set.
   - System shall reject `int` values below the column minimum when that constraint is set.
   - System shall reject type mismatches (e.g. string value for int column).

4. **Print All Records**
   - System shall print all records in a table in a readable row format.
   - System shall include column headers derived from the table schema.

5. **Filter Records**
   - System shall return and print records where the given column equals the given value.
   - System shall validate that the filter column exists and that the value type matches the column type.
   - System shall return an empty result set when no rows match (not an error).

6. **Facade API**
   - System shall expose all operations through `InMemoryDatabase` (facade).
   - System shall communicate validation failures via exceptions with clear messages.

## Non-Functional Requirements

1. **Modularity and Separation of Concerns**
   - System should maintain `SchemaService`, `RecordService`, and `QueryService`.
   - Column validation logic should remain independent of storage formatting.

2. **Extensibility**
   - System should allow adding new column types or constraints via validator strategy without rewriting query logic.
   - Schema and rows should remain decoupled so future persistence can be plugged in.

3. **Maintainability**
   - Code should remain readable and consistent with other LLD projects in the repository.
   - All implementation code should live under namespace `in_memory_sql_lld`.

4. **Reliability**
   - Invalid operations should fail fast with explicit error messages.
   - Table schema should be immutable during insert except through defined update rules.

5. **Performance (Current Scope)**
   - In-memory linear scan for filter is acceptable for demo dataset sizes.

6. **Portability**
   - System should build with C++17 via `compile.sh` producing `sql_database_app`.

7. **Usability (Developer/Console)**
   - Demo application should illustrate create, update schema, insert, print all, filter, and delete table flows.

## Out of Scope (Current Implementation)

- SQL parser, JOIN, ORDER BY, aggregate functions, indexes, transactions, WAL, networking.

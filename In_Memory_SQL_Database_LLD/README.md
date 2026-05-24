# In-Memory SQL-like Database LLD (C++17)

Minimal **in-memory database** with tables, typed columns, constraints, insert, print-all, and filter-by-column.

## Quick start

```bash
./compile.sh
./sql_database_app
```

## Docs

| File | Purpose |
|------|---------|
| [`problem_statement.md`](./problem_statement.md) | Scope |
| [`requirements.md`](./requirements.md) | FR / NFR |

## Architecture

```
In_Memory_SQL_Database_LLD/
├── core/InMemoryDatabase.h     # Facade
├── services/
│   ├── SchemaService.h         # create / update / delete table
│   ├── RecordService.h         # insert rows
│   └── QueryService.h          # print all, filter
├── validators/ColumnValueValidator.h
├── models/Table.h, ColumnDefinition.h, CellValue.h
└── main.cpp
```

## Main APIs

| API | Description |
|-----|-------------|
| `createTable(name, columns)` | Define schema (string/int, required, constraints) |
| `updateTable(name, columns)` | Replace schema if table empty |
| `deleteTable(name)` | Drop table |
| `insert(table, row)` | Insert validated record |
| `printAll(table)` | Display all rows |
| `printWhere(table, column, value)` | Filter equals + print |

## Column constraints (demo)

| Type | Constraint example |
|------|-------------------|
| `string` | `maxLength = 20` |
| `int` | `minValue = 1024` |
| any | `required = true` |

## Design patterns

| Pattern | Where |
|---------|--------|
| **Facade** | `InMemoryDatabase` |
| **Service layer** | Schema / Record / Query |
| **Validator** | `ColumnValueValidator` |

## Interview extensions

- `SELECT` with multiple WHERE clauses
- B-tree index on column
- Persistence (serialize table to disk)
- Mini SQL parser (`INSERT INTO ...`)

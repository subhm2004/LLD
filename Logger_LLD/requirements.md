# Requirements: Logging Framework LLD

## Functional Requirements

1. System should expose a single logger with `debug()`, `info()`, `warn()`, `error()`, `fatal()` methods.
2. Every log message should carry a **level**, the **message text**, and a **timestamp**.
3. System should route a message through a **handler chain** ordered by severity (`DEBUG` -> `INFO` -> `WARN` -> `ERROR` -> `FATAL`).
4. A handler should process a message only if its level is **greater than or equal to** the handler's configured level, otherwise forward it down the chain.
5. System should allow registering one or more **appenders per level** (e.g. `INFO` -> console, `ERROR` -> console + file).
6. Each appender should render the message through a **formatter** before writing.
7. System should support at least two formatters: **plain text** and **JSON**.
8. System should support at least two appenders: **console** and **file**.

## Non-Functional Requirements

1. Adding a new level, appender, or formatter should not modify existing handler logic (OCP).
2. Logger access should be global and consistent via a **Singleton** (`Logger::getInstance()`).
3. Code should be modular and header-based, compiled via `./compile.sh`.
4. Use `#ifndef / #define / #endif` guards and the `logger_lld` namespace.

## Design Patterns

| Pattern | Where |
| ------- | ----- |
| **Chain of Responsibility** | `handlers/` - `Debug -> Info -> Warn -> Error -> Fatal` |
| **Strategy** | `formatter/` - `PlainTextFormatter`, `JsonFormatter` |
| **Singleton** | `Logger` - single global instance |
| **Bridge-like** | Appender holds a `LogFormatter` reference (output decoupled from format) |

## Assumptions

1. Single-process console/file output is sufficient (no async, no rotation).
2. Level-to-appender configuration is set up in code (`LogHandlerConfiguration`).
3. Whole-message logging only (no structured field maps).

## Out of Scope

- Asynchronous/buffered logging, log rotation, and retention policies.
- Network/syslog sinks and distributed aggregation.

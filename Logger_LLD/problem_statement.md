# Problem Statement: Logging Framework LLD

Design an extensible, production-style **logging framework** (similar to Log4j / SLF4J) that routes a log message to the right destination based on its **severity level**.

The system should support:

- Multiple **log levels** — `DEBUG`, `INFO`, `WARN`, `ERROR`, `FATAL` — with increasing severity.
- A **Chain of Responsibility** of handlers, where each handler decides whether it should process a message of a given level or pass it to the next handler.
- Pluggable **appenders** (output sinks) such as console and file, configurable **per level** (e.g. `INFO` → console, `ERROR` → console **and** file).
- Pluggable **formatters** (Strategy) so the same message can be rendered as plain text or JSON without changing appender code.
- A single, globally accessible **`Logger`** entry point (Singleton) exposing `debug()`, `info()`, `warn()`, `error()`, `fatal()`.

## Scope

- In-memory / file-based, single-process simulation.
- Focus on LLD: separation between message model, level routing, formatting, and output.

## Key Design Objective

Adding a new **output target** (e.g. network/syslog), a new **format** (e.g. XML), or a new **level** should require minimal changes to existing classes (Open/Closed Principle).

## Out of scope

- Async/buffered logging and log rotation.
- Distributed log aggregation, sampling, and structured tracing.
- Thread-safety guarantees beyond the Singleton instance access.

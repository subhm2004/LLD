# Problem Statement: URL Shortner

Design a Low-Level URL Shortner system that converts long URLs into short and shareable links.

The system should support:

- Creating short URL from long URL
- Resolving short URL back to original long URL
- Avoiding duplicate records for same long URL
- Tracking click count for analytics

The goal is to build an extensible C++ design where URL mapping, short-code generation, and request handling are clearly separated.

## Scope

- In-memory implementation
- Single service instance
- Focus on LLD and core behaviors

## Key Design Objective

Keep code generation and URL storage loosely coupled so features like custom aliases, expiration, and distributed ID generation can be added later.

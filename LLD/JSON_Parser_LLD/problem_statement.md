# Problem Statement: JSON Parser

Design a Low-Level JSON Parser that converts a JSON string into an in-memory object model.

The system should support:

- Parsing JSON primitives (string, number, boolean, null)
- Parsing JSON arrays and objects
- Parsing nested JSON structures
- Validating syntax and reporting errors for invalid JSON

The goal is to build an extensible C++ object-oriented design where each JSON type is modeled clearly and the parser logic is separated from data representation.

## Scope

- In-memory parser only
- No file/network I/O in core logic
- Focus on parser design and object model

## Key Design Objective

Implement a clean recursive-descent parser with clear class responsibilities and straightforward extensibility for future features (pretty-printing, serialization, schema checks).

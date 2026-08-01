# JSON Parser LLD (C++)

This folder contains a Low-Level Design implementation of a JSON parser using recursive descent parsing.

## Components

- `JsonValue` (base abstraction for all JSON value types)
- `JsonObject`, `JsonArray`
- Primitive value models:
  - `JsonString`
  - `JsonNumber`
  - `JsonBoolean`
  - `JsonNull`
- `JsonParser`:
  - token-free recursive parsing
  - object/array/value parsing
  - basic syntax validation

## Features

- Parses JSON types:
  - object
  - array
  - string
  - number
  - boolean
  - null
- Supports nested structures
- Handles common escape sequences in strings
- Throws parse errors for invalid input

## Structure

- `main.cpp`
- `core/JsonParserApp.h`
- `parser/JsonParser.h`
- `models/JsonValue.h`
- `models/JsonObject.h`
- `models/JsonArray.h`
- `models/JsonString.h`
- `models/JsonNumber.h`
- `models/JsonBoolean.h`
- `models/JsonNull.h`
- `enums/JsonType.h`

## Run

```bash
g++ -std=c++17 main.cpp -o json_parser_app
./json_parser_app
```

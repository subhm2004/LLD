# Builder Design Pattern — Detailed Guide

> **Creational Pattern** jo **complex object** ko step-by-step banata hai — **fluent API**, optional fields, validation at `build()`. Telescoping constructors aur mutable setters se bachata hai.

**Domain example (is repo mein):** `HttpRequest` — URL, method, headers, body, timeout.

**Files:** `WithoutBuilder.cpp` (problem) → `BuilderPattern.cpp` → `BuilderWithDirector.cpp` → `StepBuilder.cpp`.

---

## Table of Contents

1. [Problem — Telescoping & Mutable Objects](#1-problem--telescoping--mutable-objects)
2. [Builder Pattern kya hai?](#2-builder-pattern-kya-hai)
3. [Variants in This Repo](#3-variants-in-this-repo)
4. [Real-World Analogy](#4-real-world-analogy)
5. [Code Walkthrough](#5-code-walkthrough)
6. [Build & Run](#6-build--run)
7. [vs Factory & Interview](#7-vs-factory--interview)
8. [Summary](#8-summary)

---

## 1. Problem — Telescoping & Mutable Objects

`WithoutBuilder.cpp` — 6 constructors (1-arg … 6-arg) + setters:

| Anti-pattern | Issue |
| ------------ | ----- |
| **Telescoping constructor** | Har optional field = naya constructor |
| **Mutable object** | `setMethod`, `setBody` — half-built request possible |
| **Forgotten fields** | Compile-time guarantee nahi |

---

## 2. Builder Pattern kya hai?

```cpp
HttpRequest request = HttpRequestBuilder()
    .withUrl("https://api.example.com")
    .withMethod("POST")
    .withHeader("Content-Type", "application/json")
    .withBody("{\"name\": \"Aditya\"}")
    .withTimeout(60)
    .build();  // throws if url empty
```

| Piece | Role |
| ----- | ---- |
| **Product** | `HttpRequest` — private ctor |
| **Builder** | `HttpRequestBuilder` — chaining, validation |
| **Director** (optional) | `HttpRequestDirector` — preset recipes |

---

## 3. Variants in This Repo

| File | Technique |
| ---- | --------- |
| `WithoutBuilder.cpp` | Problem demo |
| `BuilderPattern.cpp` | Fluent builder + `build()` validation |
| `BuilderWithDirector.cpp` | `createGetRequest`, `createJsonPostRequest` |
| `StepBuilder.cpp` | Type-state — `UrlStep` → `MethodStep` → `HeaderStep` → `OptionalStep` |

**Step Builder** enforces order at compile time — `withUrl` before `withMethod` required.

---

## 4. Real-World Analogy

- **Custom burger order** — bun, patty, toppings step by step.
- **SQL query builder** — `SELECT().FROM().WHERE()`.
- **Protobuf / test data builders** in production code.

---

## 5. Code Walkthrough

### Fluent Builder

```cpp
class HttpRequestBuilder {
    HttpRequest req;
public:
    HttpRequestBuilder& withUrl(const string& u) { req.url = u; return *this; }
    HttpRequest build() {
        if (req.url.empty()) throw runtime_error("URL cannot be empty");
        return req;
    }
};
```

### Director

```cpp
static HttpRequest createJsonPostRequest(const string& url, const string& jsonBody) {
    return HttpRequestBuilder()
        .withUrl(url).withMethod("POST")
        .withHeader("Content-Type", "application/json")
        .withBody(jsonBody).build();
}
```

---

## 6. Build & Run

```bash
cd "L28 Builder_design_pattern/C++ Code"
g++ -std=c++17 -o without_demo WithoutBuilder.cpp && ./without_demo
g++ -std=c++17 -o builder_demo BuilderPattern.cpp && ./builder_demo
g++ -std=c++17 -o director_demo BuilderWithDirector.cpp && ./director_demo
g++ -std=c++17 -o step_demo StepBuilder.cpp && ./step_demo
```

---

## 7. vs Factory & Interview

| Builder | Factory |
| ------- | ------- |
| **Many optional steps** | **Which concrete type** to create |
| Same product, different configs | Polymorphic product families |

**Interview:** "Builder = complex construction; Factory = creation hiding."

---

## 8. Summary

| Pehlu | Detail |
| ----- | ------ |
| **Type** | Creational |
| **Solves** | Telescoping ctor, inconsistent half-built objects |
| **Key files** | `BuilderPattern.cpp`, `StepBuilder.cpp` |

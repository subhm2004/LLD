# PR: Explain Encapsulation and Abstraction with Car Example

## Overview
This change set demonstrates two core OOP principles in C++ using a consistent `Car`/`SportsCar` domain model:
- `Encapsulation.cpp` shows data hiding with controlled access via public methods.
- `Abstraction.cpp` shows interface-driven design using an abstract base class and runtime polymorphism.

## Why this is useful
- Helps beginners connect OOP theory to practical, runnable code.
- Shows the difference between **protecting internal state** (encapsulation) and **exposing only behavior contracts** (abstraction).
- Uses almost identical car operations in both files, making conceptual comparison easy.

## File-wise Notes

### `Encapsulation.cpp`
- Keeps data members like `currentSpeed`, `currentGear`, and `isEngineOn` as `private`.
- Exposes safe operations through methods such as `startEngine()`, `shiftGear()`, `accelerate()`, `brake()`, `stopEngine()`.
- Uses `getSpeed()` and `setTyreCompany()` to show controlled read/write access.
- Prevents direct external mutation of critical state (for example, speed cannot be arbitrarily set from `main`).

### `Abstraction.cpp`
- Defines abstract class `Car` with pure virtual methods (`startEngine`, `shiftGear`, `accelerate`, `brake`, `stopEngine`).
- Implements the contract in concrete class `SportsCar`.
- Demonstrates abstraction in `main` using `Car* myCar = new SportsCar(...)`.
- Focuses on **what** operations are available through interface `Car`, while `SportsCar` handles **how** those operations work.

## Concepts Demonstrated
- Class design and object lifecycle (`new` / `delete`)
- Access modifiers (`private`, `public`)
- Getter/setter based controlled access
- Pure virtual functions and abstract classes
- Inheritance and method overriding
- Runtime polymorphism via base-class pointer

## Suggested Follow-ups
- Add input validation in `shiftGear(int gear)` (e.g., invalid gear ranges).
- Use smart pointers (`std::unique_ptr`) to avoid manual memory management.
- Mark overridden methods with `override` keyword for better compile-time safety.

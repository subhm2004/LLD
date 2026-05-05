# NoSingleton (`NoSingleton.cpp`)

## Concept

This example is intentionally **not** a Singleton.  
It shows the normal behavior of a class where multiple objects can be created freely.

In Singleton design pattern terms, this file acts as a baseline so you can compare it with actual Singleton implementations.

---

## What the code does

- Defines class `NoSingleton` with a public constructor.
- In `main()`, it creates:
  - `NoSingleton* s1 = new NoSingleton();`
  - `NoSingleton* s2 = new NoSingleton();`
- Prints `(s1 == s2)`.

Because `s1` and `s2` point to two different heap objects, output is `0` (false).

---

## Why this is not Singleton

A true Singleton must enforce:

1. Exactly one instance for the whole application.
2. A global access point to that instance.
3. Controlled object creation (usually private constructor).

`NoSingleton` violates all of these:

- Constructor is public.
- No static instance is maintained.
- No `getInstance()` method exists.

---

## Key learning

If a constructor is publicly accessible and no creation control is present, any code can create as many objects as it wants.  
This is the default object-oriented behavior and the opposite of Singleton.

---

## Advantages of this approach

- Very simple.
- No global shared state.
- No lock or static-initialization complexity.

---

## Limitations (when singleton is needed)

- Cannot guarantee single shared object.
- Wasteful if object creation is expensive and you wanted only one copy.
- Different parts of code may end up using different instances unintentionally.

---

## When to use

Use this pattern (normal class instantiation) when:

- Multiple independent instances are expected.
- Shared global state is not required.
- You want better testability and lower coupling.

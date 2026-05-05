# Simple Lazy Singleton (`SimpleSingleton.cpp`)

## Concept

This file implements a **basic lazy Singleton**.

Lazy means the object is created only when it is first requested, not at program start.

---

## How Singleton is enforced here

The class `Singleton` uses:

1. `private` static pointer `instance`  
   - Stores the single object reference.
2. `private` constructor  
   - Prevents external code from creating objects directly.
3. `public static Singleton* getInstance()`  
   - Global access point to the one object.

Code flow inside `getInstance()`:

- If `instance == nullptr`, create a new object.
- Return `instance`.

So first call creates object; later calls return the same object.

---

## Why `(s1 == s2)` is true

Both:

- `s1 = Singleton::getInstance();`
- `s2 = Singleton::getInstance();`

receive the same stored pointer `instance`, so pointer comparison prints `1` (true).

---

## Strengths

- Easy to understand and implement.
- Avoids object creation if singleton is never used.
- Clear demonstration of Singleton basics.

---

## Important limitation: not thread-safe

If two threads call `getInstance()` at the same time when `instance` is `nullptr`, both might create separate objects before either write is visible to the other.

That breaks Singleton guarantee in multithreaded execution.

So this version is safe only in:

- Single-threaded programs, or
- Cases where you can guarantee serialized access during first initialization.

---

## Memory/lifetime note

Object is created with `new` and never `delete`d in this sample.  
For production code, you should define ownership/lifetime strategy to avoid leaks (or prefer static local instance approach).

---

## When to use

Use this version mainly for:

- Learning Singleton mechanics.
- Single-threaded demos or interview basics.

For concurrent code, prefer thread-safe variants.

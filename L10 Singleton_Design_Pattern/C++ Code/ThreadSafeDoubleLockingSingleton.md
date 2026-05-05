# Thread-Safe Double-Checked Locking Singleton (`ThreadSafeDoubleLockingSingleton.cpp`)

## Concept

This file uses **double-checked locking (DCL)** to implement a thread-safe lazy Singleton with lower locking overhead than full-lock-on-every-call.

Idea:

- First check without lock (fast path).
- Lock only if object might need creation.
- Check again inside lock (safety check).

---

## How the code works

`getInstance()` logic:

1. `if (instance == nullptr)`  
   First check without locking.
2. If true, acquire mutex with `lock_guard<mutex> lock(mtx);`.
3. Inside lock, check again `if (instance == nullptr)`.
4. Create object if still null.
5. Return `instance`.

The second check is essential because another thread may have created the object between first check and lock acquisition.

---

## Why this improves performance

After singleton is initialized:

- First null check fails immediately.
- No lock acquisition occurs.

So repeated `getInstance()` calls can be faster compared with always-locking approach.

---

## Correctness considerations

Double-checked locking is conceptually correct here as a teaching example, but in production C++ you should be careful about memory model details and publication safety.

A safer modern pattern is:

- Function-local static (Meyers Singleton), which is thread-safe in C++11+ by language guarantee.

Still, this sample clearly demonstrates DCL intent and structure.

---

## Strengths

- Thread-safe lazy creation.
- Less lock contention after initialization.
- Demonstrates optimized synchronization pattern.

---

## Limitations

- More complex than simple lock-based version.
- Easy to implement incorrectly in low-level concurrent code.
- Uses heap allocation without explicit cleanup in sample.

---

## When to use

Use when:

- You need lazy singleton in multi-threaded context.
- `getInstance()` is frequently called and lock overhead matters.
- Team is comfortable with concurrency subtleties.

Otherwise, prefer simpler and safer alternatives in modern C++.

# Thread-Safe Locking Singleton (`ThreadSafeLockingSingleton.cpp`)

## Concept

This file implements a **thread-safe lazy Singleton** using a mutex lock on every call to `getInstance()`.

Lazy initialization is preserved (object is created only when needed), and thread safety is added with synchronization.

---

## How it works

`Singleton` contains:

- `static Singleton* instance;`
- `static mutex mtx;`
- Private constructor.
- `static Singleton* getInstance()`.

Inside `getInstance()`:

1. `lock_guard<mutex> lock(mtx);` acquires lock.
2. Check `if (instance == nullptr)`.
3. Create object once if needed.
4. Return pointer.

Because lock is held during check-and-create, only one thread can execute this critical section at a time.

---

## Why this is thread-safe

The race condition from simple lazy Singleton is removed:

- Multiple threads may call `getInstance()`.
- But only one thread can enter the locked section at once.
- First thread creates object.
- Others see non-null `instance` and reuse it.

So only one object is created.

---

## Performance trade-off

This design locks on **every call**, even after initialization is complete.

That means:

- Correctness is strong and simple.
- But there is unnecessary synchronization overhead for repeated access in highly concurrent hot paths.

---

## Strengths

- Straightforward and reliable.
- Easy to reason about.
- Good for moderate traffic or non-performance-critical paths.

---

## Limitations

- Higher overhead than double-checked approaches once instance already exists.
- Uses dynamic allocation with no explicit destruction in sample.

---

## When to use

Choose this approach when:

- You need thread safety.
- You prefer simpler implementation over micro-optimization.
- `getInstance()` is not a very hot path.

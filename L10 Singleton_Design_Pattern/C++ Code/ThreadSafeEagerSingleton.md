# Thread-Safe Eager Singleton (`ThreadSafeEagerSingleton.cpp`)

## Concept

This file uses **eager initialization**:

- Singleton instance is created at static initialization time (before first call to `getInstance()`).
- `getInstance()` simply returns already-created pointer.

---

## How it works

Class setup:

- `static Singleton* instance;`
- Private constructor.
- `static Singleton* getInstance() { return instance; }`

Static definition:

- `Singleton* Singleton::instance = new Singleton();`

This line creates the object during static initialization phase, so runtime method calls only fetch it.

---

## Why it is thread-safe (in practice)

Because object is created before application threads typically start using it, no runtime race occurs around first-time creation.

So this avoids locking complexity in `getInstance()`.

---

## Performance characteristics

- Very fast access (`getInstance()` just returns pointer).
- No mutex overhead.
- No lazy behavior: instance is created even if never used.

---

## Strengths

- Simple runtime access path.
- No double-check logic.
- No lock contention.

---

## Limitations

- Possible unnecessary initialization cost at startup.
- Static initialization order concerns may appear across multiple translation units in larger codebases.
- Dynamic allocation and destruction policy still need explicit handling in production code.

---

## When to use

Prefer eager singleton when:

- Startup cost is acceptable.
- Instance is almost certainly needed anyway.
- You want simpler thread-safe access without runtime synchronization.

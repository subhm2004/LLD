# Singleton Notes (C++ Code)

Yeh folder `C++ Code` ke sab singleton demo files ka detailed Hinglish explanation deta hai.

## Notes Index

- `MASTER_DETAILED_SINGLETON_NOTES.md` - super detailed beginner-friendly master notes
- `NoSingleton.md` - why normal class singleton nahi hoti
- `SimpleSingleton.md` - basic lazy singleton (single-thread focus)
- `MeyersSingleton.md` - modern recommended singleton (function-local static)
- `ThreadSafeEagerSingleton.md` - eager initialization singleton
- `ThreadSafeLockingSingleton.md` - mutex locking with lazy init
- `ThreadSafeDoubleLockingSingleton.md` - double-checked locking approach
- `IMAGES.md` - diagrams/images ke liye section

## Suggested Reading Order

1. `NoSingleton.md`
2. `SimpleSingleton.md`
3. `MeyersSingleton.md`
4. `ThreadSafeEagerSingleton.md`
5. `ThreadSafeLockingSingleton.md`
6. `ThreadSafeDoubleLockingSingleton.md`

## Quick Interview Lines

- Singleton ka main goal: "only one object globally + global access point".
- Basic lazy singleton thread-safe nahi hota.
- Thread-safe banane ke common tareeke:
  - Eager initialization
  - Mutex locking
  - Double-checked locking


# Singleton Design Pattern (L10) - Updated Guide

> Yeh README `L10 Singleton_Design_Pattern` ke latest folder structure, code variants, notes, aur diagrams ke hisaab se update kiya gaya hai.

---

## What is Singleton?

Singleton ek creational pattern hai jo ensure karta hai:

1. Class ka sirf **one instance** bane
2. Us instance ka **global access point** ho (`getInstance()`)

Typical use-cases:
- Logger
- Config manager
- Connection pool manager
- Central coordinator service

---

## Variants in This Folder

`C++ Code` me ab following implementations hain:

| File | Type | Thread Safety | Notes |
|---|---|---|---|
| `NoSingleton.cpp` | Baseline (anti-singleton) | N/A | Multiple objects allowed |
| `SimpleSingleton.cpp` | Lazy pointer singleton | No | Single-thread demo |
| `ThreadSafeEagerSingleton.cpp` | Eager singleton | Yes (init time) | Startup pe object create |
| `ThreadSafeLockingSingleton.cpp` | Lazy + mutex lock every call | Yes | Safe, but lock overhead |
| `ThreadSafeDoubleLockingSingleton.cpp` | Lazy + double-checked lock | Conceptually yes | Optimized lock path |
| `MeyersSingleton.cpp` | Function-local static | Yes (C++11+) | Modern recommended style |

---

## Quick Recommendation

Modern C++ interview / production answer:

> Prefer **Meyers Singleton** (`static local instance`) unless specific pointer-based pattern dikhana ho.

---

## Folder Structure (Current)

```text
L10 Singleton_Design_Pattern/
├── README.md
└── C++ Code/
    ├── NoSingleton.cpp
    ├── SimpleSingleton.cpp
    ├── ThreadSafeEagerSingleton.cpp
    ├── ThreadSafeLockingSingleton.cpp
    ├── ThreadSafeDoubleLockingSingleton.cpp
    ├── MeyersSingleton.cpp
    ├── Markdown.md
    └── notes/
        ├── README.md
        ├── MASTER_DETAILED_SINGLETON_NOTES.md
        ├── NoSingleton.md
        ├── SimpleSingleton.md
        ├── ThreadSafeEagerSingleton.md
        ├── ThreadSafeLockingSingleton.md
        ├── ThreadSafeDoubleLockingSingleton.md
        ├── MeyersSingleton.md
        ├── IMAGES.md
        ├── class_diagram_nosingleton.png
        ├── sequence_diagram_nosingleton.png
        ├── class_diagram_simpleSingleton.png
        ├── sequence_diagram_sinpleSingleton.png
        ├── class_diagram_ThreadSafeEagerSingleton.png
        ├── sequence_diagram_ThreadSafeEagerSingleton.png
        ├── Class_diagram_ThreadSafeLockingSingleton.png
        ├── Sequence_diagram_ThreadSafeLockingSingleton.png
        ├── class_diagram_ThreadSafeDoubleLockingSingleton.png
        └── Sequence_diagram_ThreadSafeDoubleLockingSingleton.png
```

---

## Notes (Detailed Reading)

### Start here
- [`C++ Code/notes/README.md`](./C%20%2B%2B%20Code/notes/README.md)
- [`C++ Code/notes/MASTER_DETAILED_SINGLETON_NOTES.md`](./C%20%2B%2B%20Code/notes/MASTER_DETAILED_SINGLETON_NOTES.md)

### Variant-wise deep notes
- [`NoSingleton.md`](./C%20%2B%2B%20Code/notes/NoSingleton.md)
- [`SimpleSingleton.md`](./C%20%2B%2B%20Code/notes/SimpleSingleton.md)
- [`ThreadSafeEagerSingleton.md`](./C%20%2B%2B%20Code/notes/ThreadSafeEagerSingleton.md)
- [`ThreadSafeLockingSingleton.md`](./C%20%2B%2B%20Code/notes/ThreadSafeLockingSingleton.md)
- [`ThreadSafeDoubleLockingSingleton.md`](./C%20%2B%2B%20Code/notes/ThreadSafeDoubleLockingSingleton.md)
- [`MeyersSingleton.md`](./C%20%2B%2B%20Code/notes/MeyersSingleton.md)

---

## Diagrams

Diagrams `C++ Code/notes/` folder me available hain:

- NoSingleton (class + sequence)
- SimpleSingleton (class + sequence)
- ThreadSafeEagerSingleton (class + sequence)
- ThreadSafeLockingSingleton (class + sequence)
- ThreadSafeDoubleLockingSingleton (class + sequence)

Image references list:
- [`IMAGES.md`](./C%20%2B%2B%20Code/notes/IMAGES.md)

---

## Build & Run

> Har file me class name `Singleton` same hai, isliye har cpp ko alag compile/run karo.

```bash
cd "L10 Singleton_Design_Pattern/C++ Code"

g++ -std=c++17 -o no_singleton_demo NoSingleton.cpp && ./no_singleton_demo
g++ -std=c++17 -o simple_singleton_demo SimpleSingleton.cpp && ./simple_singleton_demo
g++ -std=c++17 -o eager_singleton_demo ThreadSafeEagerSingleton.cpp && ./eager_singleton_demo
g++ -std=c++17 -o locking_singleton_demo ThreadSafeLockingSingleton.cpp && ./locking_singleton_demo
g++ -std=c++17 -o dcl_singleton_demo ThreadSafeDoubleLockingSingleton.cpp && ./dcl_singleton_demo
g++ -std=c++17 -o meyers_singleton_demo MeyersSingleton.cpp && ./meyers_singleton_demo
```

---

## Interview Quick Lines

1. Singleton = one instance + global access point.
2. Naive lazy singleton thread-safe nahi hota.
3. Mutex locking safe but lock cost high.
4. DCL optimized hota hai, but careful implementation chahiye.
5. Modern C++ default recommendation: **Meyers Singleton**.

---

## Final Summary

Is folder me ab singleton ka complete progression hai:

`NoSingleton -> Simple -> Eager -> Locking -> Double-Checked -> Meyers`

Aur har variant ka detailed explanation + diagrams `C++ Code/notes/` me available hai.

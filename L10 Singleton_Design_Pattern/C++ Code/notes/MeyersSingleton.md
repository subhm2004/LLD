# `MeyersSingleton.cpp` - Super Detailed Notes

## 1) Meyers Singleton kya hota hai?

Meyers Singleton ek modern C++ singleton style hai jisme instance ko function ke andar `static` local variable ke form me banaya jata hai.

Core line:

```cpp
static Singleton instance;
```

Yahi line pattern ka heart hai.

---

## 2) Isko Meyers kyu bolte hain?

Scott Meyers ne Effective C++ context me is approach ko strongly recommend kiya tha. Tab se industry me is style ko informal naam mila: **Meyers Singleton**.

---

## 3) Yeh previous pointer-based singleton se better kyu mana jata hai?

Pointer-based lazy singleton me usually:

- manual `new`
- static pointer define
- memory lifetime management
- thread-safety ke liye extra mutex logic

Meyers singleton me:

- no manual `new`
- no explicit pointer storage
- no explicit destroy logic
- C++11+ me initialization thread-safe

---

## 4) File walkthrough

## Includes

- `#include <bits/stdc++.h>`: demo convenience include.

## Class `Singleton`

### Private constructor

```cpp
Singleton() { ... }
```

Bahar se `Singleton obj;` prevent hota hai.

### Deleted copy/move

```cpp
Singleton(const Singleton &) = delete;
Singleton &operator=(const Singleton &) = delete;
Singleton(Singleton &&) = delete;
Singleton &operator=(Singleton &&) = delete;
```

Inka purpose:

- singleton clone/copy/move se duplicate object creation block.

### `getInstance()`

```cpp
static Singleton &getInstance() {
    static Singleton instance;
    return instance;
}
```

Flow:

1. First call:
   - `instance` create hota hai.
2. Next calls:
   - same `instance` return hota hai.

### `sayHello()`

Simple member method hai jisse test kar pao ki returned object usable hai.

---

## 5) `main()` dry run

1. `Singleton &s1 = Singleton::getInstance();`
   - first call, constructor once.
2. `Singleton &s2 = Singleton::getInstance();`
   - second call, no new constructor.
3. `&s1 == &s2`
   - true => same object.

---

## 6) Thread-safety detail (important)

Since C++11, local static initialization is guaranteed thread-safe.

Meaning:

- Agar do threads first time same function call karein,
- to runtime ensure karta hai object exactly once initialize ho.

Isliye Meyers singleton typical use-case me thread-safe mana jata hai.

---

## 7) Pros

1. Clean and short
2. No manual memory management
3. No explicit mutex needed for initialization
4. Very interview-friendly
5. Less bug-prone than manual DCL pointer code

---

## 8) Cons / caveats

1. Global state problems still apply (singleton ka common downside).
2. Unit testing me hidden shared state issues aa sakte hain.
3. Destruction order across multiple static objects can be tricky in very complex systems.

---

## 9) Meyers vs other variants

| Variant | Init time | Thread-safe init | Complexity |
|---|---|---|---|
| Simple lazy (pointer) | first use | no | low |
| Eager | startup | yes (creation) | low |
| Locking lazy | first use | yes | medium |
| Double-checked locking | first use | tricky | high |
| **Meyers** | first use | **yes (C++11+)** | **low** |

---

## 10) Interview answer ready line

> "In modern C++, my default singleton implementation is Meyers singleton using function-local static, because it is concise and thread-safe by language guarantee since C++11."


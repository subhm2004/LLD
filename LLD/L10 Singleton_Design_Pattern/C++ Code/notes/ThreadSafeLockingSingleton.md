# `ThreadSafeLockingSingleton.cpp` - Super Detailed Notes

## 1) Pattern summary

Yeh version:

- lazy bhi hai (object demand pe banta hai)
- thread-safe bhi hai (mutex lock ke saath)

Approach:

> "Har getInstance call pe lock lo, phir check/create karo."

---

## 2) Core members

### `static Singleton *instance;`

- singleton object pointer
- initially null

### `static mutex mtx;`

- shared lock object
- multiple threads ko serialize karta hai

### private constructor

- direct creation blocked

### deleted copy/move

- clone via copy/move blocked
- singleton rule hard enforced

---

## 3) `lock_guard` deep meaning

Code:

```cpp
lock_guard<mutex> lock(mtx);
```

Is one line ka behavior:

1. line execute -> `mtx.lock()`
2. scope end -> auto `mtx.unlock()`

Kyu useful?

- manual unlock bhoolne ka risk kam
- exception aaye tab bhi unlock ho jata hai (RAII behavior)

---

## 4) `getInstance()` exact execution flow

Every call:

1. lock acquire
2. `if(instance == nullptr)` check
3. null ho to create
4. pointer return
5. auto unlock

Race condition kyu nahi?

- ek time pe sirf ek thread lock ke andar
- isliye duplicate creation impossible

---

## 5) Performance angle

Safe hai, but:

- object already ban chuka ho tab bhi lock lagega
- high-frequency access me overhead

So this is:

- correctness-first implementation
- performance-second

---

## 6) Example scenario

Do threads T1, T2 same time call karti hain:

- T1 lock le leti hai
- T2 wait me rahti hai
- T1 instance create karti hai, return karti hai, unlock
- T2 lock leke check karti hai, instance null nahi, same pointer return

Output:

- constructor once
- `s1 == s2` true

---

## 7) Pros vs cons

Pros:

- easy to reason
- strong thread safety
- maintainable

Cons:

- unnecessary locking after initialization
- scaling issue in high-concurrency reads

---

## 8) Kab use karein?

Use when:

- simplicity and correctness priority
- performance not extreme bottleneck

Prefer alternatives when:

- many threads and frequent reads
- need lock-free fast path (double-check or static local)

---

## 9) Interview-ready explanation

> "This is coarse-grained locking singleton: every accessor call locks mutex, guaranteeing safe lazy initialization, but paying lock overhead even after instance is created."


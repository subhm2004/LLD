# Flyweight Design Pattern — Detailed Guide

> **Structural Pattern** jo **shared intrinsic state** use karke memory bachata hai — bahut saare objects mein **same heavy data** (texture, color) **ek baar** store; **unique extrinsic state** (position, velocity) alag per instance.

**Domain example (is repo mein):** Space game **asteroids** — `WithFlyWeight.cpp` vs `WithoutFlyWeight.cpp` (1M objects memory compare).

---

## Table of Contents

1. [Problem — Memory Duplication](#1-problem--memory-duplication)
2. [Flyweight Pattern kya hai?](#2-flyweight-pattern-kya-hai)
3. [Intrinsic vs Extrinsic State](#3-intrinsic-vs-extrinsic-state)
4. [Code Walkthrough](#4-code-walkthrough)
5. [Memory Comparison](#5-memory-comparison)
6. [Build & Run](#6-build--run)
7. [Interview & Summary](#7-interview--summary)

---

## 1. Problem — Memory Duplication

`WithoutFlyWeight.cpp` — har `Asteroid` apna color, texture, material **copy** karta hai:

```cpp
// ❌ 1M asteroids = 1M × (length, width, color, texture, material, pos, vel)
new Asteroid(length, width, weight, color, texture, material, posX, posY, velX, velY);
```

**3 asteroid types** (Red/Rocky/Iron, Blue/Metallic/Stone, Gray/Icy/Ice) — lekin **1M full objects**.

---

## 2. Flyweight Pattern kya hai?

```
AsteroidFactory (cache) → AsteroidFlyweight (shared, intrinsic)
AsteroidContext (per instance, extrinsic: pos + velocity)
```

| Component | Role |
| --------- | ---- |
| **Flyweight** | `AsteroidFlyweight` — shared immutable data |
| **Flyweight Factory** | `AsteroidFactory::getAsteroid()` — key = type string, reuse |
| **Context** | `AsteroidContext` — pointer to flyweight + position/velocity |

```cpp
AsteroidFlyweight* fw = AsteroidFactory::getAsteroid(...);  // max 3 unique
asteroids.push_back(new AsteroidContext(fw, posX, posY, velX, velY));
```

---

## 3. Intrinsic vs Extrinsic State

| State | Examples | Storage |
| ----- | -------- | ------- |
| **Intrinsic (shared)** | length, width, color, texture, material | Flyweight — factory cached |
| **Extrinsic (unique)** | posX, posY, velocityX, velocityY | Context per asteroid |

**Rule:** Extrinsic state **flyweight method ko pass** karo — `render(posX, posY, velX, velY)`.

---

## 4. Code Walkthrough

Source: [`C++ Code/WithFlyWeight.cpp`](./C%20%2B%2B%20Code/WithFlyWeight.cpp)

```cpp
static AsteroidFlyweight* getAsteroid(...) {
    string key = to_string(length) + "_" + color + "_" + texture + ...;
    if (flyweights.find(key) == flyweights.end())
        flyweights[key] = new AsteroidFlyweight(...);
    return flyweights[key];
}
```

**Spawn loop:** `i % 3` → only **3 flyweight types** for 1M contexts.

---

## 5. Memory Comparison

Demo uses `ASTEROID_COUNT = 1000000`:

| Approach | Flyweight objects | Per-instance size |
| -------- | ------------------- | ----------------- |
| **Without** | 0 (all data per object) | ~full Asteroid struct |
| **With** | **3** shared + 1M small contexts | pointer + 4 ints |

Run both (enough RAM):

```bash
g++ -std=c++17 -o without_fw WithoutFlyWeight.cpp && ./without_fw
g++ -std=c++17 -o with_fw WithFlyWeight.cpp && ./with_fw
```

**With flyweight:** `Total flyweight objects: 3` — intrinsic state shared.

---

## 6. Build & Run

```bash
cd "L30 Flyweight_design_pattern/C++ Code"
g++ -std=c++17 -o with_fw WithFlyWeight.cpp && ./with_fw
g++ -std=c++17 -o without_fw WithoutFlyWeight.cpp && ./without_fw
```

> 1M objects — ensure sufficient memory; reduce count for quick test.

---

## 7. Interview & Summary

1. **One-liner:** "Flyweight shares intrinsic state across many fine-grained objects; extrinsic state passed in."
2. **Factory cache:** Key by intrinsic properties — avoid duplicate flyweights.
3. **vs Singleton:** Flyweight = many shared **types**; Singleton = one instance total.
4. **Use when:** Huge count, few unique intrinsic variants (text glyphs, map tiles, particles).

| Pehlu | Detail |
| ----- | ------ |
| **Type** | Structural |
| **Files** | `WithFlyWeight.cpp`, `WithoutFlyWeight.cpp` |

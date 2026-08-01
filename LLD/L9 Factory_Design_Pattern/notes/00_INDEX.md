# L9 Factory Design Pattern — Notes Index

> **Factory = object creation ka pattern family.** Client khud `new ConcreteClass()` nahi karta — factory se maangta hai. Is folder me **teen levels** ki poori journey hai: Simple Factory → Factory Method → Abstract Factory. Har level pichle wale ki ek problem solve karta hai.

---

## Ye notes kaise padhein?

Ye ek **evolution story** hai — order me padhna zaroori hai, kyunki har pattern pichle wale ke weakness se motivate hota hai:

```
Problem: main() me har jagah `new BasicBurger()` — tight coupling
    │
    ▼
01 Simple Factory      → "creation ko EK class me daal do"
    │  (weakness: naya type = factory edit — OCP break)
    ▼
02 Factory Method      → "factory ko abstract karo, SUBCLASS decide kare"
    │  (weakness: sirf EK product line — combo/family nahi)
    ▼
03 Abstract Factory    → "poori RELATED FAMILY ek factory se — theme consistency"
    │
    ▼
04 Comparison + SOLID  → teeno ka revision + interview pack
```

---

## Topics

| # | Topic | Notes File | Code File | Ek line me |
|---|--------|------|------|------------|
| 1 | Simple Factory | [01_Simple_Factory.md](./01_Simple_Factory.md) | [`SimpleFactory.cpp`](../C++%20Code/SimpleFactory.cpp) | Ek concrete factory, andar if-else |
| 2 | Factory Method | [02_Factory_Method.md](./02_Factory_Method.md) | [`FactoryMethod.cpp`](../C++%20Code/FactoryMethod.cpp) | Abstract factory, subclass (brand) decide kare |
| 3 | Abstract Factory | [03_Abstract_Factory.md](./03_Abstract_Factory.md) | [`AbstractFactory.cpp`](../C++%20Code/AbstractFactory.cpp) | Ek factory se poori product family (burger + bread) |
| 4 | Comparison + SOLID + Interview | [04_Comparison_SOLID_Interview.md](./04_Comparison_SOLID_Interview.md) | — | Teeno side-by-side + Q&A bank |

---

## Domain example (teeno files me same)

**Burger shop story:**
- **Products:** `BasicBurger` / `StandardBurger` / `PremiumBurger` — aur inke **wheat** versions
- **Brands:** `SinghBurger` (normal bun) vs `KingBurger` (wheat bun)
- **Combo (sirf Abstract Factory me):** Burger + `GarlicBread` — dono same theme ke

---

## One-line memory (exam se pehle yahi dekh lo)

| Pattern | Yaad rakho | GoF official? |
|---------|------------|---------------|
| **Simple Factory** | Ek factory class, andar if-else — creation centralize | ❌ Idiom hai |
| **Factory Method** | `virtual createBurger()` — brand/subclass decide kare | ✅ Haan |
| **Abstract Factory** | Multiple `create*()` — poori family ek theme ki | ✅ Haan |

**Ek aur shortcut:**
- Simple = **"kahan** banau?" (ek jagah)
- Method = **"kaun** banaye?" (subclass)
- Abstract = **"kya-kya saath** banau?" (family)

---

## Master guide & extras

- [`../README.md`](../README.md) — complete L9 guide (TOC, diagrams, build instructions, repo cross-links)
- [`../C++ Code/Markdown.md`](../C++%20Code/Markdown.md) — quick comparison cheat sheet
- [`../compile.sh`](../compile.sh) — teeno demos ek saath build karne ke liye

## Build & run (sab demos)

```bash
cd "L9 Factory_Design_Pattern" && chmod +x compile.sh && ./compile.sh
./C++\ Code/simple_factory_demo
./C++\ Code/factory_method_demo
./C++\ Code/abstract_factory_demo
```

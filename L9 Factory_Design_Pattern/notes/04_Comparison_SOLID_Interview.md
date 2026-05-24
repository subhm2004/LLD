# Factory Family — Comparison, SOLID & Interview Pack

> Teen patterns ek saath — revision sheet + interview answers.

---

## 1. Master comparison table

| Feature | **Simple Factory** | **Factory Method** | **Abstract Factory** |
|---------|-------------------|--------------------|-----------------------|
| **GoF catalog** | ❌ Idiom | ✅ Yes | ✅ Yes |
| **Creates** | One product type | One product type | **Multiple related** products |
| **Who decides class** | Central if-else | **Subclass** override | Concrete factory (whole family) |
| **Main mechanism** | `createBurger(type)` | `virtual createBurger()` | `createBurger()` + `createGarlicBread()` |
| **OCP (new product)** | Edit factory | New product class | New product + maybe factory method |
| **OCP (new brand)** | Edit factory | New factory subclass | New `MealFactory` subclass |
| **Client depends on** | Concrete `BurgerFactory` | `BurgerFactory*` | `MealFactory*` |
| **Family consistency** | ❌ Manual | ❌ Manual | ✅ Built-in |
| **Complexity** | Low | Medium | Higher |
| **Repo file** | `SimpleFactory.cpp` | `FactoryMethod.cpp` | `AbstractFactory.cpp` |

---

## 2. Evolution diagram (is course)

```mermaid
flowchart LR
    A[Client uses new everywhere] --> B[Simple Factory central if-else]
    B --> C[Factory Method per brand subclass]
    C --> D[Abstract Factory burger + bread family]
```

---

## 3. SOLID — pattern-wise

### Open-Closed Principle

> Open for extension, closed for modification.

| Pattern | Naya `VeggieBurger` | Verdict |
|---------|---------------------|---------|
| Simple | `BurgerFactory::createBurger` + `else if` | ❌ Modify |
| Factory Method | `class VeggieBurger` + factory override | ✅ Extend |
| Abstract Factory | New class + update factory methods | ✅ Mostly extend |

### Dependency Inversion Principle

> Depend on abstractions, not concretions.

```cpp
// ✅ Good — high-level depends on abstraction
BurgerFactory* factory = new SinghBurger();
Burger* b = factory->createBurger(type);
```

| Pattern | DIP strength |
|---------|--------------|
| Simple | Medium — products abstract, factory concrete |
| Factory Method | Strong |
| Abstract Factory | Strong |

### Single Responsibility Principle

- **Creation** alag factory mein
- **Behavior** product (`prepare()`) mein
- **Orchestration** client (`main`) mein

Teeno patterns ✅ SRP improve karte hain vs fat `main`.

### Liskov Substitution

Koi bhi `BurgerFactory` / `MealFactory` subclass client ke liye interchangeable — jab tak contract same (`create*` return valid products).

### Interface Segregation

Products ko chhoti interfaces — `Burger`, `GarlicBread` alag; fat `IMeal` avoid.

---

## 4. Workflow — code ki nazar se

### Simple Factory

```
main → BurgerFactory.create("standard") → StandardBurger → prepare()
```

### Factory Method

```
main → new SinghBurger() → createBurger("basic") → BasicBurger → prepare()
```

### Abstract Factory

```
main → new KingBurger() → createBurger("basic") + createGarlicBread("cheese")
     → wheat burger + wheat bread → prepare() both
```

---

## 5. Kab kaunsa pattern?

| Situation | Pattern |
|-----------|---------|
| 2–5 fixed types, internal tool | **Simple Factory** |
| Multiple brands / plugins | **Factory Method** |
| UI theme, OS widget set, meal combo | **Abstract Factory** |
| Sirf 1 class kabhi badlegi nahi | Direct `new` / DI |
| Complex step-by-step build | **Builder**, not Factory |

---

## 6. Factory vs other creational patterns

| Pattern | Purpose |
|---------|---------|
| **Factory** | *Which class* to instantiate |
| **Builder** | *How to assemble* complex object step-by-step |
| **Prototype** | *Clone* existing instance |
| **Singleton** | *One instance* only |

---

## 7. Interview question bank

### Q1: Factory pattern kya solve karta hai?

**A:** Client ko concrete classes se decouple — creation logic factory mein; client interface use kare.

### Q2: Simple Factory GoF me hai?

**A:** Nahi — common idiom. Phir bhi OOP interviews me expect.

### Q3: Factory Method vs Abstract Factory?

**A:** FM = **one product**, subclass decides. AF = **family** of related products, one factory interface with multiple `create*`.

### Q4: Abstract Factory vs Factory Method — sabse bada difference?

**A:** *"FM inheritance se ek product; AF composition se **multiple coordinated products**."*

### Q5: OCP kaun break karta hai?

**A:** Simple Factory — central if-else grow hota hai.

### Q6: Real example?

**A:** UI: `WinFactory` → Windows button + menu; `MacFactory` → Mac button + menu.

### Q7: Factory vs Strategy?

**A:** Factory = **creation**; Strategy = **runtime behavior/algorithm** after object exists.

### Q8: DI container Factory replace karta hai?

**A:** Haan — Spring `ApplicationContext.getBean()` — manual factory kam chahiye.

### Q9: Pointer / memory interview twist?

**A:** Production `unique_ptr`, virtual destructor on factory interface — demo me raw `new` for teaching.

### Q10: Is repo burger example explain karo 30 sec me

**A:** Singh normal bun, King wheat bun; Abstract Factory me burger + garlic bread dono same family se.

---

## 8. Whiteboard checklist (45 min LLD + pattern)

1. Problem — client `new` everywhere  
2. Draw `Product` interface  
3. Simple Factory box — when enough  
4. If brands → Factory Method hierarchy  
5. If combo/theme → Abstract Factory `createA` + `createB`  
6. SOLID — OCP + DIP one line each  
7. Trade-off — more classes vs flexibility  

---

## 9. Repo cross-links

| Lesson / Project | Factory usage |
|------------------|---------------|
| **L9** | Burger shop — 3 variants |
| **L11** | Order factories |
| **L18** | `DeviceFactory` |
| **L23** | Payment gateway creation |
| **Ecommerce** | Payment rail factory |

---

## 10. Quick revision (exam eve)

```
Simple   = 1 class, if-else, OCP weak
Method   = virtual create, per brand, OCP strong
Abstract = many create*, families, themes
```

---

**Index:** [00_INDEX.md](./00_INDEX.md) · **Full guide:** [../README.md](../README.md)

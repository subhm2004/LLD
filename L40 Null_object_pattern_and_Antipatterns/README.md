# Null Object Pattern & Antipatterns — Detailed Guide

> **Null Object** ek **behavioral idiom** hai jo `nullptr` checks hata kar **no-op implementation** deta hai — same interface, kuch **nahi karta** (safe default).  
> **Antipatterns** woh **common bad designs** hain jo LLD interviews mein **avoid** karne ko kehte hain.

**Is folder mein:** [`Notes.pdf`](./Notes.pdf) — primary reference (no C++ demo code).  
**Repo code example:** WhatsApp [`NoOpEncryptionService`](../WhatsApp_LLD/services/EncryptionService.h).

---

## Table of Contents

1. [Null Object Pattern kya hai?](#1-null-object-pattern-kya-hai)
2. [Problem — Null checks everywhere](#2-problem--null-checks-everywhere)
3. [Solution — No-op object](#3-solution--no-op-object)
4. [Repo Example — WhatsApp Encryption](#4-repo-example--whatsapp-encryption)
5. [Null Object vs Singleton vs Optional](#5-null-object-vs-singleton-vs-optional)
6. [Kab use karein / Kab na karein](#6-kab-use-karein--kab-na-karein)
7. [Common Antipatterns (LLD)](#7-common-antipatterns-lld)
8. [Antipattern → Better Pattern Map](#8-antipattern--better-pattern-map)
9. [Interview Talking Points](#9-interview-talking-points)
10. [Further Reading & Summary](#10-further-reading--summary)

---

## 1. Null Object Pattern kya hai?

**Intent:** Missing dependency ko `nullptr` se represent mat karo — **Null Object** do jo **same interface** implement kare lekin **kuch na kare**.

```cpp
EncryptionService* crypto = useEncryption
    ? new AESEncryption()
    : new NoOpEncryptionService();  // not nullptr

crypto->encrypt(msg);  // always safe — no if (crypto != nullptr)
```

| Property | Detail |
| -------- | ------ |
| **Same interface** | Real service jaisa API |
| **Do nothing** | Methods empty or identity (return input) |
| **Polymorphic** | Client code branch-free |

> **"Null" is an object, not a pointer.** — avoids NullPointerException-style bugs.

---

## 2. Problem — Null checks everywhere

```cpp
// ❌ Defensive code at every call site
if (logger != nullptr) logger->log(msg);
if (encryption != nullptr) msg = encryption->encrypt(msg);
if (notifier != nullptr) notifier->send(event);
```

| Problem | Detail |
| ------- | ------ |
| **Scattered checks** | Easy to forget one path |
| **Optional feature = nullptr** | Special case proliferates |
| **Testing** | Must test null and non-null branches |
| **Readability** | Business logic drowned in guards |

---

## 3. Solution — No-op object

```cpp
class NoOpLogger : public ILogger {
public:
    void log(const string& msg) override { /* intentional no-op */ }
};

class NoOpEncryptionService : public EncryptionService {
public:
    string encrypt(const string& plainText) const override {
        return plainText;  // identity — "no encryption"
    }
};
```

**Client always calls** — behavior change via **which object injected**, not `if`.

---

## 4. Repo Example — WhatsApp Encryption

Source: [`WhatsApp_LLD/services/EncryptionService.h`](../WhatsApp_LLD/services/EncryptionService.h)

```cpp
class EncryptionService {
public:
    virtual string encrypt(const string& plainText) const = 0;
};

class NoOpEncryptionService : public EncryptionService {
public:
    string encrypt(const string& plainText) const override {
        return plainText;
    }
};
```

`ChatService` uses `NoOpEncryptionService` as default — encryption off ho tab bhi **same flow**, no null branch.

**Interview line:** "Production me Signal-style E2E; yahan Strategy + Null Object dikhane ke liye stub."

---

## 5. Null Object vs Singleton vs Optional

| Approach | Use when |
| -------- | -------- |
| **Null Object** | Optional **behavior** — same interface, no-op |
| **Singleton** | Exactly **one shared** instance (config, pool) |
| **`std::optional` / pointer** | Truly absent object; no behavior to invoke |
| **Strategy** | **Real** alternate algorithms — Null Object = one strategy |

**Null Object ≠ Singleton:** Null can have many instances (stateless no-op); Singleton = one global.

---

## 6. Kab use karein / Kab na karein

### ✅ Kab use karein

| Scenario | Example |
| -------- | ------- |
| Optional feature off by default | No encryption, no logging |
| Avoid null checks in hot paths | `logger->log()` always valid |
| Strategy set mein "none" option | `NoOp` as explicit choice |

### ❌ Kab na karein

| Scenario | Reason |
| -------- | ------ |
| **Absence ≠ do nothing** | Missing user ≠ `NullUser` with empty name — domain error |
| **Silent failures hide bugs** | Null logger OK; null **payment** dangerous |
| **Heavy null object** | Should stay lightweight |

---

## 7. Common Antipatterns (LLD)

From course notes + industry practice — **symptoms** aur **fixes**:

### 7.1 God Class / God Object

| Symptom | 2000-line `System` / `Manager` does everything |
| Fix | **SRP** — `OrderService`, `InventoryService`, **Facade** for simple API |

### 7.2 Spaghetti Code / Big Ball of Mud

| Symptom | No clear layers, everything calls everything |
| Fix | Layering: models → services → facade; **dependency direction** down |

### 7.3 Golden Hammer

| Symptom | "Har jagah Singleton" / "Har jagah Strategy" |
| Fix | Simple function/class jab variation nahi |

### 7.4 Copy-Paste Programming

| Symptom | Same switch/if in 5 classes |
| Fix | **Strategy**, **Template Method**, **Visitor** |

### 7.5 Singleton Abuse

| Symptom | Global state everywhere, untestable |
| Fix | **DI**, pass interfaces; Singleton sirf true single resource |

### 7.6 Anemic Domain Model

| Symptom | Models = getters/setters only; logic in services only |
| Fix | Rich domain methods where behavior belongs to entity |

### 7.7 Yo-Yo Problem (Deep Inheritance)

| Symptom | 8-level hierarchy, empty overrides |
| Fix | **Composition** — Bridge, Strategy, Decorator |

### 7.8 Shotgun Surgery

| Symptom | One change → 20 files edit |
| Fix | **Cohesive modules**, patterns to localize change (OCP) |

### 7.9 Lava Flow / Dead Code

| Symptom | Old code fear delete, unused classes remain |
| Fix | Refactor, tests, remove safely |

### 7.10 Premature Optimization

| Symptom | Flyweight/Singleton before need |
| Fix | Measure first; pattern jab problem real ho |

---

## 8. Antipattern → Better Pattern Map

| Smell | Better approach | Repo example |
| ----- | ----------------- | -------------- |
| Giant if on type | **Strategy** / **State** | L8, L32 |
| M×N subclasses | **Bridge** | L25, L34 |
| Complex constructor | **Builder** | L28 |
| Null checks optional service | **Null Object** | WhatsApp |
| Many-to-many chat wiring | **Mediator** | L35, L37 |
| Add op without editing elements | **Visitor** | L38 |
| Undo needed | **Memento** / **Command** | L39, L15 |
| One instance globally (justified) | **Singleton** (careful) | L10 |

---

## 9. Interview Talking Points

1. **Null Object:** "`nullptr` ki jagah no-op implementation — client code clean, polymorphic off switch."

2. **vs null pointer:** "Null Object is valid object; behavior defined (do nothing), not crash."

3. **WhatsApp:** "`NoOpEncryptionService` — encrypt returns plain text."

4. **God class:** "Split by responsibility; Facade for external API."

5. **When patterns hurt:** "Golden hammer — use pattern when variation/reuse real hai."

6. **L40 content:** "Detailed PDF notes in folder — antipatterns list for revision."

---

## 10. Further Reading & Summary

| Resource | Content |
| -------- | ------- |
| [`Notes.pdf`](./Notes.pdf) | Course notes — Null Object + antipatterns (primary) |
| [`docs/Design_Patterns.md`](../docs/Design_Patterns.md) | Section 7 — Null Object & antipatterns |
| [`WhatsApp_LLD`](../WhatsApp_LLD/) | `NoOpEncryptionService` code |

| Pehlu | Detail |
| ----- | ------ |
| **Null Object type** | Behavioral idiom (not always GoF catalog) |
| **This folder** | PDF notes — no C++ demo |
| **Code in repo** | WhatsApp `EncryptionService.h` |
| **Antipatterns** | God class, singleton abuse, anemic model, shotgun surgery, … |

> **Yaad rakho:** Null Object **blank employee badge** hai — system treat karta hai employee present hai, lekin koi kaam assign nahi karta; `nullptr` = badge hi nahi mila, har gate par check lagao. Antipatterns = **galat shortcuts** jo interview mein consciously avoid karo. 📋

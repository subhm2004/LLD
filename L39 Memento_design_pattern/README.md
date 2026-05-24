# Memento Design Pattern — Detailed Guide

> **Behavioral Pattern** jo object ka **state snapshot** save/restore karta hai **without exposing internals** — **Originator** creates memento, **Caretaker** stores it, **undo/rollback** possible.

**Domain example (is repo mein):** `Database` + `DatabaseMemento` + `TransactionManager` — commit vs rollback.

---

## Table of Contents

1. [Problem — No undo](#1-problem--no-undo)
2. [Memento Pattern kya hai?](#2-memento-pattern-kya-hai)
3. [Three Roles](#3-three-roles)
4. [Code Walkthrough](#4-code-walkthrough)
5. [Execution Flow](#5-execution-flow)
6. [Build & Run](#6-build--run)
7. [Chess extension & Summary](#7-chess-extension--summary)

---

## 1. Problem — No undo

```cpp
db.insert("user3", "Saurav");
// error — how restore to state before transaction?
```

Direct copy of `map` everywhere — **encapsulation break**; caretaker logic mixed with DB.

---

## 2. Memento Pattern kya hai?

```
Originator (Database) ──creates──► Memento (snapshot)
       ▲                                │
       │ restore                        │ stored by
       └──────────── Caretaker (TransactionManager)
```

| Role | Class | Job |
| ---- | ----- | --- |
| **Originator** | `Database` | `createMemento()`, `restoreFromMemento()` |
| **Memento** | `DatabaseMemento` | Opaque state storage |
| **Caretaker** | `TransactionManager` | `begin` / `commit` / `rollback` |

---

## 3. Three Roles

```cpp
DatabaseMemento* Database::createMemento() {
    return new DatabaseMemento(records);  // copy map
}

void Database::restoreFromMemento(const DatabaseMemento& m) {
    records = m.getState();
}
```

**Caretaker** memento ko modify nahi karta — sirf hold/delete.

---

## 4. Code Walkthrough

Source: [`C++ Code/MementoPattern.cpp`](./C%20%2B%2B%20Code/MementoPattern.cpp)

```cpp
void TransactionManager::beginTransaction(Database& db) {
    backup = db.createMemento();
}
void TransactionManager::commitTransaction() {
    delete backup; backup = nullptr;
}
void TransactionManager::rollbackTransaction(Database& db) {
    db.restoreFromMemento(*backup);
}
```

---

## 5. Execution Flow

| Phase | Action | DB state |
| ----- | ------ | -------- |
| 1 | begin → insert user1, user2 → **commit** | user1, user2 |
| 2 | begin → insert user3, user4 | 4 users |
| 3 | **rollback** (error) | back to user1, user2 only |

---

## 6. Build & Run

```bash
cd "L39 Memento_design_pattern/C++ Code"
g++ -std=c++17 -o memento_demo MementoPattern.cpp && ./memento_demo
```

---

## 7. Chess extension & Summary

**Interview extensions:** Chess move history (stack of mementos), text editor undo, game save slots.

**vs Command:** Command undo = reverse action; Memento = **state snapshot**.

| Pehlu | Detail |
| ----- | ------ |
| **Type** | Behavioral |
| **File** | `MementoPattern.cpp` |
| **Repo** | L37 Chess undo (extension idea) |

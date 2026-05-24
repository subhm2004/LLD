# Mediator Design Pattern — Detailed Guide

> **Behavioral Pattern** jo objects ke beech **direct references hata kar** communication **central mediator** se karwata hai — **N×(N−1)/2** connections → **N** connections to mediator.

**Domain example (is repo mein):** `ChatMediator` — broadcast, private message, mute; compare `WithoutMediator.cpp`.

---

## Table of Contents

1. [Problem — Mesh coupling](#1-problem--mesh-coupling)
2. [Mediator Pattern kya hai?](#2-mediator-pattern-kya-hai)
3. [With vs Without Mediator](#3-with-vs-without-mediator)
4. [Key Participants](#4-key-participants)
5. [Code Walkthrough](#5-code-walkthrough)
6. [Build & Run](#6-build--run)
7. [vs Observer & Summary](#7-vs-observer--summary)

---

## 1. Problem — Mesh coupling

`WithoutMediator.cpp` — har `User` ko **saare peers** manually add:

```cpp
user1->addPeer(user2); user2->addPeer(user1);
user1->addPeer(user3); user3->addPeer(user1);
// 3 users → 3 connections each; mute logic duplicated per User
```

| Problem | Detail |
| ------- | ------ |
| **N² wiring** | N users → O(N²) links |
| **Feature duplication** | Mute, private send har colleague mein |
| **Hard to extend** | Logging, ban list — sab classes edit |

---

## 2. Mediator Pattern kya hai?

```
User ──► ChatMediator ◄── User
              │
         broadcast / private / mute rules
```

**Colleagues** sirf mediator ko jaante hain — ek doosre ko nahi.

```cpp
void User::send(const string& msg) {
    mediator->send(name, msg);  // delegate
}
```

---

## 3. With vs Without Mediator

| Aspect | Without | With Mediator |
| ------ | ------- | ------------- |
| **Connections** | Peer list per user | Mediator registry |
| **Mute** | Per-user mute list | `ChatMediator::mute(who, whom)` |
| **Private msg** | `sendTo(User*)` | `sendPrivate(from, to, msg)` |
| **New feature** | Edit every User | Edit mediator only |

---

## 4. Key Participants

| Role | Class |
| ---- | ----- |
| **Mediator interface** | `IMediator` |
| **Concrete mediator** | `ChatMediator` |
| **Colleague** | `Colleague` / `User` |

**Mute logic:** Rohan mutes Mohan — Mohan's broadcasts skip Rohan (`from == muted && receiver == muter`).

---

## 5. Code Walkthrough

Source: [`C++ Code/MediatorPattern.cpp`](./C%20%2B%2B%20Code/MediatorPattern.cpp)

```cpp
class Colleague {
protected:
    IMediator* mediator;
public:
    Colleague(IMediator* m) {
        mediator = m;
        mediator->registerColleague(this);
    }
};
```

**Broadcast:**

```cpp
void ChatMediator::send(const string& from, const string& msg) {
    for (Colleague* c : colleagues)
        if (c->getName() != from && !isMuted(from, c))
            c->receive(from, msg);
}
```

---

## 6. Build & Run

```bash
cd "L35 Mediator_design_pattern/C++ Code"
g++ -std=c++17 -o with_med MediatorPattern.cpp && ./with_med
g++ -std=c++17 -o without_med WithoutMediator.cpp && ./without_med
```

**With mediator output:**

```
[Rohan broadcasts]: Hello Everyone!
    Neha got from Rohan: Hello Everyone!
    Mohan got from Rohan: Hello Everyone!
[Mohan→Neha]: Hey Neha!
    Neha got from Mohan: Hey Neha!
```

---

## 7. vs Observer & Summary

| Mediator | Observer |
| -------- | -------- |
| **Many-to-many** via hub | **One-to-many** notify |
| Colleagues don't know each other | Observers know subject |

**Repo:** L37 Chess `ChatMediator`.

| Pehlu | Detail |
| ----- | ------ |
| **Type** | Behavioral |
| **Files** | `MediatorPattern.cpp`, `WithoutMediator.cpp` |

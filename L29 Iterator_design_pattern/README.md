# Iterator Design Pattern — Detailed Guide

> **Behavioral Pattern** jo collection ke **internal structure hide** karke **uniform traversal** deta hai — `hasNext()` / `next()` se linked list, binary tree, playlist same client loop se iterate.

**Domain example (is repo mein):** `Iterator<T>` + `Iterable<T>` — `LinkedList`, `BinaryTree` (in-order), `Playlist`.

---

## Table of Contents

1. [Problem kya hai?](#1-problem-kya-hai)
2. [Iterator Pattern kya hai?](#2-iterator-pattern-kya-hai)
3. [Key Participants](#3-key-participants)
4. [Three Iterators in Code](#4-three-iterators-in-code)
5. [Code Walkthrough](#5-code-walkthrough)
6. [Build & Run](#6-build--run)
7. [vs Related & Summary](#7-vs-related--summary)

---

## 1. Problem kya hai?

Har collection alag traverse API:

```cpp
// ❌ Client har structure jaane
for (Node* n = head; n; n = n->next) ...
inorderRecursive(root);
for (int i = 0; i < songs.size(); i++) ...
```

---

## 2. Iterator Pattern kya hai?

```cpp
Iterator<int>* it = list->getIterator();
while (it->hasNext())
    cout << it->next() << " ";
```

**Polymorphic traversal** — client sirf iterator interface.

---

## 3. Key Participants

| Role | Interface / Class |
| ---- | ----------------- |
| **Iterator** | `Iterator<T>` — `hasNext()`, `next()` |
| **Iterable** | `Iterable<T>` — `getIterator()` |
| **Concrete Iterator** | `LinkedListIterator`, `BinaryTreeInorderIterator`, `PlaylistIterator` |
| **Concrete Aggregate** | `LinkedList`, `BinaryTree`, `Playlist` |

---

## 4. Three Iterators in Code

| Aggregate | Iterator | Algorithm |
| --------- | -------- | --------- |
| **LinkedList** | `LinkedListIterator` | Pointer walk `1→2→3` |
| **BinaryTree** | `BinaryTreeInorderIterator` | Stack — left spine + right |
| **Playlist** | `PlaylistIterator` | Vector index |

**Binary tree in-order:** stack mein left path push → pop → visit → push right's left chain.

---

## 5. Code Walkthrough

Source: [`C++ Code/IteratorPattern.cpp`](./C%20%2B%2B%20Code/IteratorPattern.cpp)

```cpp
template<typename T>
class Iterable {
public:
    virtual Iterator<T>* getIterator() = 0;
};
```

**Client loop (same for all):**

```cpp
while (iterator->hasNext()) {
    // use iterator->next()
}
```

---

## 6. Build & Run

```bash
cd "L29 Iterator_design_pattern/C++ Code"
g++ -std=c++17 -o iter_demo IteratorPattern.cpp && ./iter_demo
```

**Expected:**

```
LinkedList contents: 1 2 3 
BinaryTree inorder: 1 2 3 
Playlist songs:
  Admirin You by Karan Aujla
  Husn by Anuv Jain
```

---

## 7. vs Related & Summary

| Pattern | Farq |
| ------- | ---- |
| **Composite** | Tree structure | Iterator = traverse it |
| **C++ STL** | `begin()`/`end()` | Same idea, standard library |

**Interview:** "Iterator encapsulates traversal — multiple algorithms per aggregate without exposing internals."

| Pehlu | Detail |
| ----- | ------ |
| **Type** | Behavioral |
| **File** | `IteratorPattern.cpp` |

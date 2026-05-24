# State Design Pattern — Detailed Guide

> **Behavioral Pattern** jo object ke **internal state** ke hisaab se **behavior change** karta hai — giant `if-else` / `switch` ki jagah **state objects** jo transitions handle karte hain. Context (`VendingMachine`) current state ko delegate karta hai.

**Domain example (is repo mein):** Vending machine — `NoCoin` → `HasCoin` → `Dispense` → `SoldOut`.

**Note:** Blinkit L26 order **enum state** use karta hai — lighter form; L32 **full State pattern** with state classes.

---

## Table of Contents

1. [Problem — Giant switch](#1-problem--giant-switch)
2. [State Pattern kya hai?](#2-state-pattern-kya-hai)
3. [State Diagram (This Demo)](#3-state-diagram-this-demo)
4. [Key Participants](#4-key-participants)
5. [Code Walkthrough](#5-code-walkthrough)
6. [Execution Flow (main scenarios)](#6-execution-flow-main-scenarios)
7. [State vs Strategy](#7-state-vs-strategy)
8. [Build & Run](#8-build--run)
9. [Summary](#9-summary)

---

## 1. Problem — Giant switch

```cpp
// ❌ All transitions in one class
void insertCoin(int c) {
    if (state == NO_COIN) ...
    else if (state == HAS_COIN) ...
    else if (state == SOLD_OUT) ...
}
```

Har naya state → sab methods edit. **Illegal transitions** compile-time catch nahi.

---

## 2. State Pattern kya hai?

```cpp
void VendingMachine::insertCoin(int coin) {
    currentState = currentState->insertCoin(this, coin);  // new state returned
}
```

| Piece | Role |
| ----- | ---- |
| **Context** | `VendingMachine` — inventory, coins, `currentState` |
| **State interface** | `VendingState` — `insertCoin`, `selectItem`, `dispense`, `returnCoin`, `refill` |
| **Concrete states** | `NoCoinState`, `HasCoinState`, `DispenseState`, `SoldOutState` |

**State objects reusable** — ek hi `HasCoinState` instance, context data alag.

---

## 3. State Diagram (This Demo)

```
        insertCoin                    selectItem (enough $)
NO_COIN ─────────► HAS_COIN ─────────────────────► DISPENSE
   ▲                    │                              │
   │ returnCoin         │ insufficient                 │ dispense()
   │                    │ stay                         ▼
   └────────────────────┘                         NO_COIN (items>0)
                                                       │
                                                       │ items==0
                                                       ▼
                                                    SOLD_OUT
                                                       │
                                              refill() │
                                                       ▼
                                                    NO_COIN
```

---

## 4. Key Participants

| State | insertCoin | selectItem | dispense |
| ----- | ---------- | ---------- | -------- |
| **NoCoin** | → HasCoin | reject | reject |
| **HasCoin** | add coin | → Dispense if paid | reject |
| **Dispense** | return coin | wait | → NoCoin / SoldOut |
| **SoldOut** | return coin | reject | reject; refill → NoCoin |

---

## 5. Code Walkthrough

Source: [`C++ Code/StatePattern.cpp`](./C%20%2B%2B%20Code/StatePattern.cpp)

```cpp
class VendingState {
public:
    virtual VendingState* insertCoin(VendingMachine* machine, int coin) = 0;
    virtual VendingState* selectItem(VendingMachine* machine) = 0;
    virtual VendingState* dispense(VendingMachine* machine) = 0;
    // ...
};
```

**HasCoin → Dispense transition:**

```cpp
if (machine->getInsertedCoin() >= machine->getPrice()) {
    // change calculation...
    return machine->getDispenseState();
}
```

**Context stores state singletons** — created once in constructor.

---

## 6. Execution Flow (main scenarios)

| Step | Action | Result state |
| ---- | ------ | ------------ |
| 1 | `selectItem()` no coin | NO_COIN |
| 2 | `insertCoin(10)` | HAS_COIN (price 20) |
| 3 | `selectItem()` insufficient | HAS_COIN |
| 4 | `insertCoin(10)` + `selectItem()` | DISPENSE |
| 5 | `dispense()` | NO_COIN (1 item left) |
| 6 | Buy last item | SOLD_OUT |
| 7 | `refill(2)` | NO_COIN |

---

## 7. State vs Strategy

| | **State** | **Strategy** |
| - | --------- | ------------ |
| **Who changes behavior** | Object **transitions itself** | **Client** injects strategy |
| **Awareness** | States know context | Strategies usually don't know context |
| **Example** | Vending machine | Payment algorithm |

---

## 8. Build & Run

```bash
cd "L32 State_design_pattern/C++ Code"
g++ -std=c++17 -o state_demo StatePattern.cpp && ./state_demo
```

---

## 9. Summary

| Pehlu | Detail |
| ----- | ------ |
| **Type** | Behavioral |
| **Solves** | State-dependent behavior without mega conditionals |
| **Repo link** | Vending Machine LLD, Blinkit order states (enum) |
| **File** | `StatePattern.cpp` |

> **Yaad rakho:** State = vending machine ka **mode** — same buttons, alag mode par alag reaction.

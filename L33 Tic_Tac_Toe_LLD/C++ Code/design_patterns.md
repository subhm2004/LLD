# Tic-Tac-Toe App: Design Patterns Analysis

Is document me `/Users/shubham/Desktop/LLD/L33 Tic_Tac_Toe_LLD/C++ Code/TicTacToe.cpp` file me use hone wale sabhi design patterns ko detail me explain kiya gaya hai.

---

## Quick Summary (Overview of Patterns)

Tic-Tac-Toe LLD application me clean code architecture, validation swapping aur dynamic updates ke liye **3 design patterns** ka use kiya gaya hai:

| Pattern Name | Category | Purpose in Tic-Tac-Toe App |
| :--- | :--- | :--- |
| **1. Observer Pattern** | Behavioral | Game me hone wale events (jaise moves, start, draw, win) ko register kiye gaye observers (notification handler, console logger) tak dispatch karne ke liye. |
| **2. Strategy Pattern** | Behavioral | Game validation rules, winning conditions, aur draw check methods ko interchange/swap karne ke liye (jaise traditional rules vs custom rules). |
| **3. Factory Pattern** | Creational | `GameType` aur configuration ke based par exact type ka game instance instantiate karke return karne ke liye. |

---

## Detailed Analysis of Design Patterns

### 1. Observer Design Pattern
#### **Kyu use kiya gaya? (Intent)**
Game play cycle ke events (move record, winner announce, draw declare, etc.) ko decoupled notifications system se pass karne ke liye observer model use hota hai. Agar future me UI representation banana ho ya database logging karni ho, to game logic class ko bina chhede naya observer register kiya ja sakta hai.

#### **Implementation in Code:**
* **Observer Interface:** [IObserver](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L12-L17) interface update API provide karta hai.
* **Concrete Observer:** [ConsoleNotifier](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L20-L25) interface implement karta hai aur warnings/events ko console par update karta hai.
* **Observable Subject:** [TicTacToeGame](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L202-L215) dynamic collection range of observers rakhta hai aur custom events hone par alerts dispatch karta hai.

```cpp
// Observer Interface (Line 12-17)
class IObserver {
public:
  virtual void update(string msg) = 0;
  virtual ~IObserver() {}
};

// Concrete Observer (Line 20-25)
class ConsoleNotifier : public IObserver {
public:
  void update(string msg) override { 
    cout << "[Notification] " << msg << endl; 
  }
};
```

---

### 2. Strategy Design Pattern
#### **Kyu use kiya gaya? (Intent)**
Traditional rules structure ke bajaye alternative layouts ke rules dynamically customize ya swap ho sakein (jaise wildcard, time-out rules, extreme mode rules), isliye verification code rules-handling base strategy classes me split hai.

#### **Implementation in Code:**
* **Strategy Interface:** [TicTacToeRules](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L121-L128) dynamic strategies coordinate verification interface provide karti hai.
* **Concrete Strategy:** [StandardTicTacToeRules](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L131-L200) custom coordinate board validation, rows-columns line calculation matching, and remaining coordinate limits conditions design karti hai.

```cpp
// Strategy Interface (Line 121-128)
class TicTacToeRules {
public:
  virtual bool isValidMove(Board *board, int row, int col) = 0;
  virtual bool checkWinCondition(Board *board, Symbol *symbol) = 0;
  virtual bool checkDrawCondition(Board *board) = 0;
  virtual ~TicTacToeRules() {}
};
```

---

### 3. Factory Design Pattern (Simple Factory)
#### **Kyu use kiya gaya? (Intent)**
Client function direct game constructor dependency na rakhe aur configuration details encapsulate ho sakein, isliye TicTacToe application generation Simple Factory coordinate ke based par setup hoti hai.

#### **Implementation in Code:**
* [TicTacToeGameFactory](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L295-L305) static creation flow run karti hai:

```cpp
// Simple Factory (Line 295-305)
class TicTacToeGameFactory {
public:
  static TicTacToeGame *createGame(GameType gt, int boardSize) {
    if (GameType::STANDARD == gt) {
      return new TicTacToeGame(boardSize);
    }
    return nullptr;
  }
};
```

---
*Created by Antigravity*

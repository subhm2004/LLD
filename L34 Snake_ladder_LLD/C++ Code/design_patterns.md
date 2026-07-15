# Snake & Ladder App: Design Patterns Analysis

Is document me `/Users/shubham/Desktop/LLD/L34 Snake_ladder_LLD/C++ Code/Snake_and_Ladder.cpp` file me use hone wale sabhi design patterns ko detail me explain kiya gaya hai.

---

## Quick Summary (Overview of Patterns)

Snake & Ladder LLD application me code flexibility, reusable algorithm strategies aur modular design lane ke liye **3 main design patterns** use kiye gaye hain:

| Pattern Name | Category | Purpose in Snake & Ladder App |
| :--- | :--- | :--- |
| **1. Observer Pattern** | Behavioral | Game events (jaise rolling dice, snake/ladder bite, player index change, ya winning triggers) ko dynamically reporting notifier systems tak dispatch karne ke liye. |
| **2. Strategy Pattern** | Behavioral | Two places pe implementation strategy interchange karne ke liye: <br> 1) **Board Setup Logic** (Standard, Random, Custom setup configurations). <br> 2) **Game Validation Rules** (Standard parameters rules). |
| **3. Factory Pattern** | Creational | Configuration settings ke basis par appropriate Game instance construct karke return karne ke liye. |

---

## Detailed Analysis of Design Patterns

### 1. Observer Design Pattern
#### **Kyu use kiya gaya? (Intent)**
Game business logs ko display representations (Console window, network updates streams, DB history loggers) se clean decoupling dene ke liye Observer pattern register setup support use kiya jata hai. Game loop directly print statements hard-code karne ke bajaye generic interface events call karta hai.

#### **Implementation in Code:**
* **Observer Interface:** [IObserver](file:///Users/shubham/Desktop/LLD/L34%20Snake_ladder_LLD/C++%20Code/Snake_and_Ladder.cpp#L12-L17) notification receiving update contract define karta hai.
* **Concrete Observer:** [SnakeAndLadderConsoleNotifier](file:///Users/shubham/Desktop/LLD/L34%20Snake_ladder_LLD/C++%20Code/Snake_and_Ladder.cpp#L20-L25) console window layout warnings format setup ko handle karta hai.
* **Observable Subject:** [SnakeAndLadderGame](file:///Users/shubham/Desktop/LLD/L34%20Snake_ladder_LLD/C++%20Code/Snake_and_Ladder.cpp#L487) class observers array trace list store karti hai aur `notify()` dynamic loops trigger karti hai.

```cpp
// Observer Interface (Line 12-17)
class IObserver {
public:
  virtual void update(string msg) = 0;
  virtual ~IObserver() {}
};

// Concrete Observer (Line 20-25)
class SnakeAndLadderConsoleNotifier : public IObserver {
public:
  void update(string msg) override { 
    cout << "[NOTIFICATION] " << msg << endl; 
  }
};
```

---

### 2. Strategy Design Pattern
#### **Kyu use kiya gaya? (Intent)**
Do areas me algorithms easily interchangeable banaye gaye hain:
1. **Board Layout Strategy:** Traditional layout placement, dynamic random difficulties (Easy, Hard probabilities grids), aur custom inputs placement behaviors ko encapsulate kiya gaya hai.
2. **Move validation & translation rules:** Dice rolls values grid dimensions bounds control check, and destination index resolutions calculations settings algorithms ko isolated logic block me encapsulate kiya gaya hai.

#### **Implementation in Code:**
* **Example A: Board Setup Strategy**
  * Interface: [BoardSetupStrategy](file:///Users/shubham/Desktop/LLD/L34%20Snake_ladder_LLD/C++%20Code/Snake_and_Ladder.cpp#L225-L229)
  * Concrete Strategies: [StandardBoardSetupStrategy](file:///Users/shubham/Desktop/LLD/L34%20Snake_ladder_LLD/C++%20Code/Snake_and_Ladder.cpp#L328-L364), [RandomBoardSetupStrategy](file:///Users/shubham/Desktop/LLD/L34%20Snake_ladder_LLD/C++%20Code/RandomBoardSetupStrategy), aur [CustomCountBoardSetupStrategy](file:///Users/shubham/Desktop/LLD/L34%20Snake_ladder_LLD/C++%20Code/CustomCountBoardSetupStrategy).
* **Example B: Game Rules Strategy**
  * Interface: [SnakeAndLadderRules](file:///Users/shubham/Desktop/LLD/L34%20Snake_ladder_LLD/C++%20Code/Snake_and_Ladder.cpp#L442-L450)
  * Concrete Strategy: [StandardSnakeAndLadderRules](file:///Users/shubham/Desktop/LLD/L34%20Snake_ladder_LLD/C++%20Code/Snake_and_Ladder.cpp#L453-L472)

```cpp
// Strategy Interface: Board Setup (Line 225-229)
class BoardSetupStrategy {
public:
  virtual void setupBoard(Board *board) = 0;
  virtual ~BoardSetupStrategy() {}
};

// Context Executor callback link inside Board Class (Line 367-369)
void Board::setupBoard(BoardSetupStrategy *strategy) {
  strategy->setupBoard(this);
}
```

---

### 3. Factory Design Pattern (Simple Factory)
#### **Kyu use kiya gaya? (Intent)**
Standard game structures, randomized limits difficulty setups, and custom configuration game settings instantiations ko single helper class me control block wrap kiya gaya hai, taaki board settings setups complexity client ko directly handle na karni pade.

#### **Implementation in Code:**
* [SnakeAndLadderGameFactory](file:///Users/shubham/Desktop/LLD/L34%20Snake_ladder_LLD/C++%20Code/Snake_and_Ladder.cpp#L614-L649) class me concrete initialization flows manage hotey hain:

```cpp
// Creation Factory (Line 614-649)
class SnakeAndLadderGameFactory {
public:
  static SnakeAndLadderGame *createStandardGame() {
    Board *board = new Board(10);
    BoardSetupStrategy *strategy = new StandardBoardSetupStrategy();
    board->setupBoard(strategy);
    delete strategy;
    Dice *dice = new Dice(6);
    return new SnakeAndLadderGame(board, dice);
  }
  // ... createRandomGame & createCustomGame methods ...
};
```

---
*Created by Antigravity*

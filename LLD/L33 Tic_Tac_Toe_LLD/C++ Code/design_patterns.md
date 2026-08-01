# Tic-Tac-Toe App: Design Patterns Analysis

Is document me `/Users/shubham/Desktop/LLD/L33 Tic_Tac_Toe_LLD/C++ Code/TicTacToe.cpp` file me use hone wale sabhi design patterns, classes ki responsibilities, game flow aur important design decisions ko detail me explain kiya gaya hai.

---

## Quick Summary (Overview of Patterns)

Yeh application ek classic Low-Level Design (LLD) problem (Tic-Tac-Toe game) ko solve karti hai. Isme system ko scalable, maintainable aur decoupled banane ke liye **3 major Design Patterns** ka use kiya gaya hai:

| Pattern Name | Category | Purpose in Tic-Tac-Toe App |
| :--- | :--- | :--- |
| **1. Observer Pattern** | Behavioral | Game ke events (start, move, win, draw) ko sabhi registered listeners (jaise `ConsoleNotifier`) tak automatically broadcast karne ke liye. |
| **2. Strategy Pattern** | Behavioral | Valid-move, win aur draw check karne ke **rules** ko game logic se alag rakhne ke liye — taaki naye rule-sets (Anti Tic-Tac-Toe, Wildcard mode) bina game class chhede plug ho sakein. |
| **3. Factory Pattern** | Creational | `GameType` enum ke basis par sahi game instance create karke return karne ke liye — client ko `new TicTacToeGame(...)` direct nahi likhna padta. |

---

## Detailed Analysis of Design Patterns

### 1. Observer Design Pattern

#### **Kyu use kiya gaya? (Intent)**
Game me jab bhi kuch important hota hai — game start hua, kisi ne move chala, koi jeeta, ya draw hua — to ye khabar alag-alag jagah pahunchani pad sakti hai (console, UI, log file, database...). Agar game loop ke andar hi `cout` likh dete, to har naye notification channel ke liye `TicTacToeGame` class kholni padti.

Observer pattern se game (Subject) bas `notify(msg)` bolta hai, aur registered observers ki poori list ka `update()` apne aap call ho jata hai. **Game logic aur reporting logic completely decoupled** rehte hain (Open/Closed Principle).

#### **Implementation in Code:**
* **Observer Interface:** [IObserver](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L67-L77) — ek abstract class jisme pure virtual function `update(msg)` hai. Ye har observer ke liye common contract set karti hai.
* **Concrete Observer:** [ConsoleNotifier](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L82-L89) — `update()` ko override karke message console par `[Notification]` prefix ke saath print karta hai.
* **Subject (Observable):** [TicTacToeGame](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L433) — `observers` vector maintain karta hai; [addObserver()](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L465) se registration hoti hai aur [notify()](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L472) se broadcast.

```cpp
// Observer Interface (Line 67-77)
class IObserver {
public:
  virtual void update(string msg) = 0;
  virtual ~IObserver() {}
};

// Concrete Observer (Line 82-89)
class ConsoleNotifier : public IObserver {
public:
  void update(string msg) override {
    cout << "[Notification] " << msg << endl;
  }
};

// Subject ke andar broadcast logic (Line 472-478)
void notify(string msg) {
  for (auto observer : observers) {
    observer->update(msg);
  }
}
```

#### **Kaise extend karein?**
Ek nayi class banao jo `IObserver` implement kare (jaise `FileLogger`, `DatabaseLogger`, `UINotifier`) aur `game->addObserver(...)` se register kar do — `TicTacToeGame` ki ek line bhi nahi badalni padegi.

---

### 2. Strategy Design Pattern

#### **Kyu use kiya gaya? (Intent)**
"Move valid hai?", "Koi jeeta?", "Draw hua?" — ye teeno checks game ke **rules** hain, aur rules game-mode ke hisaab se badal sakte hain. Jaise:
- **Standard mode**: line banane wala jeet ta hai.
- **Anti Tic-Tac-Toe**: line banane wala *haar* jata hai.
- **Wildcard mode**: kuch special cells/moves allowed hote hain.

Strategy pattern in algorithms ko independent classes me encapsulate kar deta hai. `TicTacToeGame` sirf abstract interface (`TicTacToeRules`) ko jaanta hai — naya mode chahiye to bas nayi subclass inject karo.

#### **Implementation in Code:**
* **Strategy Interface:** [TicTacToeRules](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L309-L322) — teen pure virtual functions define karti hai: `isValidMove()`, `checkWinCondition()`, `checkDrawCondition()`.
* **Concrete Strategy:** [StandardTicTacToeRules](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L327-L420) — classic N x N rules implement karti hai:
  1. [isValidMove()](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L332): move valid hai agar target cell khali hai (bounds check `Board` ke andar hota hai).
  2. [checkWinCondition()](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L340): 4 tarah ki lines scan karta hai — har **row**, har **column**, **main diagonal** `[i][i]`, aur **anti-diagonal** `[i][size-1-i]`. Complexity: O(N²).
  3. [checkDrawCondition()](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L405): poore board me ek bhi khali cell na bache to draw (win-check pehle ho chuka hota hai).
* **Context:** [TicTacToeGame](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L433) apne paas `TicTacToeRules *rules` pointer rakhta hai aur saare checks isi ke through karwata hai.

```cpp
// Strategy Interface (Line 309-322)
class TicTacToeRules {
public:
  virtual bool isValidMove(Board *board, int row, int col) = 0;
  virtual bool checkWinCondition(Board *board, Symbol *symbol) = 0;
  virtual bool checkDrawCondition(Board *board) = 0;
  virtual ~TicTacToeRules() {}
};

// Game loop me Strategy ka use (Context) — game khud decide nahi karta:
if (rules->isValidMove(board, row, col)) {
  ...
  if (rules->checkWinCondition(board, currentPlayer->getSymbol())) { ... }
  else if (rules->checkDrawCondition(board)) { ... }
}
```

#### **Kaise extend karein?**
`class AntiTicTacToeRules : public TicTacToeRules` banao jisme `checkWinCondition` ulta kaam kare (line banne par *opponent* jeete). Phir `TicTacToeGame` ke constructor me standard rules ki jagah ye inject kar do — baaki system untouched rahega.

---

### 3. Factory Design Pattern (Simple Factory)

#### **Kyu use kiya gaya? (Intent)**
Client (`main()`) ko game object banane ki internal details (kaunsi rules class lagegi, board kaise banega) se door rakhne ke liye. Client sirf `GameType` enum bhejta hai aur factory ready-made game lauta deti hai. Isse **object creation logic centralize** ho jata hai.

#### **Implementation in Code:**
* **Type Tag:** [GameType](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L574) enum — abhi sirf `STANDARD` hai; future modes ke liye extension point.
* **Factory:** [TicTacToeGameFactory](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L582-L594) — static method [createGame()](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L586) type check karke sahi game instance return karta hai.

```cpp
// Simple Factory (Line 582-594)
class TicTacToeGameFactory {
public:
  static TicTacToeGame *createGame(GameType gt, int boardSize) {
    if (GameType::STANDARD == gt) {
      return new TicTacToeGame(boardSize);
    }
    return nullptr;
  }
};

// Client sirf itna likhta hai (Line 613-614):
TicTacToeGame *game =
    TicTacToeGameFactory::createGame(GameType::STANDARD, boardSize);
```

#### **Kaise extend karein?**
`GameType` me nayi value add karo (jaise `ANTI`) aur factory me ek branch — jisme game ko `AntiTicTacToeRules` ke saath configure karke return kiya jaye. Client code bilkul same rahega.

---

## Class Responsibilities (Ek Nazar Me)

| Class | Role | Responsibility |
| :--- | :--- | :--- |
| [IObserver](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L67) | Observer Interface | Har notification listener ka common contract (`update`). |
| [ConsoleNotifier](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L82) | Concrete Observer | Events ko console par print karna. |
| [Symbol](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L105) | Data Model | Ek mark ('X'/'O'/'-') ka wrapper — future me color/asset attach karne ke liye. |
| [Board](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L127) | Data Model ("Dumb Object") | Sirf N x N grid ki state: cell khali hai?, mark rakho, mark batao, board draw karo. **Rules ka koi logic nahi.** |
| [TicTacToePlayer](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L240) | Data Model | Player ki id, naam, symbol aur wins score. |
| [TicTacToeRules](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L309) | Strategy Interface | Valid-move / win / draw checks ka contract. |
| [StandardTicTacToeRules](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L327) | Concrete Strategy | Classic rules: row/column/diagonal line = win; board full = draw. |
| [TicTacToeGame](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L433) | Controller / Subject / Context | Game loop chalana, turns manage karna (round robin), rules se checks karwana, observers ko notify karna. |
| [TicTacToeGameFactory](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L582) | Simple Factory | `GameType` ke hisaab se game instance create karna. |

---

## Game Flow (`play()` ka Working)

[play()](file:///Users/shubham/Desktop/LLD/L33%20Tic_Tac_Toe_LLD/C++%20Code/TicTacToe.cpp#L487) method poore game ka main loop hai. Har iteration = ek player ki turn:

```
                    ┌──────────────────────────────┐
                    │  players < 2 ? => exit       │
                    └──────────────┬───────────────┘
                                   ▼
                       notify("Game Started!")
                                   ▼
              ┌────────────► board->display()
              │                    ▼
              │        currentPlayer = players.front()
              │                    ▼
              │          cin >> row >> col (input lo)
              │                    ▼
              │        rules->isValidMove(row, col) ?
              │            │NO                │YES
              │            ▼                  ▼
              │    "Invalid move!"    placeMark + notify(move)
              │    (wahi player            ▼
              │     dobara khelega)   WIN check? ──YES──► score++, notify, EXIT
              │            │               │NO
              │            │               ▼
              │            │          DRAW check? ──YES──► notify, EXIT
              │            │               │NO
              │            │               ▼
              │            │     pop_front + push_back
              │            │     (ROUND ROBIN — agla player front pe)
              └────────────┴───────────────┘
```

**Round Robin turns:** Players `deque` me rehte hain. Front wala player khelta hai; valid move ke baad use pop karke back me daal diya jata hai. `deque` isliye choose kiya kyunki dono ends par O(1) push/pop milta hai, aur design naturally **2 se zyada players** bhi support kar leta hai.

---

## Important Design Decisions (Dhyan Dene Wali Baatein)

### 1. Pointer Identity Comparison (sabse zaroori niyam)
`Board` har cell me `Symbol*` (pointer) store karta hai, value nahi:
- **Khali cell** = `grid[i][j] == emptyCell` — saare khali cells **ek hi shared `'-'` object** ko point karte hain.
- **Win check** = `getCell(i,j) == playerKaSymbol` — yahan char `'X'` compare **nahi** hota, pointer **address** compare hota hai.

Ye kaam isliye karta hai kyunki har player ke paas apna ek **unique** `Symbol` object hai jo poore game me same rehta hai. Fast hai, par dhyan rahe: do alag-alag `new Symbol('X')` objects kabhi equal nahi maane jayenge.

### 2. Ownership & Memory Management (kaun kya delete karta hai)
"Jo banata hai, wahi delete karta hai" — ye rule follow hota hai:

| Object | Kaun banata hai | Kaun delete karta hai |
| :--- | :--- | :--- |
| `Board`, `Rules` | `TicTacToeGame` constructor | `TicTacToeGame` destructor |
| `Symbol` (X/O) | `main()` (player ko pass) | `TicTacToePlayer` destructor |
| `Player`, `Notifier`, `Game` | `main()` | `main()` (end me `delete`) |

### 3. SOLID Principles Mapping
- **S (Single Responsibility):** `Board` sirf grid state, `Rules` sirf validation, `Game` sirf orchestration, `Notifier` sirf output.
- **O (Open/Closed):** Naye observers/rules/game-modes add karne ke liye existing classes modify nahi karni padti — sirf naye subclass/branch add hote hain.
- **L (Liskov Substitution):** Koi bhi `IObserver` ya `TicTacToeRules` subclass base pointer ki jagah use ho sakti hai.
- **D (Dependency Inversion):** `TicTacToeGame` concrete classes par nahi, abstractions (`IObserver`, `TicTacToeRules`) par depend karta hai.

### 4. Generic N x N Board
Board size hardcoded 3 nahi hai — user runtime par koi bhi size de sakta hai (4x4, 5x5...). `checkWinCondition` ke loops `board->getSize()` use karte hain, isliye rules automatically har size par kaam karte hain.

---

## Future Improvements (Interview Discussion Points)

1. **Naye Game Modes:** `AntiTicTacToeRules` (line banane wala haare) — sirf nayi Strategy subclass + Factory branch.
2. **Naye Observers:** `FileLogger` / `ReplayRecorder` — moves ki history record karke game replay feature.
3. **Input Robustness:** Abhi `cin >> row >> col` par non-numeric input aane par stream fail ho sakti hai — input validation layer add ki ja sakti hai.
4. **Smart Pointers:** Raw pointers (`new`/`delete`) ki jagah `unique_ptr`/`shared_ptr` use karke ownership ko compiler-enforced banaya ja sakta hai.
5. **Best of N Series:** `score` field already hai — multiple rounds khelwa kar series winner nikala ja sakta hai.

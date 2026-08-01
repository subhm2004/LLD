# Chess App: Design Patterns Analysis

Is document me `/Users/shubham/Desktop/LLD/L37 Chess_LLD/C++ Code/chess.cpp` file me use hone wale sabhi design patterns ko detail me explain kiya gaya hai.

---

## Quick Summary (Overview of Patterns)

Chess LLD application me code extensibility, dynamic matchmaking, communication flows, aur single subsystem tracking handle karne ke liye **4 major design patterns** ka use kiya gaya hai:

| Pattern Name | Category | Purpose in Chess App |
| :--- | :--- | :--- |
| **1. Strategy Pattern** | Behavioral | Teen (3) areas me algorithms switch karne ke liye: <br> 1) **Piece movement paths calculations** (`Piece`). <br> 2) **Board checkmate/stalemate rules evaluation** (`ChessRules`). <br> 3) **Queue player pairing matchmaking** (`MatchingStrategy`). |
| **2. Factory Pattern** | Creational | Color aur piece types parameters pass karke concrete piece objects ko allocate karne ke liye. |
| **3. Mediator Pattern** | Behavioral | Two players (Colleagues) ke bich dynamic match chats aur moves transactions flow coordinates encapsulate karne ke liye. |
| **4. Singleton Pattern** | Creational | Active matches aur waiting lists system coordinator (`GameManager`) ka ek hi global reference maintain karne ke liye. |

---

## Detailed Analysis of Design Patterns

### 1. Strategy Design Pattern
#### **Kyu use kiya gaya? (Intent)**
* **Piece Movement Scans:** Chess me har piece (Pawn, Rook, King, etc.) ke rules aur paths alag hote hain. Strategy pattern in individual rules ko separate classes me override kara ke execution modular banata hai.
* **Game validation rules:** Rules interfaces (checks matching checkmate/stalemate) ko isolated context (`ChessRules`) me rakha gaya hai taaki kal ko "Blitz rules" ya custom boards configurations change hone par basic game loops impact na ho.
* **Matchmaking algorithms:** Pairing logic dynamic target check (`MatchingStrategy`) provide karti hai. Hum standard score difference validation, age-based grouping, ya location-based filters swap kar sakte hain.

#### **Implementation in Code:**
* **Interface (Piece Movement):** [Piece](file:///Users/shubham/Desktop/LLD/L37%20Chess_LLD/C++%20Code/chess.cpp#L97-L130) class dynamic algorithm interface run karti hai.
* **Interface (Rules Validation):** [ChessRules](file:///Users/shubham/Desktop/LLD/L37%20Chess_LLD/C++%20Code/chess.cpp#L509-L519) coordinates checking system validation runs execute karti hai.
* **Interface (Matchmaking):** [MatchingStrategy](file:///Users/shubham/Desktop/LLD/L37%20Chess_LLD/C++%20Code/chess.cpp#L787-L792) pairing queue selection targets setup define karti hai.

```cpp
// Rules Strategy Interface Example (Line 509-519)
class ChessRules {
public:
  virtual ~ChessRules() {}
  virtual bool isValidMove(Move move, Board *board) = 0;
  virtual bool isInCheck(Color color, Board *board) = 0;
  virtual bool isCheckmate(Color color, Board *board) = 0;
  virtual bool isStalemate(Color color, Board *board) = 0;
};
```

---

### 2. Factory Design Pattern (Simple Factory)
#### **Kyu use kiya gaya? (Intent)**
Client or Board codes ko dynamic objects coordinate structure creation directly `new Knight(color)` run karne se decoupling dene ke liye piece creation factory utility setup run hoti hai.

#### **Implementation in Code:**
* [PieceFactory](file:///Users/shubham/Desktop/LLD/L37%20Chess_LLD/C++%20Code/chess.cpp#L175-L198) parameters match karke exact concrete instances assign karti hai.

```cpp
// Creational Factory (Line 175-198)
class PieceFactory {
public:
  static Piece *createPiece(PieceType type, Color color) {
    switch (type) {
      case KING:   return new King(color);
      case KNIGHT: return new Knight(color);
      // ... cases mapping pawn, rook, bishop, queen ...
    }
  }
};
```

---

### 3. Mediator Design Pattern
#### **Kyu use kiya gaya? (Intent)**
Users/Players directly aapas me messaging ya move sync check calculations pass nahi karte (one-to-one coupling hazards). Mediator (Match) colleague references coordinate links target system manage karta hai aur bidirectional communications ko handle karta hai.

#### **Implementation in Code:**
* **Mediator Hub:** [ChatMediator](file:///Users/shubham/Desktop/LLD/L37%20Chess_LLD/C++%20Code/chess.cpp#L614-L621) communication hubs details verify karta hai.
* **Colleagues:** [Colleague](file:///Users/shubham/Desktop/LLD/L37%20Chess_LLD/C++%20Code/chess.cpp#L624-L635) base class register link setup karti hai aur derived [User](file:///Users/shubham/Desktop/LLD/L37%20Chess_LLD/C++%20Code/chess.cpp#L638-L678) concrete colleague flows coordinate messages pass karta hai.
* **Concrete Mediator:** [Match](file:///Users/shubham/Desktop/LLD/L37%20Chess_LLD/C++%20Code/chess.cpp#L681) class chat forwarding systems and coordinates movements update logic coordinate implement karti hai.

```cpp
// Mediator interface mapping (Line 614-621)
class ChatMediator {
public:
  virtual void sendMessage(Message *message, User *user) = 0;
};

// Colleague implementation (Line 638-678)
class User : public Colleague {
public:
  void send(Message *message) override {
    if (mediator != nullptr) {
      mediator->sendMessage(message, this); // Colleague delegates to Mediator
    }
  }
};
```

---

### 4. Singleton Design Pattern
#### **Kyu use kiya gaya? (Intent)**
Matches states database pools trace synchronization, waiting players buffers queue controls maintain karne ke liye subsystem control GameManager coordinates single target references system maintain karta hai.

#### **Implementation in Code:**
* [GameManager](file:///Users/shubham/Desktop/LLD/L37%20Chess_LLD/C++%20Code/chess.cpp#L824) constructor private access parameters aur helper global point properties control setups hold karta hai.

```cpp
// GameManager Singleton Example (Line 824-850)
class GameManager {
private:
  static GameManager *instance;
  GameManager() {} // Private Constructor preventing direct usage.
public:
  static GameManager *getInstance() {
    if (instance == nullptr) {
      instance = new GameManager();
    }
    return instance;
  }
};
GameManager *GameManager::instance = nullptr;
```

---
*Created by Antigravity*

#include <deque>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

// ============================================================================
// 1. OBSERVER DESIGN PATTERN (Notification / Event Logging System)
// ============================================================================
// Is pattern ka main objective yeh hota hai ki jab bhi kisi object (Subject) ki 
// state me change ho (jaise: Game Start hona, kisi player ka move chalna, ya game 
// win/draw hona), toh isse jude sabhi listener/subscriber objects (Observers) 
// ko automatically update/notification mil jaye. Isse business logic aur reporting logic decoupled rehti hain.

// IObserver: Yeh ek base abstract interface class hai jo har observer ke liye contract set karti hai.
class IObserver {
public:
  // update(): Pure virtual function jisko har concrete observer implement karega.
  // msg: Yeh woh string message hai jo game loop ke events client tak pahuchane ke liye bhejega.
  virtual void update(string msg) = 0;
  
  // Virtual destructor taaki derived classes ki memory safely delete ho sake (Memory leaks se bachne ke liye).
  virtual ~IObserver() {}
};

// ConsoleNotifier: Yeh IObserver class ka concrete implementation hai.
// Iska kaam game ke notifications ko seedhe terminal console screen par print karna hai.
class ConsoleNotifier : public IObserver {
public:
  // update() method ko override kiya gaya hai notifications format karke print karne ke liye.
  void update(string msg) override { 
    cout << "[Notification] " << msg << endl; 
  }
};

// ============================================================================
// 2. SYMBOL & BOARD MANAGEMENT (Core Data Structures)
// ============================================================================

// Symbol class: Tic-Tac-Toe board ke cells par place kiye jane wale players ke 
// marks (jaise 'X' ya 'O') ko wrap karti hai. Iska benefit yeh hai ki future me 
// symbols me complex assets ya color-codes add kiye ja sakein bina primitive char par depend rahe.
class Symbol {
private:
  char mark; // Character value store karne ke liye variable (e.g. 'X', 'O', '-')

public:
  // Constructor: Ek symbol create karne ke liye char mark pass karna mandatory hai.
  Symbol(char m) { 
    mark = m; 
  }

  // getMark(): Symbol ki character value get karne ke liye getter function.
  char getMark() { 
    return mark; 
  }
};

// Board class: Tic-Tac-Toe game board grid ko maintain karti hai.
// Yeh ek "Dumb Object" hai jiska single responsibility sirf board state coordinates 
// check karna aur symbols set/get karna hai. Isme rules check karne ka logic nahi hota.
class Board {
private:
  vector<vector<Symbol *>> grid; // 2D Vector grid array matrix dynamically sized cells ko hold karne ke liye.
  int size;                      // Board dimension size (N) taaki N x N board ban sake (jaise 3 for 3x3).
  Symbol *emptyCell;             // Khali cell ko represent karne ke liye standard symbol object pointer (jaise '-').

public:
  // Constructor: Custom board size inputs lekar memory allocate karta hai.
  Board(int s) {
    size = s;
    emptyCell = new Symbol('-'); // Default empty indicator symbol banaya.
    // 2D grid matrix vector ko default emptyCell pointers se initialize kiya.
    grid = vector<vector<Symbol *>>(size, vector<Symbol *>(size, emptyCell));
  }

  // isCellEmpty: Check karta hai ki user ke entered coordinates (row, col) empty cell hain ya nahi.
  // Range index check karta hai taaki index-out-of-bounds error na aaye runtime par.
  bool isCellEmpty(int row, int col) {
    if (row < 0 || row >= size || col < 0 || col >= size) {
      return false; // Agar grid size boundary se bahar hai, toh validation fail.
    }
    // Agar waha default emptyCell symbol('-') set hai, to cell empty hai.
    return grid[row][col] == emptyCell;
  }

  // placeMark: Player ke custom mark symbol ko correct board cells coordinate pe place karta hai.
  // Returns true agar move check clear ho kar successfully set ho jata hai, else returns false.
  bool placeMark(int row, int col, Symbol *mark) {
    if (row < 0 || row >= size || col < 0 || col >= size) {
      return false; // Boundary check validation.
    }
    if (!isCellEmpty(row, col)) {
      return false; // Agar space khali nahi hai, to over-writing block kar do.
    }
    grid[row][col] = mark; // Dynamic coordinate cells index updating.
    return true;
  }

  // getCell: Kis index coordinates (row, col) par kaunsa player symbol pointer hai, use return karta hai.
  Symbol *getCell(int row, int col) {
    if (row < 0 || row >= size || col < 0 || col >= size) {
      return emptyCell; // Out of range hone par safe exit ke liye default return.
    }
    return grid[row][col];
  }

  // getSize(): Board dimension coordinates access helper function.
  int getSize() { 
    return size; 
  }

  // getEmptyCell(): Khali symbol pointer query helper function.
  Symbol *getEmptyCell() { 
    return emptyCell; 
  }

  // display: Board visual framework layout design. Isko console interface pe draw kiya jata hai.
  void display() {
    cout << "\n  ";
    // Col headers numbers display karne ke liye loop.
    for (int i = 0; i < size; i++) {
      cout << i << " ";
    }
    cout << endl;

    // Grid rendering logic: Har row ka index line start pe print karke inner values layout show karta hai.
    for (int i = 0; i < size; i++) {
      cout << i << " "; // Row headers number.
      for (int j = 0; j < size; j++) {
        cout << grid[i][j]->getMark() << " ";
      }
      cout << endl;
    }
    cout << endl;
  }
};

// ============================================================================
// 3. PLAYER MODEL
// ============================================================================

// TicTacToePlayer: Player user class jo player attributes (ID, name, symbol, dynamic wins score) wrap karti hai.
class TicTacToePlayer {
private:
  int playerId;    // Unique database/ID assignment code.
  string name;     // Player name display character.
  Symbol *symbol;  // Is player ke associated active symbol pointer ('X' ya 'O').
  int score;       // Match matches wins counter tracker.

public:
  // Constructor: Player model parameter fields parsing initialization.
  TicTacToePlayer(int playerId, string n, Symbol *s) {
    this->playerId = playerId;
    name = n;
    symbol = s;
    score = 0; // Starting score initialization starting point.
  }

  // getName(): Player profile user lookup function helper.
  string getName() { 
    return name; 
  }

  // getSymbol(): Player customized symbol character selector helper.
  Symbol *getSymbol() { 
    return symbol; 
  }

  // getScore(): Active total wins lookup helper.
  int getScore() { 
    return score; 
  }

  // incrementScore(): Win hone par points tracking system me increase register.
  void incrementScore() { 
    score++; 
  }

  // Destructor: Allocate kiye symbols dynamic variable cleanup memory trace deletion.
  ~TicTacToePlayer() { 
    delete symbol; 
  }
};

// ============================================================================
// 4. STRATEGY DESIGN PATTERN (Game Win/Draw Rules Swapping System)
// ============================================================================
// Strategy pattern isliye use hota hai taaki rule checking algorithm dynamic interfaces 
// me isolate ho sake. TicTacToeRules base interface coordinate system validation 
// define karta hai. Kal ko agar hume "Wildcard mode" ya "Anti-Tic-Tac-Toe" (jisme 3 line 
// banane par loss hota hai) introduce karna ho, to hum TicTacToeGame class ki logic ko 
// bina touch kiye bas ek nayi Rules subclass inject kar sakte hain.

// TicTacToeRules: Winning / Turn check validations standard logic. Base abstract strategy class.
class TicTacToeRules {
public:
  // isValidMove: Checks if custom move coordinates can legally fit inside current game layout dimensions.
  virtual bool isValidMove(Board *board, int row, int col) = 0;
  
  // checkWinCondition: Pure row-col grid values verification line matching algorithm checks.
  virtual bool checkWinCondition(Board *board, Symbol *symbol) = 0;
  
  // checkDrawCondition: Default limit cell counts analysis check to resolve draw states.
  virtual bool checkDrawCondition(Board *board) = 0;
  
  virtual ~TicTacToeRules() {}
};

// StandardTicTacToeRules: Traditional logic layout check checks for classic N x N grid match configurations.
class StandardTicTacToeRules : public TicTacToeRules {
public:
  // isValidMove: Dynamic index evaluation using board empty cell checkers.
  bool isValidMove(Board *board, int row, int col) override {
    return board->isCellEmpty(row, col);
  }

  // checkWinCondition: Custom check scanning lines row-by-row, column-by-column, and diagonals configurations.
  bool checkWinCondition(Board *board, Symbol *symbol) override {
    int size = board->getSize();

    // 1. Horizontal Rows validation: Har row check karo ki kya pure symbols matching player line ke hain.
    for (int i = 0; i < size; i++) {
      bool win = true;
      for (int j = 0; j < size; j++) {
        if (board->getCell(i, j) != symbol) {
          win = false; // Agar ek bhi mismatched block mila, row discard karo.
          break;
        }
      }
      if (win) return true; // Win trigger success!
    }

    // 2. Vertical Columns validation: Har column track validation logic check.
    for (int j = 0; j < size; j++) {
      bool win = true;
      for (int i = 0; i < size; i++) {
        if (board->getCell(i, j) != symbol) {
          win = false; // Mismatched cell column loop exit flag.
          break;
        }
      }
      if (win) return true;
    }

    // 3. Main Diagonal validation (Top-left block to Bottom-right block check [i,i]):
    bool win = true;
    for (int i = 0; i < size; i++) {
      if (board->getCell(i, i) != symbol) {
        win = false;
        break;
      }
    }
    if (win) return true;

    // 4. Anti-Diagonal validation (Top-right block to Bottom-left block check [i, size - 1 - i]):
    win = true;
    for (int i = 0; i < size; i++) {
      if (board->getCell(i, size - 1 - i) != symbol) {
        win = false;
        break;
      }
    }
    return win;
  }

  // checkDrawCondition: Grid analysis iteration loop setup.
  // Agar koi empty symbol ('-') nahi bachta aur win validation zero hai, to match is DRAW.
  bool checkDrawCondition(Board *board) override {
    int size = board->getSize();
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (board->getCell(i, j) == board->getEmptyCell()) {
          return false; // Abhi bhi space remaining hai, not draw yet.
        }
      }
    }
    return true; // Match drawn validation state success.
  }
};

// ============================================================================
// 5. GAME CONTROLLER & SUBJECT (Observable Object coordinate framework)
// ============================================================================

// TicTacToeGame: System Controller class. Pure matches trigger loops ko handle karti hai.
// Yeh dynamic list maintain karti hai isliye IObserver targets notifications setup me register.
class TicTacToeGame {
private:
  Board *board;                      // Grid board pointer storage.
  deque<TicTacToePlayer *> players;  // Queue interface round robin scheduling dynamic sequences players lists ke turn update control ke liye.
  TicTacToeRules *rules;             // Active logic system reference selection wrapper (Strategy Pattern Context).
  vector<IObserver *> observers;     // Notification registry container (Observer pattern collection list).
  bool gameOver;                     // State flag indicator of active validation limits loop checks.

public:
  // Constructor: Initializing standard default rules logic and Board initialization.
  TicTacToeGame(int boardSize) {
    board = new Board(boardSize);
    rules = new StandardTicTacToeRules(); // Classical strategy instance setup.
    gameOver = false;
  }

  // addPlayer: Player pointer registration queue push setup.
  void addPlayer(TicTacToePlayer *player) { 
    players.push_back(player); 
  }

  // addObserver: Notification targets callback registration loop storage setup.
  void addObserver(IObserver *observer) { 
    observers.push_back(observer); 
  }

  // notify: Broadcaster helper function. Collection observers lookup trigger checks.
  void notify(string msg) {
    for (auto observer : observers) {
      observer->update(msg);
    }
  }

  // play: Core game execution sequence logic control loops.
  void play() {
    // 2-player validation check. Game cannot execute single player operations.
    if (players.size() < 2) {
      cout << "Need at least 2 players!" << endl;
      return;
    }

    notify("Tic Tac Toe Game Started!"); // Event notification broadcast.

    // Game loop structure: gameOver flag check limit validation.
    while (!gameOver) {
      board->display(); // Har turn par user screen update printing updates.

      // Round robin sequence pop-push logic controller setup:
      // Front elements query dynamic turn assignments.
      TicTacToePlayer *currentPlayer = players.front();
      cout << currentPlayer->getName() << " ("
           << currentPlayer->getSymbol()->getMark()
           << ") - Enter row and column: ";

      int row, col;
      cin >> row >> col; // Keyboard input checks trigger.

      // Validation step: Check if input indices coordinates are legally vacant.
      if (rules->isValidMove(board, row, col)) {
        board->placeMark(row, col, currentPlayer->getSymbol());
        notify(currentPlayer->getName() + " played (" + to_string(row) + "," +
               to_string(col) + ")");

        // Win state checks checking sequence:
        if (rules->checkWinCondition(board, currentPlayer->getSymbol())) {
          board->display();
          cout << currentPlayer->getName() << " wins!" << endl;
          currentPlayer->incrementScore();
          notify(currentPlayer->getName() + " wins!");
          gameOver = true; // Turn exit loop trigger.
        } 
        // Draw state checking sequence:
        else if (rules->checkDrawCondition(board)) {
          board->display();
          cout << "It's a draw!" << endl;
          notify("Game is Draw!");
          gameOver = true; // Turn exit draw status sequence successful.
        } 
        // Agar dynamic checks negative hain to next active player turn push back line.
        else {
          players.pop_front();          // Active player turn clear.
          players.push_back(currentPlayer); // Queue bottom standard positioning sequence round robin trigger.
        }
      } else {
        cout << "Invalid move! Try again." << endl; // Invalid indices coordinate warnings message print.
      }
    }
  }

  // Destructor: System components delete memory cleaners functions.
  ~TicTacToeGame() {
    delete board;
    delete rules;
  }
};

// ============================================================================
// 6. FACTORY DESIGN PATTERN (Game Instance Creation Service)
// ============================================================================

enum GameType { STANDARD }; // Future expandability ke liye game mode type settings indicators enum list.

// TicTacToeGameFactory: Game instances allocator provider. Object construction logic parameters details 
// hide framework setup provide karta hai (Client ko direct constructor instantiation bypass).
class TicTacToeGameFactory {
public:
  // createGame: Factory creation static constructor logic dispatcher.
  static TicTacToeGame *createGame(GameType gt, int boardSize) {
    if (GameType::STANDARD == gt) {
      return new TicTacToeGame(boardSize);
    }
    return nullptr;
  }
};

// ============================================================================
// 7. CLIENT DRIVER ENTRY POINT
// ============================================================================
int main() {
  cout << "=== TIC TAC TOE GAME ===" << endl;

  // Board dynamic dimension values dynamic query input from user screen
  int boardSize;
  cout << "Enter board size (e.g., 3 for 3x3): ";
  cin >> boardSize;

  // Game initialization dynamic factory request lookup.
  TicTacToeGame *game =
      TicTacToeGameFactory::createGame(GameType::STANDARD, boardSize);

  // Observer console callback initialization and registration mapping:
  IObserver *notifier = new ConsoleNotifier();
  game->addObserver(notifier);

  // Players generation allocation and dynamic character values assignments:
  TicTacToePlayer *player1 = new TicTacToePlayer(1, "Aditya", new Symbol('X'));
  TicTacToePlayer *player2 = new TicTacToePlayer(2, "Harshita", new Symbol('O'));

  // Players loading array triggers:
  game->addPlayer(player1);
  game->addPlayer(player2);

  // Game loop runtime thread triggering:
  game->play();

  // Active resource cleanup dynamic deletions to avoid memory leaks:
  delete game;
  delete player1;
  delete player2;
  delete notifier;

  return 0;
}
#include <cstdlib>
#include <ctime>
#include <deque>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

// ============================================================================
// 1. OBSERVER DESIGN PATTERN (Notification / Event Logging System)
// ============================================================================
// Is pattern ka use hum tab karte hain jab hume game ke events (jaise game start 
// hona, player ka dice roll karna, snake/ladder encounter karna, ya game over hona) 
// ko kisi reporting system (jaise console log, analytics, or UI notifier) ko 
// dynamically notify karna ho bina core game logic code ko hard-couple kiye.

// IObserver: Sabhi Concrete Observers ke liye base abstract interface class.
class IObserver {
public:
  // update(): Yeh function tab call hota hai jab bhi subject me koi state change ho.
  // msg: Broadcaster dwara bheja gaya notification event message.
  virtual void update(string msg) = 0;
  
  // Virtual destructor taaki memory leakage ke bina child class clean ho sake.
  virtual ~IObserver() {}
};

// SnakeAndLadderConsoleNotifier: ConsoleNotifier jo IObserver ko implement karta hai.
// Iska single work notifications ko console stream standard output screen par print karna hai.
class SnakeAndLadderConsoleNotifier : public IObserver {
public:
  void update(string msg) override { 
    cout << "[NOTIFICATION] " << msg << endl; 
  }
};

// ============================================================================
// 2. DICE MODEL
// ============================================================================

// Dice: Game me numbers generate karne ke liye standard Dice representation class.
class Dice {
private:
  int faces; // Dice ke total number of sides/faces (jaise traditional dice me 6 faces hote hain)

public:
  // Constructor: Dice initialize karta hai faces quantity ke saath aur random seed initialize karta hai.
  Dice(int f) {
    faces = f;
    srand(time(0)); // Random number generator engine ko initialize kiya taaki har run pe different values milein.
  }

  // roll(): Dice ko roll karta hai aur 1 se lekar faces value range ke bich ka random integer return karta hai.
  int roll() { 
    return (rand() % faces) + 1; 
  }
};

// ============================================================================
// 3. BOARD ENTITIES (Snakes & Ladders models using Inheritance)
// ============================================================================

// BoardEntity: Snake aur Ladder dono ke start aur end positions hote hain. 
// Is common behavior ko extract karke yeh abstract base class design ki gayi hai.
class BoardEntity {
protected:
  int startPosition; // Entity ka starting index cell number (jahan player land karega)
  int endPosition;   // Destination index cell number (jahan landing ke baad player pahunchega)

public:
  // Constructor: Start aur End cell configuration settings map karta hai.
  BoardEntity(int start, int end) {
    startPosition = start;
    endPosition = end;
  }

  // getStart(): Starting coordinate range trace check.
  int getStart() { 
    return startPosition; 
  }

  // getEnd(): Ending coordinate target destination check.
  int getEnd() { 
    return endPosition; 
  }

  // Pure Virtual methods derived classes custom representation aur debug logs printing ke liye.
  virtual void display() = 0;
  virtual string name() = 0;
  virtual ~BoardEntity() {}
};

// Snake class: BoardEntity se inherit karti hai.
// Snake ka startPosition humesha endPosition se bada hona chahiye (kyunki snake bite ke baad player neeche jata hai).
class Snake : public BoardEntity {
public:
  Snake(int start, int end) : BoardEntity(start, end) {
    if (end >= start) {
      // Logic boundary warning checks.
      cout << "Invalid snake! End must be less than start." << endl;
    }
  }

  // display(): Console screen layout details.
  void display() override {
    cout << "Snake: " << startPosition << " -> " << endPosition << endl;
  }

  // name(): Game string tags identification.
  string name() override { 
    return "SNAKE"; 
  }
};

// Ladder class: BoardEntity se inherit karti hai.
// Ladder me endPosition humesha startPosition se bada hona chahiye (kyunki ladder se player upar jata hai).
class Ladder : public BoardEntity {
public:
  Ladder(int start, int end) : BoardEntity(start, end) {
    if (end <= start) {
      // Logic boundary checks coordinate validations.
      cout << "Invalid ladder! End must be greater than start." << endl;
    }
  }

  // display(): Visual coordinate checks representation.
  void display() override {
    cout << "Ladder: " << startPosition << " -> " << endPosition << endl;
  }

  // name(): Identity verification tag.
  string name() override { 
    return "LADDER"; 
  }
};

// ============================================================================
// 4. STRATEGY DESIGN PATTERN (Board Layout Initialization Setup Strategies)
// ============================================================================
// Board ko setup karne ke alag-alag tarike ho sakte hain: Standard traditional board positions, 
// random board configuration with difficulties (Easy, Hard), ya user configured positions.
// Strategy Pattern board setup logic ko switch karne me help karta hai.

class BoardSetupStrategy;

// Board class: Board layout configuration and matrix properties storage.
class Board {
private:
  int size;                                     // Total board dimension capacity (M * M cells grid, jaise 10x10=100)
  vector<BoardEntity *> snakesAndLadders;       // Board entities ki dynamic array list storage.
  map<int, BoardEntity *> boardEntities;        // Mapped cell indices to check entities faster (O(1) search).

public:
  Board(int s) {
    size = s * s; // dynamic sizing (M*M) setup.
  }

  // canAddEntity: Check karta hai ki targeted position grid cell par pehle se koi snake ya ladder toh nahi hai.
  bool canAddEntity(int position) {
    return boardEntities.find(position) == boardEntities.end();
  }

  // addBoardEntity: Valid entities ko lists and map datasets me safely store karta hai.
  void addBoardEntity(BoardEntity *boardEntity) {
    if (canAddEntity(boardEntity->getStart())) {
      snakesAndLadders.push_back(boardEntity);
      boardEntities[boardEntity->getStart()] = boardEntity;
    }
  }

  // setupBoard: Dynamic strategy parameter run configuration logic loader (Strategy Injection).
  void setupBoard(BoardSetupStrategy *strategy);

  // getEntity: Check coordinates cell numbers pe search query trace.
  BoardEntity *getEntity(int position) {
    if (boardEntities.find(position) != boardEntities.end()) {
      return boardEntities[position];
    }
    return nullptr; // No entity present at this coordinate.
  }

  int getBoardSize() { 
    return size; 
  }

  // display(): Active details printing loops layout config.
  void display() {
    cout << "\n=== Board Configuration ===" << endl;
    cout << "Board Size: " << size << " cells" << endl;

    int snakeCount = 0;
    int ladderCount = 0;
    for (auto entity : snakesAndLadders) {
      if (entity->name() == "SNAKE")
        snakeCount++;
      else
        ladderCount++;
    }

    cout << "\nSnakes: " << snakeCount << endl;
    for (auto entity : snakesAndLadders) {
      if (entity->name() == "SNAKE") {
        entity->display();
      }
    }

    cout << "\nLadders: " << ladderCount << endl;
    for (auto entity : snakesAndLadders) {
      if (entity->name() == "LADDER") {
        entity->display();
      }
    }
    cout << "=========================" << endl;
  }

  // Destructor: Vector containers clear arrays dynamics variables cleanups to prevent memory leaks.
  ~Board() {
    for (auto entity : snakesAndLadders) {
      delete entity;
    }
  }
};

// BoardSetupStrategy: Strategy pattern interface class for Board Setup.
class BoardSetupStrategy {
public:
  virtual void setupBoard(Board *board) = 0;
  virtual ~BoardSetupStrategy() {}
};

// RandomBoardSetupStrategy: BoardSetupStrategy ka implementation class.
// Yeh random coordinates generators ke base par complexity select karke setup run karta hai.
class RandomBoardSetupStrategy : public BoardSetupStrategy {
public:
  enum Difficulty {
    EASY,   // More ladders (easy to win), fewer snakes
    MEDIUM, // Equal density of snakes and ladders
    HARD    // More snakes (painful to win), fewer ladders
  };

private:
  Difficulty difficulty;

  // setupWithProbability: Probability matrix analysis setup function.
  // snakeProbability coordinate limit set karta hai. Easy me kam snakes and hard me zyada snakes.
  void setupWithProbability(Board *board, double snakeProbability) {
    int boardSize = board->getBoardSize();
    int totalEntities = boardSize / 10; // Board grid cell capacity check (roughly 10% blocks are entities).

    for (int i = 0; i < totalEntities; i++) {
      double prob = (double)rand() / RAND_MAX;

      if (prob < snakeProbability) {
        // Snake creation loop logic (Attempts dynamic bounds checking loops)
        int attempts = 0;
        while (attempts < 50) {
          int start = rand() % (boardSize - 10) + 10; // Minimum 10 index threshold requirement.
          int end = rand() % (start - 1) + 1;         // End position must be below starting point.

          if (board->canAddEntity(start)) {
            board->addBoardEntity(new Snake(start, end));
            break;
          }
          attempts++;
        }
      } else {
        // Ladder creation loop logic setup
        int attempts = 0;
        while (attempts < 50) {
          int start = rand() % (boardSize - 10) + 1;
          int end = rand() % (boardSize - start) + start + 1; // End position must be above starting point.

          if (board->canAddEntity(start) && end < boardSize) {
            board->addBoardEntity(new Ladder(start, end));
            break;
          }
          attempts++;
        }
      }
    }
  }

public:
  RandomBoardSetupStrategy(Difficulty d) { 
    difficulty = d; 
  }

  // setupBoard: Difficulty configuration map selector updates values.
  void setupBoard(Board *board) override {
    switch (difficulty) {
    case EASY:
      setupWithProbability(board, 0.3); // 30% Snakes, 70% Ladders
      break;
    case MEDIUM:
      setupWithProbability(board, 0.5); // 50% Snakes, 50% Ladders
      break;
    case HARD:
      setupWithProbability(board, 0.7); // 70% Snakes, 30% Ladders
      break;
    }
  }
};

// CustomCountBoardSetupStrategy: BoardSetupStrategy ka implementation.
// Is strategy me user board size ke sath manual entity configuration values pass kar sakta hai.
class CustomCountBoardSetupStrategy : public BoardSetupStrategy {
private:
  int numSnakes;                          // Custom quantity of snakes.
  int numLadders;                         // Custom quantity of ladders.
  bool randomPositions;                   // Positions coordinates manually input check flag or automatic random mapping.
  vector<pair<int, int>> snakePositions;  // Custom snake positions list coordinates array.
  vector<pair<int, int>> ladderPositions; // Custom ladder positions list coordinates array.

public:
  CustomCountBoardSetupStrategy(int snakes, int ladders, bool random) {
    numSnakes = snakes;
    numLadders = ladders;
    randomPositions = random;
  }

  void addSnakePosition(int start, int end) {
    snakePositions.push_back(make_pair(start, end));
  }

  void addLadderPosition(int start, int end) {
    ladderPositions.push_back(make_pair(start, end));
  }

  // setupBoard: Evaluates board positions based on user requirements.
  void setupBoard(Board *board) override {
    if (randomPositions) {
      // User targeted limits configurations with random coordinate mapping loops
      int boardSize = board->getBoardSize();

      // Add snakes loop
      int snakesAdded = 0;
      while (snakesAdded < numSnakes) {
        int start = rand() % (boardSize - 10) + 10;
        int end = rand() % (start - 1) + 1;

        if (board->canAddEntity(start)) {
          board->addBoardEntity(new Snake(start, end));
          snakesAdded++;
        }
      }

      // Add ladders loop
      int laddersAdded = 0;
      while (laddersAdded < numLadders) {
        int start = rand() % (boardSize - 10) + 1;
        int end = rand() % (boardSize - start) + start + 1;

        if (board->canAddEntity(start) && end < boardSize) {
          board->addBoardEntity(new Ladder(start, end));
          laddersAdded++;
        }
      }
    } else {
      // Manual coordinate placement checks
      for (auto &pos : snakePositions) {
        if (board->canAddEntity(pos.first)) {
          board->addBoardEntity(new Snake(pos.first, pos.second));
        }
      }

      for (auto &pos : ladderPositions) {
        if (board->canAddEntity(pos.first)) {
          board->addBoardEntity(new Ladder(pos.first, pos.second));
        }
      }
    }
  }
};

// StandardBoardSetupStrategy: BoardSetupStrategy implementation class.
// Yeh traditional configurations standard values set karta hai (classic 10x10 configuration values).
class StandardBoardSetupStrategy : public BoardSetupStrategy {
public:
  void setupBoard(Board *board) override {
    // Basic verification of sizes compatibility rules
    if (board->getBoardSize() != 100) {
      cout << "Standard setup only works for 10x10 board!" << endl;
      return;
    }

    // Classic snake placement configuration coordinates mapping (Traditional values)
    board->addBoardEntity(new Snake(99, 54));
    board->addBoardEntity(new Snake(95, 75));
    board->addBoardEntity(new Snake(92, 88));
    board->addBoardEntity(new Snake(89, 68));
    board->addBoardEntity(new Snake(74, 53));
    board->addBoardEntity(new Snake(64, 60));
    board->addBoardEntity(new Snake(62, 19));
    board->addBoardEntity(new Snake(49, 11));
    board->addBoardEntity(new Snake(46, 25));
    board->addBoardEntity(new Snake(16, 6));

    // Classic ladders positions coordinates mapping
    board->addBoardEntity(new Ladder(2, 38));
    board->addBoardEntity(new Ladder(7, 14));
    board->addBoardEntity(new Ladder(8, 31));
    board->addBoardEntity(new Ladder(15, 26));
    board->addBoardEntity(new Ladder(21, 42));
    board->addBoardEntity(new Ladder(28, 84));
    board->addBoardEntity(new Ladder(36, 44));
    board->addBoardEntity(new Ladder(51, 67));
    board->addBoardEntity(new Ladder(71, 91));
    board->addBoardEntity(new Ladder(78, 98));
    board->addBoardEntity(new Ladder(87, 94));
  }
};

// Defining setupBoard for Board class (Dynamic strategy executor callback link)
void Board::setupBoard(BoardSetupStrategy *strategy) {
  strategy->setupBoard(this);
}

// ============================================================================
// 5. PLAYER MODEL
// ============================================================================

// SnakeAndLadderPlayer: Player configuration properties storage model.
class SnakeAndLadderPlayer {
private:
  int playerId;   // Player standard index identification.
  string name;    // Player name.
  int position;   // Player cell index number on current board coordinate sheet.
  int score;      // Total tournament wins score.

public:
  SnakeAndLadderPlayer(int playerId, string n) {
    this->playerId = playerId;
    name = n;
    position = 0; // Sabhi players game start par index cell 0 (Board ke bahar) par hote hain.
    score = 0;
  }

  // Getters and Setters
  string getName() { 
    return name; 
  }
  
  int getPosition() { 
    return position; 
  }
  
  void setPosition(int pos) { 
    position = pos; 
  }
  
  int getScore() { 
    return score; 
  }
  
  void incrementScore() { 
    score++; 
  }
};

// ============================================================================
// 6. STRATEGY DESIGN PATTERN (Game Rule validation Strategies)
// ============================================================================
// Turn validation rules aur positions changes checking algorithms are isolated. 
// Kal ko rules change karne hon (jaise double rolls required on start coordinate), 
// to hum easily rules strategy modify or inject kar sakte hain.

// SnakeAndLadderRules: Game behavior and index change rules interface checker.
class SnakeAndLadderRules {
public:
  // isValidMove: Checks if current move coordinates doesn't overflow Board index limits.
  virtual bool isValidMove(int currentPos, int diceValue, int boardSize) = 0;
  
  // calculateNewPosition: Calculates next position by resolving snakes and ladders coordinates.
  virtual int calculateNewPosition(int currentPos, int diceValue, Board *board) = 0;
  
  // checkWinCondition: Check if player reaches target destination exactly.
  virtual bool checkWinCondition(int position, int boardSize) = 0;
  
  virtual ~SnakeAndLadderRules() {}
};

// StandardSnakeAndLadderRules: Classic traditional rules implementation class.
class StandardSnakeAndLadderRules : public SnakeAndLadderRules {
public:
  // isValidMove: Target overflow limit checks. 
  // (Agar player index 97 pe hai aur dice value 5 aati hai, to move invalid hai kyunki 97+5=102, jo limits se bahar hai).
  bool isValidMove(int currentPos, int diceValue, int boardSize) override {
    return (currentPos + diceValue) <= boardSize;
  }

  // calculateNewPosition: Cell intermediate translation validation rules checks.
  int calculateNewPosition(int currentPos, int diceValue, Board *board) override {
    int newPos = currentPos + diceValue;
    BoardEntity *entity = board->getEntity(newPos); // Checks if target cell has snake/ladder.

    if (entity != nullptr) {
      return entity->getEnd(); // Return updated destination.
    }
    return newPos; // If no entity, player stays on newPos.
  }

  // checkWinCondition: Check logic target match. Player must reach the exact board size cell.
  bool checkWinCondition(int position, int boardSize) override {
    return position == boardSize;
  }
};

// ============================================================================
// 7. GAME SYSTEM SYSTEM CONTROLLER (Subject / Observable class)
// ============================================================================

// SnakeAndLadderGame: Master controller class jo matches execution triggers loop runs manage karti hai.
class SnakeAndLadderGame {
private:
  Board *board;                             // Grid board memory settings parameters reference setup.
  Dice *dice;                               // Dice utility engine pointer object.
  deque<SnakeAndLadderPlayer *> players;    // Queue dynamic round robin player scheduling turn indicators.
  SnakeAndLadderRules *rules;               // Rules verification Strategy Context object.
  vector<IObserver *> observers;           // Observer array notifications targets container.
  bool gameOver;                            // Game active running iteration indicator flag.

public:
  // Constructor: Initialize Board, Dice, Default Classic rules validation objects.
  SnakeAndLadderGame(Board *b, Dice *d) {
    board = b;
    dice = d;
    rules = new StandardSnakeAndLadderRules(); // Default strategies injected interface setup.
    gameOver = false;
  }

  void addPlayer(SnakeAndLadderPlayer *player) { 
    players.push_back(player); 
  }

  void addObserver(IObserver *observer) { 
    observers.push_back(observer); 
  }

  // notify: Broadcast notification messages to all subscribed observers.
  void notify(string msg) {
    for (auto observer : observers) {
      observer->update(msg);
    }
  }

  // displayPlayerPositions: Display players positions during the game in console.
  void displayPlayerPositions() {
    cout << "\n=== Current Positions ===" << endl;
    for (auto player : players) {
      cout << player->getName() << ": " << player->getPosition() << endl;
    }
    cout << "======================" << endl;
  }

  // play: Main loop execution thread triggering turn inputs sequences.
  void play() {
    if (players.size() < 2) {
      cout << "Need at least 2 players!" << endl;
      return;
    }

    notify("Game started");
    board->display();

    // Loop continues until a player triggers win conditions settings
    while (!gameOver) {
      SnakeAndLadderPlayer *currentPlayer = players.front();

      cout << "\n" << currentPlayer->getName() << "'s turn. Press Enter to roll dice...";
      cin.ignore(); // Clears residual newline characters from cin.
      cin.get();    // Wait for User Enter key input confirmation.

      int diceValue = dice->roll();
      cout << "Rolled: " << diceValue << endl;

      int currentPos = currentPlayer->getPosition();

      // Rule strategy execution checks
      if (rules->isValidMove(currentPos, diceValue, board->getBoardSize())) {
        int intermediatePos = currentPos + diceValue;
        int newPos = rules->calculateNewPosition(currentPos, diceValue, board);

        currentPlayer->setPosition(newPos);

        // Verification if player encountered snakes/ladders elements.
        BoardEntity *entity = board->getEntity(intermediatePos);
        if (entity != nullptr) {
          bool isSnake = (entity->name() == "SNAKE");
          if (isSnake) {
            cout << "Oh no! Snake at " << intermediatePos << "! Going down to " << newPos << endl;
            notify(currentPlayer->getName() + " encountered snake at " +
                   to_string(intermediatePos) + " now going down to " + to_string(newPos));
          } else {
            cout << "Great! Ladder at " << intermediatePos << "! Going up to " << newPos << endl;
            notify(currentPlayer->getName() + " encountered ladder at " +
                   to_string(intermediatePos) + " now going up to " + to_string(newPos));
          }
        }

        notify(currentPlayer->getName() + " played. New Position : " + to_string(newPos));
        displayPlayerPositions();

        // Win parameters verification check.
        if (rules->checkWinCondition(newPos, board->getBoardSize())) {
          cout << "\n" << currentPlayer->getName() << " wins!" << endl;
          currentPlayer->incrementScore();

          notify("Game Ended. Winner is : " + currentPlayer->getName());
          gameOver = true;
        } else {
          // Round-robin scheduling loop queue positioning update.
          players.pop_front();
          players.push_back(currentPlayer);
        }
      } else {
        // Targeted limit overshoot error messages
        cout << "Need exact roll to reach " << board->getBoardSize() << "!" << endl;
        // Shift active user turn indicators to end of queue list.
        players.pop_front();
        players.push_back(currentPlayer);
      }
    }
  }

  // Destructor: clean strategy instance parameters setup checks.
  ~SnakeAndLadderGame() { 
    delete rules; 
  }
};

// ============================================================================
// 8. FACTORY DESIGN PATTERN (Decoupled Game Instantiation logic)
// ============================================================================
// Direct creation coordinates validation encapsulates dynamic flows of initialization.

class SnakeAndLadderGameFactory {
public:
  // createStandardGame: Traditional classic game creation helper static method.
  static SnakeAndLadderGame *createStandardGame() {
    Board *board = new Board(10); // Classic 10x10 size grid setup.
    BoardSetupStrategy *strategy = new StandardBoardSetupStrategy();
    board->setupBoard(strategy);
    delete strategy;

    Dice *dice = new Dice(6); // Standard 6-faced dice generator setup.

    return new SnakeAndLadderGame(board, dice);
  }

  // createRandomGame: Random game settings layout initialization.
  static SnakeAndLadderGame *createRandomGame(int boardSize, RandomBoardSetupStrategy::Difficulty difficulty) {
    Board *board = new Board(boardSize);
    BoardSetupStrategy *strategy = new RandomBoardSetupStrategy(difficulty);
    board->setupBoard(strategy);
    delete strategy;

    Dice *dice = new Dice(6);

    return new SnakeAndLadderGame(board, dice);
  }

  // createCustomGame: Manual/Custom count placements coordinates initializer.
  static SnakeAndLadderGame *createCustomGame(int boardSize, BoardSetupStrategy *strategy) {
    Board *board = new Board(boardSize);
    board->setupBoard(strategy);

    Dice *dice = new Dice(6);

    return new SnakeAndLadderGame(board, dice);
  }
};

// ============================================================================
// 9. CLIENT DRIVER ENTRY POINT
// ============================================================================
int main() {
  cout << "=== SNAKE AND LADDER GAME ===" << endl;

  SnakeAndLadderGame *game = nullptr;
  Board *board = nullptr;

  cout << "Choose game setup:" << endl;
  cout << "1. Standard Game (10x10 board with traditional positions)" << endl;
  cout << "2. Random Game with Difficulty" << endl;
  cout << "3. Custom Game" << endl;

  int choice;
  cin >> choice;

  // Setup selector conditions
  if (choice == 1) {
    game = SnakeAndLadderGameFactory::createStandardGame();
    board = new Board(10);

  } else if (choice == 2) {
    int boardSize;
    cout << "Enter board size (e.g., 10 for 10x10 board): ";
    cin >> boardSize;

    cout << "Choose difficulty:" << endl;
    cout << "1. Easy (more ladders)" << endl;
    cout << "2. Medium (balanced)" << endl;
    cout << "3. Hard (more snakes)" << endl;

    int diffChoice;
    cin >> diffChoice;

    RandomBoardSetupStrategy::Difficulty diff;
    switch (diffChoice) {
    case 1:
      diff = RandomBoardSetupStrategy::EASY;
      break;
    case 2:
      diff = RandomBoardSetupStrategy::MEDIUM;
      break;
    case 3:
      diff = RandomBoardSetupStrategy::HARD;
      break;
    default:
      diff = RandomBoardSetupStrategy::MEDIUM;
    }

    game = SnakeAndLadderGameFactory::createRandomGame(boardSize, diff);
    board = new Board(boardSize);

  } else if (choice == 3) {
    int boardSize;
    cout << "Enter board size (e.g., 10 for 10x10 board): ";
    cin >> boardSize;

    cout << "Choose custom setup type:" << endl;
    cout << "1. Specify counts only (random placement)" << endl;
    cout << "2. Specify exact positions" << endl;

    int customChoice;
    cin >> customChoice;

    if (customChoice == 1) {
      int numSnakes, numLadders;
      cout << "Enter number of snakes: ";
      cin >> numSnakes;
      cout << "Enter number of ladders: ";
      cin >> numLadders;

      BoardSetupStrategy *strategy = new CustomCountBoardSetupStrategy(numSnakes, numLadders, true);
      game = SnakeAndLadderGameFactory::createCustomGame(boardSize, strategy);
      delete strategy;

    } else {
      int numSnakes, numLadders;
      cout << "Enter number of snakes: ";
      cin >> numSnakes;
      cout << "Enter number of ladders: ";
      cin >> numLadders;

      CustomCountBoardSetupStrategy *strategy = new CustomCountBoardSetupStrategy(numSnakes, numLadders, false);

      // Get custom snake positions coordinates inputs
      for (int i = 0; i < numSnakes; i++) {
        int start, end;
        cout << "Enter snake " << (i + 1) << " start and end positions: ";
        cin >> start >> end;
        strategy->addSnakePosition(start, end);
      }

      // Get custom ladder positions coordinates inputs
      for (int i = 0; i < numLadders; i++) {
        int start, end;
        cout << "Enter ladder " << (i + 1) << " start and end positions: ";
        cin >> start >> end;
        strategy->addLadderPosition(start, end);
      }

      game = SnakeAndLadderGameFactory::createCustomGame(boardSize, strategy);
      delete strategy;
    }

    board = new Board(boardSize);
  }

  // Safe checks
  if (game == nullptr) {
    cout << "Invalid choice!" << endl;
    return 1;
  }

  // Create and Add Observer Notification triggers link
  IObserver *notifier = new SnakeAndLadderConsoleNotifier();
  game->addObserver(notifier);

  // Players sizing configurations
  int numPlayers;
  cout << "Enter number of players: ";
  cin >> numPlayers;

  for (int i = 0; i < numPlayers; i++) {
    string name;
    cout << "Enter name for player " << (i + 1) << ": ";
    cin >> name;
    SnakeAndLadderPlayer *player = new SnakeAndLadderPlayer(i + 1, name);
    game->addPlayer(player);
  }

  // Play target game triggering thread
  game->play();

  // Dynamic resources cleanups
  delete game;
  delete board;
  delete notifier;

  return 0;
}
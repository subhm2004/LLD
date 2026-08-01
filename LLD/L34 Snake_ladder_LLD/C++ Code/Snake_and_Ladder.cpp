// UML banate time is game me bhi TOP DOWN approach use kiya gaya hai (jaisa
// TicTacToe me tha): sabse pehle Observer pattern, fir Board/Entity models,
// fir Strategy pattern (board setup + rules), fir Game controller, fir
// Factory pattern, aur sabse last me Client (main) code. Isse UML diagram me
// har component ka relation upar se neeche clearly dikh raha hai.

// ============================================================================
//  SnakeAndLadder.cpp  —  POORA Snake & Ladder system EK HI FILE me (L33 - LLD)
// ----------------------------------------------------------------------------
//  ============================================================================
//   IS FILE ME 3 DESIGN PATTERNS — ek nazar me
//  ----------------------------------------------------------------------------
//   1. OBSERVER  -> IObserver / SnakeAndLadderConsoleNotifier : game ke
//                   events (game start, dice roll, snake bite, ladder climb,
//                   win) hone par saare registered listeners ko automatically
//                   notification milti hai.
//   2. STRATEGY  -> do jagah use hua hai:
//                     a) BoardSetupStrategy / StandardBoardSetupStrategy /
//                        RandomBoardSetupStrategy / CustomCountBoardSetupStrategy
//                        — board par snakes/ladders KAISE place honge, ye
//                        algorithm swappable hai.
//                     b) SnakeAndLadderRules / StandardSnakeAndLadderRules
//                        — move valid hai ya nahi, naya position kya hoga,
//                        win kab hoga — ye RULES bhi swappable hain.
//   3. FACTORY   -> SnakeAndLadderGameFactory : client ko khud Board, Dice,
//                   Strategy sab manually wire karne ki zaroorat nahi —
//                   factory ek hi call me pura ready-made game de deti hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ CELL-INDEX CONVENTION — is file ka sabse zaroori niyam                │
//  │                                                                          │
//  │  Board me entities (Snake/Ladder) ek `map<int, BoardEntity*>` me         │
//  │  unke START position ke against store hoti hain.                        │
//  │                                                                          │
//  │  - Player jab kisi cell par land karta hai, to us cell ko                │
//  │    `boardEntities` map me START key ki tarah dhoonda jata hai.           │
//  │  - Agar entity mil jaye, to player ki final position uski END ho         │
//  │    jaati hai (Snake me end < start => neeche; Ladder me end > start      │
//  │    => upar).                                                             │
//  │  - Isliye Snake aur Ladder ka START position kabhi bhi ek dusre ke ya    │
//  │    khud apne START ke saath CLASH nahi hona chahiye — warna map me       │
//  │    overwrite ho jayega (`canAddEntity()` isi cheez se bachata hai).      │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#include <cstdlib>
#include <ctime>
#include <deque>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

// ============================================================================
//  1. OBSERVER DESIGN PATTERN  (Notification / Event Logging System)
// ----------------------------------------------------------------------------
//  Problem : Game me jab bhi kuch important hota hai (game start hua, kisi ne
//            dice roll kiya, koi snake ne kaata, koi ladder chadha, ya koi
//            jeet gaya) to ye khabar alag-alag jagah pahunchani pad sakti hai
//            — console pe, UI pe, log file me, analytics dashboard me...
//
//  Solution: Observer pattern. Game (Subject) ke paas observers ki ek list
//            hoti hai. Event hone par game bas `notify(msg)` bol deta hai aur
//            list ke SAARE observers ka `update()` apne aap call ho jata hai.
//
//  Fayda   : Game logic aur reporting/notification logic DECOUPLED rehte hain.
//            Kal ko FileLogger ya UINotifier add karna ho to sirf ek nayi
//            IObserver subclass banao aur register kar do — SnakeAndLadderGame
//            ki ek line bhi nahi badalni padegi (Open/Closed Principle).
// ============================================================================

// IObserver: Sabhi Concrete Observers ke liye common CONTRACT (abstract
// interface). Game sirf is interface ko jaanta hai, concrete classes ko nahi.
class IObserver
{
public:
  // update(): Yeh function tab call hota hai jab bhi subject me koi state
  // change ho. `msg` wahi notification event message hai jo game loop ne
  // bheja hai.
  virtual void update(string msg) = 0;

  // Virtual destructor zaroori hai: base class pointer (IObserver*) se
  // delete karne par derived class ka destructor sahi se chale, warna
  // memory leak ho sakta hai.
  virtual ~IObserver() {}
};

// SnakeAndLadderConsoleNotifier: IObserver ka ek CONCRETE implementation.
// Iska single kaam hai: jo bhi event mile, use "[NOTIFICATION]" prefix laga
// kar terminal (console) standard output par print kar dena.
class SnakeAndLadderConsoleNotifier : public IObserver
{
public:
  void update(string msg) override
  {
    cout << "[NOTIFICATION] " << msg << endl;
  }
};

// ============================================================================
//  2. DICE MODEL
// ----------------------------------------------------------------------------
//  Dice ek chhoti si UTILITY class hai jo random numbers generate karti hai.
//  Ise seedha `rand() % 6` ki jagah class me isliye wrap kiya gaya hai taaki
//  future me weighted dice, ya different faces (jaise 4-sided/20-sided dice)
//  chahiye ho to sirf yahi ek jagah badalni pade.
//
//  ⭐ getRandomInRange — is class ke andar hi ek STATIC utility method hai
//  ----------------------------------------------------------------------------
//  Problem : Is file me BOHOT saari jagah "ek RANGE ke beech random integer
//            chahiye" wala pattern repeat ho raha hai — dice roll ho (1 se
//            faces tak), ya board par snake/ladder ka random start-end
//            position ho (jaise 10 se boardSize tak). Har jagah manually
//            `rand() % (max - min + 1) + min` type formula likhna:
//              - REPETITIVE hai (DRY principle todta hai),
//              - aur galti se off-by-one error (jaise ek extra `+1` bhool
//                jaana) hone ka risk hamesha rehta hai.
//
//  Solution: `Dice` class ke andar hi ek `static` method `getRandomInRange`
//            bana diya — kyunki randomness generate karna conceptually
//            "Dice/randomness" ki hi responsibility hai, isliye isko
//            bahar ek loose global function ki jagah Dice class ke andar
//            rakhna zyada COHESIVE (Object-Oriented) design hai.
//
//            `static` isliye kyunki:
//              - Ye kisi PARTICULAR Dice OBJECT ki property par depend nahi
//                karta (jaise `faces` member par nahi), sirf pure diye gaye
//                `min`/`max` par depend karta hai.
//              - Isliye ise बिना koi Dice OBJECT banaye seedha
//                `Dice::getRandomInRange(min, max)` likh kar call kar
//                sakte hain — bilkul jaise `roll()` andar se call karta hai.
//
//  Formula ka breakdown:
//      rand() % (max - min + 1)   =>  ye [0, (max-min)] tak ki range deta hai
//                                       (kyunki `%` operator hamesha 0 se
//                                       (divisor - 1) tak result deta hai)
//      + min                       =>  is 0-based range ko SHIFT karke
//                                       [min, max] bana deta hai
//
//  Example: getRandomInRange(10, 20)
//      max - min + 1 = 11  =>  rand() % 11 gives [0, 10]
//      + 10                =>  final range becomes [10, 20]  ✅ (dono inclusive)
// ============================================================================

// Dice: Game me numbers generate karne ke liye standard Dice representation.
class Dice
{
private:
  int faces; // Dice ke total number of sides/faces (jaise traditional dice
             // me 6 faces hote hain, 1 se 6 tak).

public:
  // getRandomInRange — [min, max] (dono INCLUSIVE) ke beech ek random
  // integer return karta hai. STATIC hai, isliye class ke bahar se bhi
  // `Dice::getRandomInRange(min, max)` likh kar bina Dice object banaye
  // directly call kiya ja sakta hai — Board setup strategies (RandomBoard,
  // CustomCountBoard) isi tarah use karti hain.
  static int getRandomInRange(int min, int max)
  {
    // `rand() % (max - min + 1)` => [0, max-min] range ka random offset
    // `+ min`                     => us offset ko `min` se shift karke
    //                                 final range [min, max] bana deta hai
    return min + (rand() % (max - min + 1));
  }

  // Constructor: Dice initialize karta hai faces quantity ke saath, aur
  // random seed (`srand`) bhi initialize karta hai — seed current TIME
  // (`time(0)`) se li jaati hai taaki HAR PROGRAM RUN me alag-alag random
  // sequence mile (warna `rand()` har baar SAME predictable numbers dega).
  Dice(int f)
  {
    faces = f;
    srand(time(0)); // Random number generator engine ko "current time" se
                    // seed kiya, taaki har run pe different dice values milein.
  }

  // roll(): Dice ko roll karta hai aur 1 se `faces` tak (dono inclusive) ka
  // random integer return karta hai.
  //
  // Pehle isko manually `(rand() % faces) + 1` likha jata tha, ab hum apna
  // class ke andar wala static `getRandomInRange(min, max)` helper use kar
  // rahe hain — same result, par cleaner aur less error-prone (min/max
  // explicitly dikhte hain, koi manual `%` / `+1` arithmetic khud se sochni
  // nahi padti). NOTE: static method hone ki wajah se isko seedha naam se
  // (`getRandomInRange(...)`) call kar sakte hain, `this->` ya `Dice::`
  // likhne ki zaroorat nahi kyunki hum khud isi class ke andar hain.
  int roll()
  {
    return getRandomInRange(1, faces); // Range: 1 se faces tak (jaise 1-6).
  }
};

// ============================================================================
//  3. BOARD ENTITIES  (Snakes & Ladders models using Inheritance)
// ----------------------------------------------------------------------------
//  Snake aur Ladder dono ka COMMON BEHAVIOR ek hi hai: dono ke paas ek START
//  position hoti hai (jahan player land karta hai) aur ek END position
//  (jahan wo teleport ho jata hai). Ye common shape ek abstract base class
//  `BoardEntity` me extract kar diya gaya hai — isse Single Responsibility
//  aur code reuse dono milte hain (Snake aur Ladder sirf apna VALIDATION aur
//  DISPLAY/NAME logic define karte hain, baaki sab base class se aata hai).
// ============================================================================

// BoardEntity: Snake aur Ladder dono ke start aur end positions common hote
// hain. Is common behavior ko extract karke yeh abstract base class banayi
// gayi hai (Polymorphism ke through Board is class ko generically treat
// karta hai, use pata nahi hota ki actual me Snake hai ya Ladder).
class BoardEntity
{
protected:
  int startPosition; // Entity ka starting index cell number (jahan player
                     // land karega tabhi ye entity "trigger" hoti hai).
  int endPosition;   // Destination index cell number (jahan landing ke
                     // baad player teleport hoke pahunchega).

public:
  // Constructor: Start aur End cell configuration set karta hai. Ye base
  // class ka constructor hai — Snake/Ladder apne constructor me isi ko
  // (base-class-initializer-list se) call karte hain.
  BoardEntity(int start, int end)
  {
    startPosition = start;
    endPosition = end;
  }

  // getStart(): Us cell ka index jahan is entity ko "step on" karne se
  // trigger hota hai.
  int getStart()
  {
    return startPosition;
  }

  // getEnd(): Us cell ka index jahan player teleport hoke pahunchega.
  int getEnd()
  {
    return endPosition;
  }

  // Pure virtual methods — har derived class (Snake/Ladder) ko apna
  // DISPLAY format aur apna NAME string batana padega. Isse Board class
  // bina "if Snake else Ladder" likhe generically `entity->display()` ya
  // `entity->name()` bula sakta hai (Polymorphism).
  virtual void display() = 0;
  virtual string name() = 0;

  // Virtual destructor — base pointer (`BoardEntity*`) se delete karne par
  // derived (Snake/Ladder) destructor bhi sahi se chale, memory leak na ho.
  virtual ~BoardEntity() {}
};

// Snake class: BoardEntity se INHERIT karti hai (is-a relationship — "Snake
// IS-A BoardEntity"). Snake ka niyam: startPosition HAMESHA endPosition se
// BADA hona chahiye, kyunki snake ke "mooh" (start, upar wala cell) me
// player land karta hai aur "poonch" (end, neeche wala cell) tak fisal jata
// hai — matlab player NEECHE jata hai.
class Snake : public BoardEntity
{
public:
  // Constructor — base class (BoardEntity) ka constructor initializer-list
  // se call karta hai, phir apna sanity/validation check karta hai.
  Snake(int start, int end) : BoardEntity(start, end)
  {
    if (end >= start)
    {
      // Logic boundary warning check: agar end >= start diya gaya hai, to
      // ye ek INVALID snake hai (snake to hamesha "neeche le jaane wala"
      // hona chahiye) — bas warning print karte hain, object phir bhi ban
      // jata hai (production code me yahan exception throw karna better
      // hoga, par yahan simplicity ke liye sirf warning hai).
      cout << "Invalid snake! End must be less than start." << endl;
    }
  }

  // display(): Console screen par is snake ka "start -> end" layout
  // dikhata hai, taaki debugging/board-overview me saaf dikhe.
  void display() override
  {
    cout << "Snake: " << startPosition << " -> " << endPosition << endl;
  }

  // name(): Game logic me (jaise game loop me) entity ka TYPE identify
  // karne ke liye string tag — "SNAKE" ya "LADDER" compare karke pata
  // chalta hai ki player ke saath kya hua.
  string name() override
  {
    return "SNAKE";
  }
};

// Ladder class: BoardEntity se INHERIT karti hai. Ladder ka niyam Snake ke
// bilkul ULTA hai — endPosition HAMESHA startPosition se BADA hona chahiye,
// kyunki ladder chadh kar player UPAR jata hai.
class Ladder : public BoardEntity
{
public:
  Ladder(int start, int end) : BoardEntity(start, end)
  {
    if (end <= start)
    {
      // Agar end <= start hai to ye INVALID ladder hai (ladder to upar le
      // jaane wali honi chahiye) — warning print, boundary sanity check.
      cout << "Invalid ladder! End must be greater than start." << endl;
    }
  }

  // display(): Ladder ka bhi visual "start -> end" coordinate representation.
  void display() override
  {
    cout << "Ladder: " << startPosition << " -> " << endPosition << endl;
  }

  // name(): Identity verification tag — game loop isi se decide karta hai
  // ki player "neeche gira" (SNAKE) ya "upar chadha" (LADDER).
  string name() override
  {
    return "LADDER";
  }
};

// ============================================================================
//  4. STRATEGY DESIGN PATTERN #1  (Board Layout Initialization Setup Strategies)
// ----------------------------------------------------------------------------
//  Problem : Board ko setup karne ke alag-alag tarike ho sakte hain —
//            (a) Standard traditional fixed positions (classic 10x10 game),
//            (b) Random configuration jisme difficulty (Easy/Medium/Hard) ke
//                hisaab se snakes/ladders ka RATIO change ho,
//            (c) User apna khud ka custom configuration de (ya to sirf
//                COUNT bata de aur positions random ho jayein, ya har snake/
//                ladder ka EXACT start-end khud specify kare).
//            Agar ye teeno tarike Board class ke andar hi if-else me likh
//            dete, to Board class bohot bhaari aur rigid ho jaati.
//
//  Solution: Strategy pattern. `BoardSetupStrategy` ek abstract interface hai
//            jo sirf ek kaam janta hai: `setupBoard(Board*)`. Har concrete
//            strategy (Standard/Random/CustomCount) apne andar apna algorithm
//            rakhti hai ki KAISE entities place karni hain. Board class ko
//            farak nahi padta ki kaunsi strategy use ho rahi hai.
//
//  Fayda   : Naya setup-mode chahiye (jaise "Themed Board" ya "Tournament
//            Board")? Bas ek nayi BoardSetupStrategy subclass likho — Board
//            class ki ek line bhi nahi badalni padegi.
// ============================================================================

// Forward declaration — BoardSetupStrategy interface abhi neeche define hoga,
// par Board class ke `setupBoard(BoardSetupStrategy*)` method ki SIGNATURE
// me iska naam pehle hi chahiye, isliye compiler ko "aisi ek class aayegi"
// bata diya gaya hai (actual definition baad me).
class BoardSetupStrategy;

// Board class: Board layout configuration aur matrix properties ki storage.
// Ye class khud entities KAISE place hoti hain wo NAHI jaanti (wo Strategy
// ka kaam hai) — Board sirf ITNA jaanta hai ki entity ko SAFELY store kaise
// karna hai aur GIVEN position par entity KAISE lookup karni hai.
class Board
{
private:
  int size;                               // Total board dimension capacity
                                          // (M * M cells grid, jaise
                                          // 10x10 board => size=100).
  vector<BoardEntity *> snakesAndLadders; // Board entities ki dynamic
                                          // array/list storage — display
                                          // aur cleanup (destructor) ke
                                          // liye "sabko iterate karo"
                                          // wale use-cases ke liye.
  map<int, BoardEntity *> boardEntities;  // Cell-index => entity mapping,
                                          // taaki O(log n) me fast lookup
                                          // ho sake ki "kya is cell par
                                          // koi entity hai?" (vector me
                                          // linear search karna padta,
                                          // map isse fast bana deta hai).

public:
  // Constructor — dynamic sizing: user "N" deta hai (jaise 10 => 10x10 board),
  // aur hum use N*N total cells me convert kar dete hain (jaise size=100).
  Board(int s)
  {
    size = s * s; // dynamic sizing (M*M) setup.
  }

  // canAddEntity: Check karta hai ki targeted position (cell) par PEHLE SE
  // koi snake ya ladder to nahi already baithi hai. `boardEntities.find()`
  // agar `end()` return kare, matlab key exist NAHI karti — matlab cell
  // free hai, entity add ki ja sakti hai.
  bool canAddEntity(int position)
  {
    return boardEntities.find(position) == boardEntities.end();
  }

  // addBoardEntity: Valid entities ko dono data-structures (list + map) me
  // safely store karta hai. Pehle `canAddEntity` se check karta hai taaki
  // koi START position DUPLICATE/OVERWRITE na ho jaye.
  void addBoardEntity(BoardEntity *boardEntity)
  {
    if (canAddEntity(boardEntity->getStart()))
    {
      snakesAndLadders.push_back(boardEntity);              // Iteration/display ke liye list me daalo.
      boardEntities[boardEntity->getStart()] = boardEntity; // Fast lookup ke liye map me daalo.
    }
  }

  // setupBoard: Dynamic strategy parameter run karke Board ki actual
  // filling karwata hai (Strategy Injection / dependency inversion — Board
  // khud concrete strategy class ko NAHI jaanta, sirf interface ko).
  // ACTUAL definition class ke NEECHE likhi hai kyunki tab tak
  // BoardSetupStrategy poori define ho chuki hogi.
  void setupBoard(BoardSetupStrategy *strategy);

  // getEntity: Diye gaye cell-index par agar entity hai to uska pointer
  // lauta do, warna `nullptr`. Game loop isi se check karta hai ki player
  // jahan land hua wahan snake/ladder to nahi.
  BoardEntity *getEntity(int position)
  {
    if (boardEntities.find(position) != boardEntities.end())
    {
      return boardEntities[position];
    }
    return nullptr; // Us coordinate par koi entity present nahi hai.
  }

  // getBoardSize() — total cells count (M*M). Rules classes isi se
  // "overflow" (board ke bahar move) check karti hain.
  int getBoardSize()
  {
    return size;
  }

  // display(): Poore board ka SUMMARY console par print karta hai — kitne
  // snakes hain, kitne ladders, aur har ek ka start->end.
  void display()
  {
    cout << "\n=== Board Configuration ===" << endl;
    cout << "Board Size: " << size << " cells" << endl;

    // Pehle dono entities ki COUNT nikalo (loop ek baar chalake).
    int snakeCount = 0;
    int ladderCount = 0;
    for (auto entity : snakesAndLadders)
    {
      if (entity->name() == "SNAKE")
        snakeCount++;
      else
        ladderCount++;
    }

    // Fir dobara loop chalake sirf SNAKES print karo.
    cout << "\nSnakes: " << snakeCount << endl;
    for (auto entity : snakesAndLadders)
    {
      if (entity->name() == "SNAKE")
      {
        entity->display(); // Polymorphic call — Snake ka apna display() chalega.
      }
    }

    // Fir dobara loop chalake sirf LADDERS print karo.
    cout << "\nLadders: " << ladderCount << endl;
    for (auto entity : snakesAndLadders)
    {
      if (entity->name() == "LADDER")
      {
        entity->display(); // Polymorphic call — Ladder ka apna display() chalega.
      }
    }
    cout << "=========================" << endl;
  }

  // Destructor: Board jo entities khud "OWN" karta hai (snakesAndLadders
  // vector me stored) unko delete karta hai — memory leak se bachne ke liye.
  // NOTE: `boardEntities` map me SAME pointers hain (duplicate ownership
  // nahi), isliye map ko dobara delete karne ki zaroorat nahi.
  ~Board()
  {
    for (auto entity : snakesAndLadders)
    {
      delete entity;
    }
  }
};

// BoardSetupStrategy: Strategy pattern ka abstract INTERFACE class Board
// Setup ke liye. Har concrete strategy isi ek method ko implement karti hai.
class BoardSetupStrategy
{
public:
  // setupBoard: Diye gaye Board object ko ENTITIES se bhar do — KAISE bharna
  // hai ye decide karna is concrete strategy ka apna kaam hai.
  virtual void setupBoard(Board *board) = 0;
  virtual ~BoardSetupStrategy() {}
};

// RandomBoardSetupStrategy: BoardSetupStrategy ka ek CONCRETE implementation.
// Ye random coordinates generate karke, difficulty ke hisaab se snakes aur
// ladders ka RATIO adjust karke board fill karta hai.
class RandomBoardSetupStrategy : public BoardSetupStrategy
{
public:
  // Difficulty — kitni "meherbaan" ya "sakht" board hogi, uska enum tag.
  enum Difficulty
  {
    EASY,   // Zyada ladders (jeetna aasan), kam snakes.
    MEDIUM, // Snakes aur ladders ka balanced density.
    HARD    // Zyada snakes (jeetna mushkil/painful), kam ladders.
  };

private:
  Difficulty difficulty; // Konsi difficulty select hui hai, wo store karta hai.

  // setupWithProbability: Probability-based entity placement ka MAIN
  // ALGORITHM. `snakeProbability` decide karta hai ki har entity SNAKE
  // banegi ya LADDER — jitni high probability, utne zyada snakes.
  void setupWithProbability(Board *board, double snakeProbability)
  {
    int boardSize = board->getBoardSize();
    int totalEntities = boardSize / 10; // Heuristic: roughly board ka ~10%
                                        // hissa entities (snakes+ladders)
                                        // se bhara jayega.

    for (int i = 0; i < totalEntities; i++)
    {
      // NOTE: Ye probability ek FRACTION [0.0, 1.0) hai, integer range nahi
      // — isliye yahan `getRandomInRange` (jo INTEGER range ke liye hai)
      // use nahi hota, seedha `rand() / RAND_MAX` hi rehta hai.
      double prob = (double)rand() / RAND_MAX;

      if (prob < snakeProbability)
      {
        // === SNAKE creation loop ===
        // Attempts bounded rakhe hain (max 50 tries) taaki agar board
        // "bhar chuka" ho aur koi free cell na mile, to INFINITE LOOP me
        // atak na jaye — bas skip ho jayega.
        int attempts = 0;
        while (attempts < 50)
        {
          // start: range [10, boardSize-1] — minimum 10 index se upar start
          // rakha (bahut shuru me snake avoid karne ke liye).
          // NOTE: `getRandomInRange` ab Dice class ke andar STATIC method
          // hai, isliye class ke BAHAR se `Dice::getRandomInRange(...)`
          // likh kar call karna padta hai (Dice ka object banane ki
          // zaroorat nahi, `Dice::` sirf "ye method kis class ka hai" bata
          // raha hai).
          int start = Dice::getRandomInRange(10, boardSize - 1);
          // end: range [1, start-1] — end HAMESHA start se NEECHE (snake ka
          // niyam maintain: player neeche jaayega).
          int end = Dice::getRandomInRange(1, start - 1);

          if (board->canAddEntity(start))
          {
            board->addBoardEntity(new Snake(start, end));
            break; // Successfully placed — is entity ke liye loop khatam.
          }
          attempts++;
        }
      }
      else
      {
        // === LADDER creation loop ===
        int attempts = 0;
        while (attempts < 50)
        {
          // start: range [1, boardSize-10].
          int start = Dice::getRandomInRange(1, boardSize - 10);
          // end: range [start+1, boardSize] — end HAMESHA start se UPAR
          // (ladder ka niyam maintain: player upar jaayega).
          int end = Dice::getRandomInRange(start + 1, boardSize);

          if (board->canAddEntity(start) && end < boardSize)
          {
            board->addBoardEntity(new Ladder(start, end));
            break;
          }
          attempts++;
        }
      }
    }
  }

public:
  // Constructor — user jo difficulty chunta hai wo store kar leta hai.
  RandomBoardSetupStrategy(Difficulty d)
  {
    difficulty = d;
  }

  // setupBoard: Difficulty ke hisaab se snake-probability set karke actual
  // placement function ko call karta hai.
  void setupBoard(Board *board) override
  {
    switch (difficulty)
    {
    case EASY:
      setupWithProbability(board, 0.3); // 30% Snakes, 70% Ladders — aasan.
      break;
    case MEDIUM:
      setupWithProbability(board, 0.5); // 50% Snakes, 50% Ladders — balanced.
      break;
    case HARD:
      setupWithProbability(board, 0.7); // 70% Snakes, 30% Ladders — mushkil.
      break;
    }
  }
};

// CustomCountBoardSetupStrategy: BoardSetupStrategy ka ek aur CONCRETE
// implementation. Is strategy me user KHUD decide karta hai kitne snakes
// aur kitne ladders chahiye — aur EK CHOICE bhi deta hai:
//   (a) positions RANDOM generate ho jayein (bas count do), YA
//   (b) user har entity ka EXACT start-end khud specify kare.
class CustomCountBoardSetupStrategy : public BoardSetupStrategy
{
private:
  int numSnakes;                          // User ne kitne snakes maange.
  int numLadders;                         // User ne kitne ladders maange.
  bool randomPositions;                   // true => positions random generate
                                          // hongi; false => user ne manually
                                          // exact start-end diya hai.
  vector<pair<int, int>> snakePositions;  // Manual mode ke liye: user-diye
                                          // snake (start,end) pairs ki list.
  vector<pair<int, int>> ladderPositions; // Manual mode ke liye: user-diye
                                          // ladder (start,end) pairs ki list.

public:
  // Constructor — snakes/ladders ki desired COUNT aur mode (random vs manual)
  // set karta hai.
  CustomCountBoardSetupStrategy(int snakes, int ladders, bool random)
  {
    numSnakes = snakes;
    numLadders = ladders;
    randomPositions = random;
  }

  // addSnakePosition / addLadderPosition — MANUAL mode me client (main())
  // in methods se ek-ek karke exact coordinates feed karta hai, isse pehle
  // setupBoard() call karega.
  void addSnakePosition(int start, int end)
  {
    snakePositions.push_back(make_pair(start, end));
  }

  void addLadderPosition(int start, int end)
  {
    ladderPositions.push_back(make_pair(start, end));
  }

  // setupBoard: `randomPositions` flag ke hisaab se do alag paths lete hai.
  void setupBoard(Board *board) override
  {
    if (randomPositions)
    {
      // === RANDOM PATH: sirf count pata hai, positions khud generate karo ===
      int boardSize = board->getBoardSize();

      // Add snakes loop — jab tak requested COUNT poori na ho jaye, tab tak
      // random valid position dhoondte raho.
      int snakesAdded = 0;
      while (snakesAdded < numSnakes)
      {
        int start = Dice::getRandomInRange(10, boardSize - 1); // range [10, boardSize-1]
        int end = Dice::getRandomInRange(1, start - 1);        // range [1, start-1]

        if (board->canAddEntity(start))
        {
          board->addBoardEntity(new Snake(start, end));
          snakesAdded++;
        }
        // NOTE: agar canAddEntity false ho, hum bas loop dobara chalate hain
        // (naya random start try hoga) — attempts-bound yahan nahi hai,
        // isliye agar board bahut chhota/bhara ho to ye loop lambi chal
        // sakti hai. (Design tradeoff: simplicity vs strict bounding.)
      }

      // Add ladders loop — same tarika, ladders ke liye.
      int laddersAdded = 0;
      while (laddersAdded < numLadders)
      {
        int start = Dice::getRandomInRange(1, boardSize - 10);  // range [1, boardSize-10]
        int end = Dice::getRandomInRange(start + 1, boardSize); // range [start+1, boardSize]

        if (board->canAddEntity(start) && end < boardSize)
        {
          board->addBoardEntity(new Ladder(start, end));
          laddersAdded++;
        }
      }
    }
    else
    {
      // === MANUAL PATH: user-diye exact coordinates use karo ===
      for (auto &pos : snakePositions)
      {
        if (board->canAddEntity(pos.first))
        {
          board->addBoardEntity(new Snake(pos.first, pos.second));
        }
        // Agar position clash ho rahi ho (canAddEntity false), to us entry
        // ko SILENTLY SKIP kar diya jata hai.
      }

      for (auto &pos : ladderPositions)
      {
        if (board->canAddEntity(pos.first))
        {
          board->addBoardEntity(new Ladder(pos.first, pos.second));
        }
      }
    }
  }
};

// StandardBoardSetupStrategy: BoardSetupStrategy ka teesra CONCRETE
// implementation. Ye HARD-CODED classic Snake & Ladder positions use karta
// hai (wahi traditional board jo bachpan me khelte the) — isliye ye SIRF
// 10x10 (size=100) board ke liye kaam karta hai.
class StandardBoardSetupStrategy : public BoardSetupStrategy
{
public:
  void setupBoard(Board *board) override
  {
    // Basic verification: agar board 10x10 nahi hai, to classic
    // hardcoded coordinates board ke bounds se bahar jaake INVALID ho
    // sakti hain — isliye pehle hi guard laga diya.
    if (board->getBoardSize() != 100)
    {
      cout << "Standard setup only works for 10x10 board!" << endl;
      return;
    }

    // Classic snake placement — traditional fixed coordinates.
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

    // Classic ladder placement — traditional fixed coordinates.
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

// Board::setupBoard ki ACTUAL definition — yahan isliye di hai kyunki ab
// tak BoardSetupStrategy poori define ho chuki hai (upar sirf forward
// declaration tha). Ye method Strategy Pattern ka "injection point" hai:
// Board sirf itna karta hai ki jo bhi strategy object mile, uske
// `setupBoard(this)` ko call kar de — khud koi entity-placement logic NAHI
// rakhta.
void Board::setupBoard(BoardSetupStrategy *strategy)
{
  strategy->setupBoard(this);
}

// ============================================================================
//  5. PLAYER MODEL
// ----------------------------------------------------------------------------
//  Player ki basic details ka simple data holder — id, naam, current cell
//  position, aur ab tak ke wins ka score.
// ============================================================================

// SnakeAndLadderPlayer: Player configuration properties ki storage model.
class SnakeAndLadderPlayer
{
private:
  int playerId; // Player ki standard index identification.
  string name;  // Player ka display naam.
  int position; // Player ka CURRENT cell index number board coordinate
                // sheet par.
  int score;    // Total tournament wins ka score counter.

public:
  // Constructor — id aur naam set karta hai; position 0 (matlab "board ke
  // bahar/start line par") aur score 0 se shuru hote hain.
  SnakeAndLadderPlayer(int playerId, string n)
  {
    this->playerId = playerId;
    name = n;
    position = 0; // Sabhi players game start par index cell 0 (Board ke
                  // bahar, start point) par hote hain.
    score = 0;
  }

  // ---- Getters and Setters ----

  // getName() — player ka naam.
  string getName()
  {
    return name;
  }

  // getPosition() — player ki abhi ki current cell position.
  int getPosition()
  {
    return position;
  }

  // setPosition() — dice roll / snake / ladder ke baad player ki NAYI
  // position update karne ke liye.
  void setPosition(int pos)
  {
    position = pos;
  }

  // getScore() — ab tak ke total wins.
  int getScore()
  {
    return score;
  }

  // incrementScore() — game jeetne par score +1.
  void incrementScore()
  {
    score++;
  }
};

// ============================================================================
//  6. STRATEGY DESIGN PATTERN #2  (Game Rule validation Strategies)
// ----------------------------------------------------------------------------
//  Problem : "Move valid hai?", "Naya position kya banega (snake/ladder ke
//            baad)?", "Kya player jeet gaya?" — ye teeno checks GAME RULES
//            hain. Agar seedha SnakeAndLadderGame class ke andar likh dete,
//            to naye rule-variants (jaise "exact roll zaroori nahi, extra
//            dena bounce-back ho jaye" jaisa mode) laane ke liye game class
//            hi kholni padti.
//
//  Solution: Strategy pattern. Turn validation aur position-change
//            algorithms ko `SnakeAndLadderRules` interface me isolate kar
//            diya gaya hai. Kal ko rules change karne hon (jaise "start
//            cell par double roll compulsory ho"), to hum easily naya rules
//            strategy likh ke inject kar sakte hain.
// ============================================================================

// SnakeAndLadderRules: Game behavior aur position-change rules ka INTERFACE.
class SnakeAndLadderRules
{
public:
  // isValidMove: Checks karta hai ki current position + dice value board ke
  // index limits se OVERFLOW to nahi kar rahi.
  virtual bool isValidMove(int currentPos, int diceValue, int boardSize) = 0;

  // calculateNewPosition: Naya position calculate karta hai — agar landing
  // cell par koi snake/ladder ho to uska END position resolve karke deta hai.
  virtual int calculateNewPosition(int currentPos, int diceValue, Board *board) = 0;

  // checkWinCondition: Check karta hai ki player EXACT target destination
  // (board ke aakhri cell) tak pahunch gaya hai ya nahi.
  virtual bool checkWinCondition(int position, int boardSize) = 0;

  virtual ~SnakeAndLadderRules() {}
};

// StandardSnakeAndLadderRules: Classic traditional rules ka CONCRETE
// implementation.
class StandardSnakeAndLadderRules : public SnakeAndLadderRules
{
public:
  // isValidMove: Target overflow limit check.
  // (Agar player index 97 pe hai aur dice value 5 aati hai, to move INVALID
  //  hai kyunki 97+5=102, jo board-size (100) se bahar chala jata hai —
  //  classic rule: EXACT roll chahiye jeetne ke liye.)
  bool isValidMove(int currentPos, int diceValue, int boardSize) override
  {
    return (currentPos + diceValue) <= boardSize;
  }

  // calculateNewPosition: Pehle simple addition se "intermediate" position
  // nikalta hai, phir Board se check karta hai ki us cell par koi
  // Snake/Ladder to nahi baithi.
  int calculateNewPosition(int currentPos, int diceValue, Board *board) override
  {
    int newPos = currentPos + diceValue;
    BoardEntity *entity = board->getEntity(newPos); // Checks if target cell
                                                    // has snake/ladder.

    if (entity != nullptr)
    {
      return entity->getEnd(); // Entity mili — updated (teleported) destination.
    }
    return newPos; // Koi entity nahi mili — player wahi newPos par rukega.
  }

  // checkWinCondition: Player ka position EXACTLY boardSize ke barabar
  // hona chahiye — tabhi WIN maana jayega (classic Snake & Ladder me
  // "overshoot" allowed nahi hota).
  bool checkWinCondition(int position, int boardSize) override
  {
    return position == boardSize;
  }
};

// ============================================================================
//  7. GAME SYSTEM CONTROLLER  (Observer pattern ka SUBJECT + Strategy ka CONTEXT)
// ----------------------------------------------------------------------------
//  SnakeAndLadderGame poore game ka ORCHESTRATOR hai:
//   - Board aur Dice reference rakhta hai, players ki turn-order sambhalta
//     hai (deque se round-robin — bilkul TicTacToe wale pattern jaisa),
//     rules se checks karwata hai, observers ko notify karta hai.
//   - Observer pattern me ye SUBJECT hai (observers ki list + notify()).
//   - Strategy pattern me ye CONTEXT hai (rules* ke through kaam karta hai,
//     concrete rules class ko nahi jaanta).
// ============================================================================

// SnakeAndLadderGame: Master controller class jo matches execution loop
// runs manage karti hai.
class SnakeAndLadderGame
{
private:
  Board *board;                          // Grid board memory settings
                                         // parameters reference (Factory
                                         // se banaya gaya, Game khud
                                         // OWN nahi karta — isliye
                                         // destructor me delete nahi
                                         // hota, cleanup client karta hai).
  Dice *dice;                            // Dice utility engine pointer object.
  deque<SnakeAndLadderPlayer *> players; // Round-robin turn scheduling ke
                                         // liye deque — front wala player
                                         // khelta hai, phir (agar jeeta
                                         // nahi) end me chala jata hai.
  SnakeAndLadderRules *rules;            // Rules verification Strategy
                                         // Context object.
  vector<IObserver *> observers;         // Observer array — saare
                                         // registered notification targets.
  bool gameOver;                         // Game active/running iteration
                                         // indicator flag.

public:
  // Constructor: Board aur Dice INJECT hote hain (Factory/client se aate
  // hain — Game khud inhe nahi banata), aur default Standard rules object
  // internally create hota hai.
  SnakeAndLadderGame(Board *b, Dice *d)
  {
    board = b;
    dice = d;
    rules = new StandardSnakeAndLadderRules(); // Default strategy injected setup.
    gameOver = false;
  }

  // addPlayer — player ko turn-queue ke end me jodo. Jis order me add
  // kiya, usi order me pehli baari milegi.
  void addPlayer(SnakeAndLadderPlayer *player)
  {
    players.push_back(player);
  }

  // addObserver — notification listener register karo (Observer pattern).
  void addObserver(IObserver *observer)
  {
    observers.push_back(observer);
  }

  // notify: SAARE registered observers ko ek hi message broadcast karo.
  void notify(string msg)
  {
    for (auto observer : observers)
    {
      observer->update(msg);
    }
  }

  // displayPlayerPositions: Har player ki current position console par
  // dikhata hai — turn ke baad player ko "state" ka snapshot dene ke liye.
  void displayPlayerPositions()
  {
    cout << "\n=== Current Positions ===" << endl;
    for (auto player : players)
    {
      cout << player->getName() << ": " << player->getPosition() << endl;
    }
    cout << "======================" << endl;
  }

  // play: Main game loop — har iteration = ek player ki EK TURN:
  //
  //   Enter dabao -> dice roll karo -> rules se validate karo
  //     |- valid   : naya position calculate karo (snake/ladder resolve
  //     |            karke), notify karo, phir WIN check -> agar nahi jeeta
  //     |            to player ko queue ke end me bhej do (round robin)
  //     |- invalid : "exact roll chahiye" message dikhao, player ko FIR BHI
  //                  end me bhej do (turn skip — TicTacToe se DIFFERENT
  //                  hai: yahan invalid move par retry nahi milta, turn hi
  //                  chala jata hai, kyunki dice pe koi control nahi hota).
  void play()
  {
    // Kam se kam 2 players chahiye, warna game shuru hi nahi hoga.
    if (players.size() < 2)
    {
      cout << "Need at least 2 players!" << endl;
      return;
    }

    notify("Game started"); // Observers ko game-start event.
    board->display();       // Shuru me ek baar poora board layout dikha do.

    // Loop continues until a player triggers win conditions settings
    while (!gameOver)
    {
      // ROUND ROBIN: deque ke front wale player ki turn hai.
      SnakeAndLadderPlayer *currentPlayer = players.front();

      cout << "\n"
           << currentPlayer->getName() << "'s turn. Press Enter to roll dice...";
      cin.ignore(); // Pichle `cin >>` se bacha hua residual newline character
                    // clear karta hai (warna `cin.get()` turant khali Enter
                    // padh ke aage badh jayega bina user ke actual Enter
                    // press ka wait kiye).
      cin.get();    // User ke Enter key press ka wait karta hai — ek chhota
                    // sa "suspense/dramatic pause" UX ke liye.

      int diceValue = dice->roll();
      cout << "Rolled: " << diceValue << endl;

      int currentPos = currentPlayer->getPosition();

      // Rule strategy se move validate karwao — game khud decide nahi
      // karta ki valid kya hai (Strategy delegation).
      if (rules->isValidMove(currentPos, diceValue, board->getBoardSize()))
      {
        int intermediatePos = currentPos + diceValue;                           // Sirf dice-roll ke
                                                                                // baad ka RAW
                                                                                // position (snake/
                                                                                // ladder resolve
                                                                                // hone SE PEHLE).
        int newPos = rules->calculateNewPosition(currentPos, diceValue, board); // FINAL
                                                                                // position (agar
                                                                                // koi entity mili
                                                                                // to uska end).

        currentPlayer->setPosition(newPos);

        // Check karo ki player ne INTERMEDIATE cell par koi snake/ladder
        // encounter kiya kya (agar entity nahi milti to intermediatePos ==
        // newPos hi rahega, aur ye block skip ho jayega).
        BoardEntity *entity = board->getEntity(intermediatePos);
        if (entity != nullptr)
        {
          bool isSnake = (entity->name() == "SNAKE");
          if (isSnake)
          {
            cout << "Oh no! Snake at " << intermediatePos << "! Going down to " << newPos << endl;
            notify(currentPlayer->getName() + " encountered snake at " +
                   to_string(intermediatePos) + " now going down to " + to_string(newPos));
          }
          else
          {
            cout << "Great! Ladder at " << intermediatePos << "! Going up to " << newPos << endl;
            notify(currentPlayer->getName() + " encountered ladder at " +
                   to_string(intermediatePos) + " now going up to " + to_string(newPos));
          }
        }

        notify(currentPlayer->getName() + " played. New Position : " + to_string(newPos));
        displayPlayerPositions();

        // Win condition rules (Strategy) se verify karwao.
        if (rules->checkWinCondition(newPos, board->getBoardSize()))
        {
          cout << "\n"
               << currentPlayer->getName() << " wins!" << endl;
          currentPlayer->incrementScore();

          notify("Game Ended. Winner is : " + currentPlayer->getName());
          gameOver = true; // Loop se bahar.
        }
        else
        {
          // Nahi jeeta — turn khatam, round-robin: player ko queue ke end
          // me bhej do.
          players.pop_front();
          players.push_back(currentPlayer);
        }
      }
      else
      {
        // Move INVALID tha (overflow ho raha tha, exact roll nahi mila).
        // Player ki turn yahin KHATAM ho jaati hai (retry NAHI milta —
        // dice pe uska control nahi hai, isliye seedha agle player ki
        // baari aa jaati hai).
        cout << "Need exact roll to reach " << board->getBoardSize() << "!" << endl;
        // Turn indicator ko queue ke end me shift karo.
        players.pop_front();
        players.push_back(currentPlayer);
      }
    }
  }

  // Destructor: Game apne OWN kiye hue resource — sirf `rules` — delete
  // karta hai. Board aur Dice yahan delete NAHI hote kyunki Game unka
  // OWNER nahi hai (wo Factory/client ne banaye the, isliye cleanup bhi
  // wahin (main() me) hoga — jo banata hai, wahi delete karta hai).
  ~SnakeAndLadderGame()
  {
    delete rules;
  }
};

// ============================================================================
//  8. FACTORY DESIGN PATTERN  (Decoupled Game Instantiation Logic)
// ----------------------------------------------------------------------------
//  Client ko ye jaanne ki zaroorat nahi ki poora game (Board + Strategy +
//  Dice + Game object) kaise wire hota hai — wo bas apna DESIRED mode
//  (Standard / Random / Custom) bolta hai aur factory ek READY-MADE game
//  object bana ke de deti hai. Isse main() bahut CLEAN reh jaata hai.
// ============================================================================

class SnakeAndLadderGameFactory
{
public:
  // createStandardGame: Traditional classic 10x10 game banane ka helper
  // static method. Andar-hi-andar Board banata hai, StandardBoardSetupStrategy
  // se fill karwata hai, phir Dice ke saath Game bana ke deta hai.
  static SnakeAndLadderGame *createStandardGame()
  {
    Board *board = new Board(10); // Classic 10x10 size grid setup (size=100 cells).
    BoardSetupStrategy *strategy = new StandardBoardSetupStrategy();
    board->setupBoard(strategy);
    delete strategy; // Strategy object ka kaam ho gaya (sirf setup ke liye
                     // tha), isliye turant free kar diya — Board apne
                     // andar entities already store kar chuka hai.

    Dice *dice = new Dice(6); // Standard 6-faced dice generator setup.

    return new SnakeAndLadderGame(board, dice);
  }

  // createRandomGame: Random difficulty-based game layout initialization.
  static SnakeAndLadderGame *createRandomGame(int boardSize, RandomBoardSetupStrategy::Difficulty difficulty)
  {
    Board *board = new Board(boardSize);
    BoardSetupStrategy *strategy = new RandomBoardSetupStrategy(difficulty);
    board->setupBoard(strategy);
    delete strategy;

    Dice *dice = new Dice(6);

    return new SnakeAndLadderGame(board, dice);
  }

  // createCustomGame: Manual/Custom count placements ka initializer. NOTE:
  // yahan `strategy` OUTSIDE se already-configured (CustomCountBoardSetupStrategy
  // me positions add ki hui) aata hai, isliye is method me `delete strategy`
  // NAHI kiya jata — caller (main()) hi uski ownership rakhta hai aur khud
  // delete karega.
  static SnakeAndLadderGame *createCustomGame(int boardSize, BoardSetupStrategy *strategy)
  {
    Board *board = new Board(boardSize);
    board->setupBoard(strategy);

    Dice *dice = new Dice(6);

    return new SnakeAndLadderGame(board, dice);
  }
};

// ============================================================================
//  9. CLIENT (main) — Entry Point
// ----------------------------------------------------------------------------
//  Client ka kaam sirf WIRING aur USER INPUT collection hai: user se setup
//  mode poochho, Factory se game lo, observer register karo, players banao
//  aur game chala do. Board-setup algorithm, rules, notifications — kisi ki
//  bhi internal detail client ko nahi pata (Encapsulation).
// ============================================================================
int main()
{
  cout << "=== SNAKE AND LADDER GAME ===" << endl;

  SnakeAndLadderGame *game = nullptr;
  Board *board = nullptr; // NOTE: ye ek SEPARATE Board pointer hai jo sirf
                          // cleanup ke liye bana hai (Factory ke andar jo
                          // Board actual game use karta hai wo alag hai) —
                          // isse related detail neeche destructor
                          // discussion me clear hai.

  cout << "Choose game setup:" << endl;
  cout << "1. Standard Game (10x10 board with traditional positions)" << endl;
  cout << "2. Random Game with Difficulty" << endl;
  cout << "3. Custom Game" << endl;

  int choice;
  cin >> choice;

  // Setup selector conditions — user ke choice ke hisaab se Factory ka
  // sahi method call karo.
  if (choice == 1)
  {
    game = SnakeAndLadderGameFactory::createStandardGame();
    board = new Board(10);
  }
  else if (choice == 2)
  {
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
    switch (diffChoice)
    {
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
      diff = RandomBoardSetupStrategy::MEDIUM; // Safe fallback agar invalid input aaye.
    }

    game = SnakeAndLadderGameFactory::createRandomGame(boardSize, diff);
    board = new Board(boardSize);
  }
  else if (choice == 3)
  {
    int boardSize;
    cout << "Enter board size (e.g., 10 for 10x10 board): ";
    cin >> boardSize;

    cout << "Choose custom setup type:" << endl;
    cout << "1. Specify counts only (random placement)" << endl;
    cout << "2. Specify exact positions" << endl;

    int customChoice;
    cin >> customChoice;

    if (customChoice == 1)
    {
      // === Sub-choice (a): sirf COUNT do, positions random honge ===
      int numSnakes, numLadders;
      cout << "Enter number of snakes: ";
      cin >> numSnakes;
      cout << "Enter number of ladders: ";
      cin >> numLadders;

      BoardSetupStrategy *strategy = new CustomCountBoardSetupStrategy(numSnakes, numLadders, true);
      game = SnakeAndLadderGameFactory::createCustomGame(boardSize, strategy);
      delete strategy; // Yahan delete SAFE hai kyunki randomPositions=true
                       // mode me `strategy` object ke andar koi extra
                       // pre-fed manual data nahi tha jo baad me kaam
                       // aata — is call ke andar hi setup complete ho
                       // chuka hai.
    }
    else
    {
      // === Sub-choice (b): EXACT positions khud do ===
      int numSnakes, numLadders;
      cout << "Enter number of snakes: ";
      cin >> numSnakes;
      cout << "Enter number of ladders: ";
      cin >> numLadders;

      CustomCountBoardSetupStrategy *strategy = new CustomCountBoardSetupStrategy(numSnakes, numLadders, false);

      // Get custom snake positions coordinates inputs — user se ek-ek
      // karke start/end lo aur strategy object me FEED karo.
      for (int i = 0; i < numSnakes; i++)
      {
        int start, end;
        cout << "Enter snake " << (i + 1) << " start and end positions: ";
        cin >> start >> end;
        strategy->addSnakePosition(start, end);
      }

      // Get custom ladder positions coordinates inputs.
      for (int i = 0; i < numLadders; i++)
      {
        int start, end;
        cout << "Enter ladder " << (i + 1) << " start and end positions: ";
        cin >> start >> end;
        strategy->addLadderPosition(start, end);
      }

      game = SnakeAndLadderGameFactory::createCustomGame(boardSize, strategy);
      delete strategy; // Ab tak saara feed-in data `setupBoard()` ke andar
                       // consume ho chuka hai (Board ne apne entities me
                       // COPY/create kar liya hai), isliye strategy object
                       // ab safely delete ho sakta hai.
    }

    board = new Board(boardSize);
  }

  // Safe checks — agar user ne invalid choice (jaise 4, 5...) di ho to
  // `game` abhi bhi nullptr hoga, aage crash hone se bachao.
  if (game == nullptr)
  {
    cout << "Invalid choice!" << endl;
    return 1;
  }

  // Create and Add Observer Notification triggers link — ab har game event
  // console par dikhega.
  IObserver *notifier = new SnakeAndLadderConsoleNotifier();
  game->addObserver(notifier);

  // Players sizing configurations — user se kitne players chahiye aur unke
  // naam lo.
  int numPlayers;
  cout << "Enter number of players: ";
  cin >> numPlayers;

  for (int i = 0; i < numPlayers; i++)
  {
    string name;
    cout << "Enter name for player " << (i + 1) << ": ";
    cin >> name;
    SnakeAndLadderPlayer *player = new SnakeAndLadderPlayer(i + 1, name);
    game->addPlayer(player);
  }

  // Play target game triggering thread — ye call tab tak block rahegi jab
  // tak koi player jeet na jaye.
  game->play();

  // Dynamic resources cleanups:
  //  - `game` apne andar `rules` delete karega (destructor), par Board/Dice
  //    KHUD delete NAHI karega (unki ownership Factory-created object se
  //    directly game me nahi aayi thi is design me — asal Board/Dice jo
  //    game ke andar hai wo yahan explicitly delete nahi ho raha, sirf
  //    "board" naam ka LOCAL extra pointer neeche delete ho raha hai).
  //  - `board` (yahan wala local pointer) explicitly delete kiya ja raha hai.
  //  - `notifier` bhi client-owned hai, isliye yahi delete hoga.
  delete game;
  delete board;
  delete notifier;

  return 0;
}
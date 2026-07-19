// UML banate time is game me top down approach use kiya gaya hai. Sabse pehle Observer pattern, fir Strategy pattern, fir Factory pattern aur last me Client code. Isse UML diagram me har component ka relation clearly dikh raha hai.
//  similarly snake and the ladder me bhi top down use hua hai

// ============================================================================
//  TicTacToe.cpp  —  POORA Tic-Tac-Toe system EK HI FILE me (L33 - LLD)
// ----------------------------------------------------------------------------
//  Ye wahi Tic-Tac-Toe hai jo modular version (../core, ../models, ../rules,
//  ../enums waghairah) me alag-alag header files me bata hua hai — par yahan
//  sab kuch ek hi file me hai taaki upar se neeche ek hi flow me padha ja sake.
//
//  ============================================================================
//   IS FILE ME 3 DESIGN PATTERNS — ek nazar me
//  ----------------------------------------------------------------------------
//   1. OBSERVER  -> IObserver / ConsoleNotifier   : game ke events (start,
//                   move, win, draw) hone par saare registered listeners ko
//                   automatically notification milti hai
//   2. STRATEGY  -> TicTacToeRules / StandardTicTacToeRules : valid-move, win
//                   aur draw check karne ke RULES alag class me — kal ko naye
//                   rules aayein to game code chhede bina swap ho sakte hain
//   3. FACTORY   -> TicTacToeGameFactory          : GameType dekh kar sahi
//                   game object bana ke deta hai; client ko khud `new
//                   TicTacToeGame(...)` likhne ki zaroorat nahi
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ POINTER IDENTITY CONVENTION — is file ka sabse zaroori niyam          │
//  │                                                                          │
//  │  Board har cell me Symbol* (POINTER) rakhta hai, Symbol ki value nahi.   │
//  │                                                                          │
//  │  - Khali cell    :  grid[i][j] == emptyCell                              │
//  │    (saare khali cells EK HI shared '-' object ko point karte hain)       │
//  │  - Win check bhi :  getCell(i,j) == playerKaSymbol                       │
//  │    (yahan char 'X' compare NAHI hota — pointer ADDRESS compare hota hai) │
//  │                                                                          │
//  │  Ye isliye kaam karta hai kyunki har player ke paas apna ek UNIQUE       │
//  │  Symbol object hai jo poore game me wahi rehta hai. Fast hai, par dhyan  │
//  │  rahe: do alag-alag `new Symbol('X')` objects kabhi "equal" nahi         │
//  │  maane jayenge, kyunki unke addresses alag hain.                         │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#include <deque>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

// ============================================================================
//  1. OBSERVER DESIGN PATTERN  (Notification / Event Logging System)
// ----------------------------------------------------------------------------
//  Problem : Game me jab bhi kuch important hota hai (game start hua, kisi ne
//            move chala, koi jeeta, ya draw hua) to ye khabar alag-alag jagah
//            pahunchani pad sakti hai — console pe, UI pe, log file me,
//            database me...
//
//  Solution: Observer pattern. Game (Subject) ke paas observers ki ek list
//            hoti hai. Event hone par game bas `notify(msg)` bol deta hai aur
//            list ke SAARE observers ka `update()` apne aap call ho jata hai.
//
//  Fayda   : Game logic aur reporting/notification logic DECOUPLED rehte hain.
//            Kal ko FileLogger ya UINotifier add karna ho to sirf ek nayi
//            IObserver subclass banao aur register kar do — TicTacToeGame ki
//            ek line bhi nahi badalni padegi (Open/Closed Principle).
// ============================================================================

// IObserver — har observer ke liye common CONTRACT (abstract interface).
// Game sirf is interface ko jaanta hai, concrete classes ko nahi.
class IObserver
{
public:
  // update() — pure virtual function; har concrete observer ise implement
  // karega. `msg` wahi event message hai jo game loop ne bheja hai.
  virtual void update(string msg) = 0;

  // Virtual destructor zaroori hai: base class pointer (IObserver*) se delete
  // karne par derived class ka destructor sahi se chale, warna memory leak.
  virtual ~IObserver() {}
};

// ConsoleNotifier — IObserver ka ek CONCRETE implementation.
// Iska kaam simple hai: jo bhi event mile, use "[Notification]" prefix laga
// kar terminal (console) par print kar dena.
class ConsoleNotifier : public IObserver
{
public:
  void update(string msg) override
  {
    cout << "[Notification] " << msg << endl;
  }
};

// ============================================================================
//  2. SYMBOL & BOARD  (Core Data Models)
// ----------------------------------------------------------------------------
//  Ye dono "dumb" model classes hain — inme game ke RULES ka koi logic nahi.
//  Symbol sirf ek mark ('X'/'O'/'-') ko wrap karta hai, aur Board sirf grid
//  ki state sambhalta hai (kaunsa cell khali hai, mark rakho, mark batao).
//  Kaun jeeta / draw hua — ye faisla Rules (Strategy) classes ka kaam hai.
//  Isse Single Responsibility Principle follow hota hai.
// ============================================================================

// Symbol — board ke cell par lagne wale mark ('X', 'O', '-') ka wrapper.
// Seedha `char` use karne ki jagah class isliye banayi taaki future me
// symbol ke saath color, emoji, ya koi asset attach karna ho to primitive
// char badalne ki naubat na aaye.
class Symbol
{
private:
  char mark; // Actual character value (e.g. 'X', 'O', '-')

public:
  // Constructor — symbol banate waqt uska char dena zaroori hai.
  Symbol(char m)
  {
    mark = m;
  }

  // getMark() — stored character value nikalne ka getter.
  char getMark()
  {
    return mark;
  }
};

// Board — N x N grid ki state ka maalik.
// Sirf 3 kaam jaanta hai: (1) cell khali hai ya nahi batana, (2) mark place
// karna, (3) kisi cell ka mark lauta dena. Win/draw ka logic yahan NAHI hai.
class Board
{
private:
  vector<vector<Symbol *>> grid; // 2D grid — har cell ek Symbol* hold karta hai.
  int size;                      // Board ka dimension N (jaise 3 => 3x3 board).
  Symbol *emptyCell;             // SHARED '-' symbol — saare khali cells isi
                                 // ek object ko point karte hain (upar wala
                                 // pointer-identity niyam yahi se aata hai).

public:
  // Constructor — user ke diye size ka N x N grid banata hai aur har cell ko
  // emptyCell pointer se bhar deta hai (yaani "sab khali").
  Board(int s)
  {
    size = s;
    emptyCell = new Symbol('-');
    grid = vector<vector<Symbol *>>(size, vector<Symbol *>(size, emptyCell));
  }

  // isCellEmpty — kya (row, col) wala cell abhi khali hai?
  // Pehle bounds check (taaki out-of-range index par crash na ho), phir
  // pointer comparison: cell khali hai to wahan emptyCell ka hi address hoga.
  bool isCellEmpty(int row, int col)
  {
    if (row < 0 || row >= size || col < 0 || col >= size)
    {
      return false; // Board ke bahar ka coordinate — khali nahi maana jayega.
    }
    return grid[row][col] == emptyCell;
  }

  // placeMark — player ka symbol (row, col) par rakhne ki koshish karta hai.
  // Return true = mark lag gaya; false = invalid coordinate ya cell pehle se
  // bhara hua tha (overwrite hone se yahi check bachata hai).
  bool placeMark(int row, int col, Symbol *mark)
  {
    if (row < 0 || row >= size || col < 0 || col >= size)
    {
      return false; // Bounds validation.
    }
    if (!isCellEmpty(row, col))
    {
      return false; // Cell pehle se occupied — move reject.
    }
    grid[row][col] = mark;
    return true;
  }

  // getCell — (row, col) par jo bhi Symbol* rakha hai wo lauta do.
  // Out-of-range hone par crash karne ki jagah safe default (emptyCell)
  // return hota hai — isse Rules classes bina darr ke loop chala sakti hain.
  Symbol *getCell(int row, int col)
  {
    if (row < 0 || row >= size || col < 0 || col >= size)
    {
      return emptyCell;
    }
    return grid[row][col];
  }

  // getSize() — board ka dimension N.
  int getSize()
  {
    return size;
  }

  // getEmptyCell() — shared khali-cell symbol ka pointer. Rules classes ise
  // draw-check me use karti hain ("kya koi cell abhi bhi emptyCell hai?").
  Symbol *getEmptyCell()
  {
    return emptyCell;
  }

  // display — board ko console par draw karta hai:
  //
  //     0 1 2        <- column numbers (header)
  //   0 X - O        <- har row apne index ke saath
  //   1 - X -
  //   2 O - X
  //
  // Row/column numbers isliye dikhaye jaate hain taaki user ko pata rahe ki
  // input me kaunsa coordinate dena hai.
  void display()
  {
    cout << "\n  ";
    // Column headers print karo.
    for (int i = 0; i < size; i++)
    {
      cout << i << " ";
    }
    cout << endl;

    // Har row: pehle row ka index, phir us row ke saare marks.
    for (int i = 0; i < size; i++)
    {
      cout << i << " ";
      for (int j = 0; j < size; j++)
      {
        cout << grid[i][j]->getMark() << " ";
      }
      cout << endl;
    }
    cout << endl;
  }
};

// ============================================================================
//  3. PLAYER MODEL
// ----------------------------------------------------------------------------
//  Player ki basic details ka simple data holder — id, naam, uska symbol,
//  aur ab tak ke wins ka score. Multi-round game me score kaam aayega.
//
//  ⭐ STATIC ID GENERATION — is class me naya addition
//  ----------------------------------------------------------------------------
//  Pehle playerId ko main() se manually pass karna padta tha
//  (jaise `new TicTacToePlayer(1, "Aditya", ...)`), jisme galti se duplicate
//  id de dene ka risk tha.
//
//  Ab TicTacToePlayer ke andar ek `static int nextId` counter hai. Static
//  member CLASS ke saath bandha hota hai, kisi ek OBJECT ke saath nahi —
//  isliye saare TicTacToePlayer objects (chahe kitne bhi bane) is EK HI
//  shared counter ko access karte hain. Har naya player banate waqt
//  constructor khud `nextId++` karke apni id le leta hai, isliye:
//    - Client (main.cpp) ko ab id pass karne ki zaroorat nahi.
//    - Id kabhi duplicate/clash nahi hogi, chahe 2 players banao ya 200.
// ============================================================================

class TicTacToePlayer
{
private:
  static int nextId; // <-- NAYA: class-level (static) counter — instance
                     //     member nahi hai, isliye har object ki alag copy
                     //     nahi banti; saare objects isi EK counter ko
                     //     share karte hain. Isko class ke BAHAR (niche,
                     //     file me ek baar) define/initialize karna hoga,
                     //     warna linker error ("undefined reference")
                     //     aayega.

  int playerId;   // Player ki unique ID — ab ye automatically nextId se milti hai.
  string name;    // Display ke liye player ka naam.
  Symbol *symbol; // Is player ka apna UNIQUE symbol object ('X' ya 'O') —
                  // win-check isi pointer se hota hai, isliye ye poore game
                  // me same object rehna chahiye.
  int score;      // Ab tak kitne match jeete (wins counter).

public:
  // Constructor — ab playerId PARAMETER me nahi leta, khud generate karta
  // hai. `nextId++` ek POST-increment hai: pehle current value playerId me
  // assign hoti hai, USKE BAAD counter ek badhta hai. Isliye pehla player
  // id=1 lega (agar nextId 1 se start hua ho), doosra id=2, waghairah —
  // bilkul database ke "auto-increment primary key" jaisa.
  TicTacToePlayer(string n, Symbol *s)
  {
    playerId = nextId++; // auto-generate unique id, phir counter aage badhao
    name = n;
    symbol = s;
    score = 0;
  }

  // getPlayerId() — is player ki auto-generated unique id.
  int getPlayerId()
  {
    return playerId;
  }

  // getName() — player ka naam.
  string getName()
  {
    return name;
  }

  // getSymbol() — player ke symbol ka pointer (board par yahi place hota hai).
  Symbol *getSymbol()
  {
    return symbol;
  }

  // getScore() — ab tak ke total wins.
  int getScore()
  {
    return score;
  }

  // incrementScore() — game jeet ne par score +1.
  void incrementScore()
  {
    score++;
  }

  // Destructor — player ka symbol player ne hi "own" kiya hai (main me
  // `new Symbol('X')` pass hota hai), isliye cleanup bhi yahi karega.
  ~TicTacToePlayer()
  {
    delete symbol;
  }
};

// Static member ki definition + initialization — class ke BAHAR, file-scope
// par ek hi baar likhni padti hai (sirf class ke andar declare karna kaafi
// nahi hota, warna compiler/linker "undefined reference to nextId" bolega).
// Yahan 1 se start kiya hai, taaki pehla banaya gaya player id=1 pae.
int TicTacToePlayer::nextId = 1;

// ============================================================================
//  4. STRATEGY DESIGN PATTERN  (Game Rules ko Swap Karne ka System)
// ----------------------------------------------------------------------------
//  Problem : "Move valid hai?", "Koi jeeta?", "Draw hua?" — ye teeno checks
//            game ke RULES hain. Agar ye logic seedha TicTacToeGame ke andar
//            likh dete, to naye rules (jaise "Anti Tic-Tac-Toe" — line banane
//            wala HAARTA hai, ya koi wildcard mode) laane ke liye game class
//            kholni padti.
//
//  Solution: Strategy pattern. Rules ka ek abstract interface (TicTacToeRules)
//            banaya, aur classic wale rules uski ek concrete subclass
//            (StandardTicTacToeRules) me daal diye. Game sirf interface ko
//            jaanta hai — naye rules chahiye to bas nayi subclass inject karo.
// ============================================================================

// TicTacToeRules — rules ka abstract STRATEGY interface.
// Game isi ke through saare rule-checks karwata hai.
class TicTacToeRules
{
public:
  // isValidMove — kya (row, col) par move chalna allowed hai?
  virtual bool isValidMove(Board *board, int row, int col) = 0;

  // checkWinCondition — kya diye gaye symbol wala player jeet gaya hai?
  virtual bool checkWinCondition(Board *board, Symbol *symbol) = 0;

  // checkDrawCondition — kya game draw ho gaya hai (board full, koi winner nahi)?
  virtual bool checkDrawCondition(Board *board) = 0;

  virtual ~TicTacToeRules() {}
};

// StandardTicTacToeRules — classic N x N Tic-Tac-Toe ke rules (CONCRETE
// strategy): poori row YA poori column YA koi diagonal same symbol ki ho
// jaye to win; board bhar jaye bina win ke to draw.
class StandardTicTacToeRules : public TicTacToeRules
{
public:
  // isValidMove — standard rules me move valid hai agar target cell khali hai
  // (bounds ka check Board::isCellEmpty ke andar already ho jata hai).
  bool isValidMove(Board *board, int row, int col) override
  {
    return board->isCellEmpty(row, col);
  }

  // checkWinCondition — 4 tarah ki lines scan karta hai. Har comparison
  // pointer-identity se hota hai (getCell == symbol), char se nahi.
  // Complexity: O(N^2) — N rows * N cells + N cols * N cells + 2 diagonals.
  bool checkWinCondition(Board *board, Symbol *symbol) override
  {
    int size = board->getSize();

    // 1. HORIZONTAL — har row check karo: kya poori row isi symbol ki hai?
    for (int i = 0; i < size; i++)
    {
      bool win = true;
      for (int j = 0; j < size; j++)
      {
        if (board->getCell(i, j) != symbol)
        {
          win = false; // Ek bhi cell mismatch => ye row to nahi bani.
          break;
        }
      }
      if (win)
        return true; // Poori row match — WIN!
    }

    // 2. VERTICAL — har column ke liye wahi check.
    for (int j = 0; j < size; j++)
    {
      bool win = true;
      for (int i = 0; i < size; i++)
      {
        if (board->getCell(i, j) != symbol)
        {
          win = false; // Mismatch mila, ye column chhodo.
          break;
        }
      }
      if (win)
        return true; // Poora column match — WIN!
    }

    // 3. MAIN DIAGONAL — top-left se bottom-right, yaani cells [i][i].
    bool win = true;
    for (int i = 0; i < size; i++)
    {
      if (board->getCell(i, i) != symbol)
      {
        win = false;
        break;
      }
    }
    if (win)
      return true;

    // 4. ANTI-DIAGONAL — top-right se bottom-left, yaani [i][size-1-i].
    win = true;
    for (int i = 0; i < size; i++)
    {
      if (board->getCell(i, size - 1 - i) != symbol)
      {
        win = false;
        break;
      }
    }
    return win;
  }

  // checkDrawCondition — poore board me ek bhi khali cell na bache to draw.
  // NOTE: Game loop me ye check hamesha checkWinCondition ke BAAD hota hai,
  // isliye yahan winner ka check dobara karne ki zaroorat nahi.
  bool checkDrawCondition(Board *board) override
  {
    int size = board->getSize();
    for (int i = 0; i < size; i++)
    {
      for (int j = 0; j < size; j++)
      {
        if (board->getCell(i, j) == board->getEmptyCell())
        {
          return false; // Khali cell mila — abhi game chal sakta hai.
        }
      }
    }
    return true; // Board full aur (loop ke order ki wajah se) winner nahi — DRAW.
  }
};

// ============================================================================
//  5. GAME CONTROLLER  (Observer pattern ka SUBJECT + Strategy ka CONTEXT)
// ----------------------------------------------------------------------------
//  TicTacToeGame poore game ka ORCHESTRATOR hai:
//   - Board banata hai, players ki turn-order sambhalta hai (deque se
//     round-robin), rules se checks karwata hai, observers ko notify karta hai.
//   - Observer pattern me ye SUBJECT hai (observers ki list + notify()).
//   - Strategy pattern me ye CONTEXT hai (rules* ke through kaam karta hai,
//     concrete rules class ko nahi jaanta).
// ============================================================================

class TicTacToeGame
{
private:
  Board *board;                     // Game ka board (grid state ka maalik).
  deque<TicTacToePlayer *> players; // Turn order ke liye deque: front wala
                                    // player khelta hai, phir pop karke back
                                    // me chala jata hai => ROUND ROBIN. Deque
                                    // isliye kyunki dono ends se O(1)
                                    // push/pop milta hai aur 2 se zyada
                                    // players bhi aaram se fit ho jate hain.
  TicTacToeRules *rules;            // Active rules strategy (Strategy Context).
  vector<IObserver *> observers;    // Registered observers ki list (Subject).
  bool gameOver;                    // Game loop ka exit flag.

public:
  // Constructor — diye gaye size ka board aur default (standard) rules set
  // karta hai. Naye rules support karne ho to yahan injection point banega.
  TicTacToeGame(int boardSize)
  {
    board = new Board(boardSize);
    rules = new StandardTicTacToeRules();
    gameOver = false;
  }

  // addPlayer — player ko turn-queue ke end me jodo.
  // Jis order me add kiya, usi order me turns milengi.
  void addPlayer(TicTacToePlayer *player)
  {
    players.push_back(player);
  }

  // addObserver — notification listener register karo (Observer pattern).
  void addObserver(IObserver *observer)
  {
    observers.push_back(observer);
  }

  // notify — SAARE registered observers ko ek hi message broadcast karo.
  // Game ko farak nahi padta ki aage console hai, file hai ya UI.
  void notify(string msg)
  {
    for (auto observer : observers)
    {
      observer->update(msg);
    }
  }

  // play — poore game ka MAIN LOOP. Har iteration = ek player ki turn:
  //
  //   board dikhao -> front player se input lo -> move validate karo
  //     |- valid   : mark place karo, notify karo, phir WIN -> DRAW -> warna
  //     |            player ko queue ke end me bhej do (round robin)
  //     |- invalid : warning dikha kar USI player ko dobara chance do
  //                  (player queue me aage-piche nahi hota)
  void play()
  {
    // Kam se kam 2 players chahiye, warna game shuru hi nahi hoga.
    if (players.size() < 2)
    {
      cout << "Need at least 2 players!" << endl;
      return;
    }

    notify("Tic Tac Toe Game Started!"); // Observers ko game-start event.

    while (!gameOver)
    {
      board->display(); // Har turn se pehle current board dikhao.

      // ROUND ROBIN: deque ke front wale player ki turn hai.
      TicTacToePlayer *currentPlayer = players.front();
      cout << currentPlayer->getName() << " ("
           << currentPlayer->getSymbol()->getMark()
           << ") - Enter row and column: ";

      int row, col;
      cin >> row >> col; // User se coordinates lo (e.g. "1 2").

      // Move validation rules (Strategy) se karwao — game khud decide nahi
      // karta ki valid kya hai.
      if (rules->isValidMove(board, row, col))
      {
        board->placeMark(row, col, currentPlayer->getSymbol());
        notify(currentPlayer->getName() + " played (" + to_string(row) + "," +
               to_string(col) + ")");

        // Pehle WIN check (isi player ke symbol ke liye — kyunki naya move
        // sirf isi ki line poori kar sakta hai):
        if (rules->checkWinCondition(board, currentPlayer->getSymbol()))
        {
          board->display();
          cout << currentPlayer->getName() << " wins!" << endl;
          currentPlayer->incrementScore();
          notify(currentPlayer->getName() + " wins!");
          gameOver = true; // Loop se bahar.
        }
        // Win nahi hua to DRAW check (board full ho gaya kya?):
        else if (rules->checkDrawCondition(board))
        {
          board->display();
          cout << "It's a draw!" << endl;
          notify("Game is Draw!");
          gameOver = true;
        }
        // Na win na draw — turn khatam, player ko queue ke end me bhejo.
        else
        {
          players.pop_front();              // Front se hatao...
          players.push_back(currentPlayer); // ...aur end me laga do.
        }
      }
      else
      {
        // Invalid move (out of range ya cell bhara hua) — player wahi rehta
        // hai, agli iteration me use hi dobara input dena hoga.
        cout << "Invalid move! Try again." << endl;
      }
    }
  }

  // Destructor — game ke OWN kiye hue resources (board, rules) delete karo.
  // Players aur observers yahan delete NAHI hote, kyunki wo bahar (main me)
  // banaye gaye the — jo banata hai, wahi delete karta hai (ownership rule).
  ~TicTacToeGame()
  {
    delete board;
    delete rules;
  }
};

// ============================================================================
//  6. FACTORY DESIGN PATTERN  (Game Instance Creation)
// ----------------------------------------------------------------------------
//  Client ko ye jaanne ki zaroorat nahi ki game object kaise banta hai —
//  wo bas GameType enum bhejta hai aur factory ready-made game lauta deti
//  hai. Kal ko naye modes aaye (jaise BLITZ, ANTI) to bas enum me value aur
//  factory me ek branch add karni hai; client code waisa ka waisa rahega.
// ============================================================================

// GameType — kaunse mode ka game chahiye, uska "type tag".
// Abhi sirf STANDARD hai; future modes ke liye extension point.
enum GameType
{
  STANDARD
};

// TicTacToeGameFactory — SIMPLE FACTORY: static method jo type dekh kar
// sahi game object create karke return karta hai. Object-construction ki
// details client se chhup jaati hain.
class TicTacToeGameFactory
{
public:
  // createGame — GameType ke hisaab se game banao; unknown type par nullptr.
  static TicTacToeGame *createGame(GameType gt, int boardSize)
  {
    if (GameType::STANDARD == gt)
    {
      return new TicTacToeGame(boardSize);
    }
    return nullptr;
  }
};

// ============================================================================
//  7. CLIENT (main) — Entry Point
// ----------------------------------------------------------------------------
//  Client ka kaam sirf WIRING hai: board size poocho, factory se game lo,
//  observer register karo, players banao aur game chala do. Rules, win-check,
//  notifications — kisi ki bhi internal detail client ko nahi pata.
// ============================================================================
int main()
{
  cout << "=== TIC TAC TOE GAME ===" << endl;

  // User se board ka size lo (3 => classic 3x3; 4, 5... bhi chalega).
  int boardSize;
  cout << "Enter board size (e.g., 3 for 3x3): ";
  cin >> boardSize;

  // FACTORY se game banwao — client `new TicTacToeGame` direct nahi likhta.
  TicTacToeGame *game =
      TicTacToeGameFactory::createGame(GameType::STANDARD, boardSize);

  // OBSERVER register karo — ab har game event console par dikhega.
  IObserver *notifier = new ConsoleNotifier();
  game->addObserver(notifier);

  // Players banao — ab playerId hardcode nahi karni, TicTacToePlayer ka
  // static `nextId` counter khud-ba-khud unique id assign kar dega
  // (Aditya ko id=1, Harshita ko id=2, waghairah — bina hume kuch pass kiye).
  // Har player ko apna UNIQUE Symbol object milta hai (yaad rahe: win-check
  // isi pointer ki identity se hota hai).
  TicTacToePlayer *player1 = new TicTacToePlayer("Aditya", new Symbol('X'));
  TicTacToePlayer *player2 = new TicTacToePlayer("Harshita", new Symbol('O'));

  // Players ko game ki turn-queue me daalo (Aditya pehle khelega).
  game->addPlayer(player1);
  game->addPlayer(player2);

  // Game START — ye call tab tak block rahegi jab tak win/draw na ho jaye.
  game->play();

  // Cleanup — jo jisne banaya, wahi delete karega:
  //  - game apne andar board + rules delete karega (destructor)
  //  - player apne andar apna symbol delete karega (destructor)
  delete game;
  delete player1;
  delete player2;
  delete notifier;

  return 0;
}
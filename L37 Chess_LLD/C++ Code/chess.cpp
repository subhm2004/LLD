// ============================================================================
//  chess.cpp  —  ONLINE CHESS SYSTEM (single-file MONOLITH version)
// ----------------------------------------------------------------------------
//  Ye poora chess system EK file me hai — lecture ka original code. Isi ko
//  refactor karke MODULAR version banaya gaya hai (../core, ../pieces,
//  ../models, ../rules, ../factories, ../strategies folders + ../main.cpp).
//  Detailed folder-wise comments wahan padho; ye file "sab ek jagah" reference
//  ke liye hai.
//
//  IS FILE KA LAYOUT (upar se neeche):
//    1. Enums (Color/PieceType/GameStatus) + Position model
//    2. Message + Mediator interfaces + User    <- MEDIATOR pattern
//    3. Piece hierarchy + move generation       <- polymorphism
//    4. PieceFactory                            <- FACTORY pattern
//    5. Board (8x8 + piece map)
//    6. ChessRules (validate/check/mate)        <- STRATEGY pattern
//    7. Match (gameplay + chat mediator)
//    8. MatchingStrategy + GameManager          <- STRATEGY + SINGLETON
//    9. Demo (Scholar's Mate) + main()
//
//  Patterns: Singleton + Strategy + Mediator + Factory — wahi 4 jo modular
//  version me hain. Dono versions ka behavior same hai.
// ============================================================================
#include <algorithm>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

// ============================================================================
// 1. BASIC ENUMS & COORDINATE TYPE DEFINITIONS (Type Safety Systems)
// ============================================================================

// Color: Chess pieces aur player turns ko identify karne ke liye color enum.
enum Color { WHITE, BLACK };

// PieceType: Board par use hone wale standard piece variants identifiers.
enum PieceType { KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };

// GameStatus: Dynamic Match status trackers enum.
enum GameStatus { WAITING, IN_PROGRESS, COMPLETED, ABORTED };

// Forward declarations taaki classes reference compilation order strict requirements control karein.
class Board;
class Piece;
class Match;
class User;

// Position: Board coordinate tracking helper model (Row index 0-7, Col index 0-7).
class Position {
private:
  int row; // Board row dimension (0 to 7 representation)
  int col; // Board column dimension (0 to 7 representation)

public:
  // Default Constructor: Row aur Col defaults to 0.
  Position() {
    row = 0;
    col = 0;
  }

  // Parameterized Constructor: Inputs custom dimensions.
  Position(int r, int c) {
    row = r;
    col = c;
  }

  // Getters: Coordinate indices return accessors.
  int getRow() const { return row; }
  int getCol() const { return col; }

  // isValid: Checks index ranges grid boundaries. 
  // (Yeh check karta hai ki row/col index 0 se 7 ke boundary zone me hi hain).
  bool isValid() const { 
    return row >= 0 && row < 8 && col >= 0 && col < 8; 
  }

  // Operator overloading comparison rules matching coordinates equivalence.
  bool operator==(const Position &other) const {
    return row == other.row && col == other.col;
  }

  // Map collections ordering sorting index mapping.
  bool operator<(const Position &other) const {
    if (row != other.row)
      return row < other.row;
    return col < other.col;
  }

  // toString(): Coordinates mapping formatting print strings.
  string toString() const {
    return "(" + to_string(row) + "," + to_string(col) + ")";
  }

  // toChessNotation: standard notations (e.g. e4, f7, a8) calculations conversions.
  string toChessNotation() const {
    char file = 'a' + col;     // a-h standard columns translations.
    char rank = '8' - row;     // 8-1 standard row index configurations.
    return string(1, file) + string(1, rank);
  }
};

// Move: Single turn parameters details tracker (Tracks source coordinate, target coordinate, piece details).
class Move {
private:
  Position from;        // Source square coordinates pointer.
  Position to;          // Target square coordinates pointer.
  Piece *piece;         // Jiss piece ko move kiya ja raha hai.
  Piece *capturedPiece; // Agar targeted box par opponent piece thha aur capture hua, to uska reference.

public:
  Move() {
    piece = nullptr;
    capturedPiece = nullptr;
  }

  Move(Position f, Position t, Piece *p, Piece *captured) {
    from = f;
    to = t;
    piece = p;
    capturedPiece = captured;
  }

  // Move properties getter accessor functions
  Position getFrom() const { return from; }
  Position getTo() const { return to; }
  Piece *getPiece() const { return piece; }
  Piece *getCapturedPiece() const { return capturedPiece; }
};

// ============================================================================
// 2. STRATEGY DESIGN PATTERN (Piece movement path checking)
// ============================================================================
// Piece is a base interface abstract. Har different piece type (Rook, Bishop, etc.) 
// path generation algorithms different rule classes ke under override hota hai.
// Yeh dynamic runtime move extraction setup rules execute karta hai (Strategy pattern interface).

class Piece {
protected:
  Color color;    // White or Black piece color.
  PieceType type; // King, Queen, Bishop, etc. type tags.
  bool hasMoved;  // Specially Pawn, Rook, and King status check (used in double move, castling).

public:
  Piece(Color c, PieceType t) {
    color = c;
    type = t;
    hasMoved = false;
  }

  virtual ~Piece() {}

  // Basic getters
  Color getColor() const { return color; }
  PieceType getType() const { return type; }
  bool getHasMoved() const { return hasMoved; }
  void setMoved(bool moved) { hasMoved = moved; }

  // getPossibleMoves: Base method jo har specific chess piece type ke hisab se override hoga.
  // currentPos: Piece ka current square.
  // board: Pure game-board representation taaki checks calculations easy rahein.
  virtual vector<Position> getPossibleMoves(Position currentPos, Board *board) = 0;
  
  // getSymbol: Game console outputs representation printing tag keys.
  virtual string getSymbol() = 0;

  // toString: Helper to return color prefixed short names (e.g. WK = White King, BP = Black Pawn).
  string toString() {
    string colorStr = (color == WHITE) ? "W" : "B";
    return colorStr + getSymbol();
  }
};

// ============================================================================
// 3. CONCRETE PIECES STRATEGIES (Move Calculations Modules)
// ============================================================================

// King: Base validation loops coordinate steps (moves exactly 1 coordinate block).
class King : public Piece {
public:
  King(Color color) : Piece(color, KING) {}

  vector<Position> getPossibleMoves(Position currentPos, Board *board) override;
  string getSymbol() override { return "K"; }
};

// Queen: Horizontal + Vertical + Diagonal directional loops coordinate scanner checks.
class Queen : public Piece {
public:
  Queen(Color color) : Piece(color, QUEEN) {}

  vector<Position> getPossibleMoves(Position currentPos, Board *board) override;
  string getSymbol() override { return "Q"; }
};

// Rook: Straight vertical and horizontal movement pathways scan checkers.
class Rook : public Piece {
public:
  Rook(Color color) : Piece(color, ROOK) {}

  vector<Position> getPossibleMoves(Position currentPos, Board *board) override;
  string getSymbol() override { return "R"; }
};

// Bishop: Diagonal line checks scanning.
class Bishop : public Piece {
public:
  Bishop(Color color) : Piece(color, BISHOP) {}

  vector<Position> getPossibleMoves(Position currentPos, Board *board) override;
  string getSymbol() override { return "B"; }
};

// Knight: L-shaped moves validator coordinates checks (2 steps in 1 dir, 1 step in perp dir).
class Knight : public Piece {
public:
  Knight(Color color) : Piece(color, KNIGHT) {}

  vector<Position> getPossibleMoves(Position currentPos, Board *board) override;
  string getSymbol() override { return "N"; }
};

// Pawn: Special double steps on startup, diagonal captures validations.
class Pawn : public Piece {
public:
  Pawn(Color color) : Piece(color, PAWN) {}

  vector<Position> getPossibleMoves(Position currentPos, Board *board) override;
  string getSymbol() override { return "P"; }
};

// ============================================================================
// 4. FACTORY DESIGN PATTERN (Piece Instantiation Services)
// ============================================================================
// Client code or Board initializer ko specific pieces ke constructor (e.g. `new Pawn()`) 
// ko call karne ki zaroorat nahi padti. Pieces dynamically Factory se request hote hain.

class PieceFactory {
public:
  // createPiece: Custom parameters evaluation based constructor dispatcher.
  static Piece *createPiece(PieceType type, Color color) {
    switch (type) {
    case KING:   return new King(color);
    case QUEEN:  return new Queen(color);
    case ROOK:   return new Rook(color);
    case BISHOP: return new Bishop(color);
    case KNIGHT: return new Knight(color);
    case PAWN:   return new Pawn(color);
    default:     return nullptr;
    }
  }
};

// ============================================================================
// 5. BOARD MANAGEMENT CLASS (Dumb Object coordinates mapper)
// ============================================================================

// Board: Coordinates grid management operations layout mapping.
class Board {
private:
  Piece *board[8][8];                  // Actual 8x8 pieces storage grid database.
  map<Position, Piece *> piecePositions; // Coordinates lists optimizations checking maps lookup.

public:
  // Constructor: Board cleanup grid memory setups initialization.
  Board() {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        board[i][j] = nullptr; // Clear grid.
      }
    }
    initializeBoard(); // Default set pieces setup.
  }

  // Destructor: Dynamic active piece allocations memory trace removal.
  ~Board() {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        if (board[i][j] != nullptr) {
          delete board[i][j];
          board[i][j] = nullptr;
        }
      }
    }
    piecePositions.clear();
  }

  // initializeBoard: Classic standard startup layout parameters deployment.
  void initializeBoard() {
    // 1. Setup White main row (row index 7) and pawns row (row index 6).
    placePiece(Position(7, 0), PieceFactory::createPiece(ROOK, WHITE));
    placePiece(Position(7, 1), PieceFactory::createPiece(KNIGHT, WHITE));
    placePiece(Position(7, 2), PieceFactory::createPiece(BISHOP, WHITE));
    placePiece(Position(7, 3), PieceFactory::createPiece(QUEEN, WHITE));
    placePiece(Position(7, 4), PieceFactory::createPiece(KING, WHITE));
    placePiece(Position(7, 5), PieceFactory::createPiece(BISHOP, WHITE));
    placePiece(Position(7, 6), PieceFactory::createPiece(KNIGHT, WHITE));
    placePiece(Position(7, 7), PieceFactory::createPiece(ROOK, WHITE));

    for (int i = 0; i < 8; i++) {
      placePiece(Position(6, i), PieceFactory::createPiece(PAWN, WHITE));
    }

    // 2. Setup Black main row (row index 0) and pawns row (row index 1).
    placePiece(Position(0, 0), PieceFactory::createPiece(ROOK, BLACK));
    placePiece(Position(0, 1), PieceFactory::createPiece(KNIGHT, BLACK));
    placePiece(Position(0, 2), PieceFactory::createPiece(BISHOP, BLACK));
    placePiece(Position(0, 3), PieceFactory::createPiece(QUEEN, BLACK));
    placePiece(Position(0, 4), PieceFactory::createPiece(KING, BLACK));
    placePiece(Position(0, 5), PieceFactory::createPiece(BISHOP, BLACK));
    placePiece(Position(0, 6), PieceFactory::createPiece(KNIGHT, BLACK));
    placePiece(Position(0, 7), PieceFactory::createPiece(ROOK, BLACK));

    for (int i = 0; i < 8; i++) {
      placePiece(Position(1, i), PieceFactory::createPiece(PAWN, BLACK));
    }
  }

  // placePiece: Sets active piece objects on specific coordinates.
  void placePiece(Position pos, Piece *piece) {
    board[pos.getRow()][pos.getCol()] = piece;
    piecePositions[pos] = piece;
  }

  // removePiece: Clears specific coordinate cell variables lookup.
  void removePiece(Position pos) {
    board[pos.getRow()][pos.getCol()] = nullptr;
    piecePositions.erase(pos);
  }

  // getPiece: Retrieve piece configurations pointer from target cell coordinates.
  Piece *getPiece(Position pos) { 
    return board[pos.getRow()][pos.getCol()]; 
  }

  // isOccupied: checks coordinate status.
  bool isOccupied(Position pos) { 
    return getPiece(pos) != nullptr; 
  }

  // isOccupiedBySameColor: check targeted cell. Same color block exit.
  bool isOccupiedBySameColor(Position pos, Color color) {
    Piece *piece = getPiece(pos);
    return piece != nullptr && piece->getColor() == color;
  }

  // movePiece: Core board values modifications parameters calculations updates.
  void movePiece(Position from, Position to) {
    Piece *piece = getPiece(from);
    if (piece != nullptr) {
      // 1. Capture handling: target cell coordinate memory deletion.
      Piece *capturedPiece = getPiece(to);
      if (capturedPiece != nullptr) {
        delete capturedPiece; // Delete captured object to free memory.
        piecePositions.erase(to);
      }

      // 2. Relocate active piece source to target layout references.
      board[from.getRow()][from.getCol()] = nullptr;
      board[to.getRow()][to.getCol()] = piece;

      // 3. Update lookup coordinates references tracking maps.
      piecePositions.erase(from);
      piecePositions[to] = piece;

      piece->setMoved(true); // Status flag update for casting/pawns conditions checking.
    }
  }

  // findKing: Lookups active king position matching color indicators.
  Position findKing(Color color) {
    for (auto &pair : piecePositions) {
      if (pair.second->getType() == KING && pair.second->getColor() == color) {
        return pair.first;
      }
    }
    return Position(-1, -1);
  }

  // getAllPiecesOfColor: Returns all available pieces list matching parameter query color.
  vector<Position> getAllPiecesOfColor(Color color) {
    vector<Position> pieces;
    for (auto &pair : piecePositions) {
      if (pair.second->getColor() == color) {
        pieces.push_back(pair.first);
      }
    }
    return pieces;
  }

  // display: Beautiful ASCII grid formatting output console layout system.
  void display() {
    constexpr int cellW = 3; // Cell width settings representation.

    auto printBorder = [&]() {
      cout << "  +";
      for (int i = 0; i < 8; ++i)
        cout << string(cellW, '-') << "+";
      cout << "\n";
    };

    printBorder();

    // Top headers labeling coordinate names printing.
    cout << "  |";
    for (char f = 'a'; f <= 'h'; ++f) {
      int pad = (cellW - 1) / 2;
      cout << string(pad, ' ') << f << string(cellW - 1 - pad, ' ') << "|";
    }
    cout << "\n";

    printBorder();

    // Render ranks from 8 down to 1.
    for (int rank = 8; rank >= 1; --rank) {
      int row = 8 - rank;
      cout << rank << " |";

      for (int file = 0; file < 8; ++file) {
        Piece *p = board[row][file];
        string s = p ? p->toString() : "  "; // Empty block gets empty space print.

        int pad = (cellW - 2) / 2;
        cout << string(pad, ' ') << s << string(cellW - 2 - pad, ' ') << "|";
      }

      cout << " " << rank << "\n";
      printBorder();
    }

    // Bottom headers label formatting details print.
    cout << "  |";
    for (char f = 'a'; f <= 'h'; ++f) {
      int pad = (cellW - 1) / 2;
      cout << string(pad, ' ') << f << string(cellW - 1 - pad, ' ') << "|";
    }
    cout << "\n";

    printBorder();
  }
};

// ============================================================================
// 6. DETAILED PATH MOVEMENT RESOLUTIONS (Strategies implementations)
// ============================================================================

// King Possible Moves: Scan 8 immediate neighboring cells (1 block depth limit checks).
vector<Position> King::getPossibleMoves(Position currentPos, Board *board) {
  vector<Position> moves;
  int directions[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                          {0, 1},   {1, -1}, {1, 0},  {1, 1}};

  for (int i = 0; i < 8; i++) {
    Position newPos(currentPos.getRow() + directions[i][0],
                    currentPos.getCol() + directions[i][1]);
    if (newPos.isValid() && !board->isOccupiedBySameColor(newPos, this->color)) {
      moves.push_back(newPos);
    }
  }
  return moves;
}

// Queen Possible Moves: Straight line directions (8 ways horizontal vertical diagonals).
vector<Position> Queen::getPossibleMoves(Position currentPos, Board *board) {
  vector<Position> moves;
  int directions[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                          {0, 1},   {1, -1}, {1, 0},  {1, 1}};

  for (int d = 0; d < 8; d++) {
    for (int i = 1; i < 8; i++) {
      Position newPos(currentPos.getRow() + directions[d][0] * i,
                      currentPos.getCol() + directions[d][1] * i);
      if (!newPos.isValid()) break;

      // Obstacle check: same color halts scans.
      if (board->isOccupiedBySameColor(newPos, this->color)) break;

      moves.push_back(newPos);
      
      // Stop scanning if opponent is captured (cannot jump over pieces).
      if (board->isOccupied(newPos)) break; 
    }
  }
  return moves;
}

// Rook Possible Moves: Straight 4 directions (horizontal & vertical).
vector<Position> Rook::getPossibleMoves(Position currentPos, Board *board) {
  vector<Position> moves;
  int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

  for (int d = 0; d < 4; d++) {
    for (int i = 1; i < 8; i++) {
      Position newPos(currentPos.getRow() + directions[d][0] * i,
                      currentPos.getCol() + directions[d][1] * i);
      if (!newPos.isValid()) break;

      if (board->isOccupiedBySameColor(newPos, this->color)) break;

      moves.push_back(newPos);
      if (board->isOccupied(newPos)) break;
    }
  }
  return moves;
}

// Bishop Possible Moves: Diagonal 4 directions.
vector<Position> Bishop::getPossibleMoves(Position currentPos, Board *board) {
  vector<Position> moves;
  int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

  for (int d = 0; d < 4; d++) {
    for (int i = 1; i < 8; i++) {
      Position newPos(currentPos.getRow() + directions[d][0] * i,
                      currentPos.getCol() + directions[d][1] * i);
      if (!newPos.isValid()) break;
      if (board->isOccupiedBySameColor(newPos, this->color)) break;
      moves.push_back(newPos);
      if (board->isOccupied(newPos)) break;
    }
  }
  return moves;
}

// Knight Possible Moves: Horse jumps jumps. Jumps over other obstacles checks.
vector<Position> Knight::getPossibleMoves(Position currentPos, Board *board) {
  vector<Position> moves;
  int knightMoves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
                           {1, -2},  {1, 2},  {2, -1},  {2, 1}};

  for (int i = 0; i < 8; i++) {
    Position newPos(currentPos.getRow() + knightMoves[i][0],
                    currentPos.getCol() + knightMoves[i][1]);
    if (newPos.isValid() && !board->isOccupiedBySameColor(newPos, this->color)) {
      moves.push_back(newPos);
    }
  }
  return moves;
}

// Pawn Possible Moves: Step forward calculations, initial double steps, and diagonal capture checks.
vector<Position> Pawn::getPossibleMoves(Position currentPos, Board *board) {
  vector<Position> moves;
  int direction = (color == WHITE) ? -1 : 1; // White pawns move up (-1 row), Black move down (+1 row).

  // 1. One step forward move check.
  Position oneStep(currentPos.getRow() + direction, currentPos.getCol());
  if (oneStep.isValid() && !board->isOccupied(oneStep)) {
    moves.push_back(oneStep);

    // 2. Double step forward move from baseline starting index coordinates.
    if (!hasMoved) {
      Position twoStep(currentPos.getRow() + 2 * direction, currentPos.getCol());
      if (twoStep.isValid() && !board->isOccupied(twoStep)) {
        moves.push_back(twoStep);
      }
    }
  }

  // 3. Diagonal capture evaluation checks (only legal if enemy occupies cell).
  Position leftCapture(currentPos.getRow() + direction, currentPos.getCol() - 1);
  Position rightCapture(currentPos.getRow() + direction, currentPos.getCol() + 1);

  if (leftCapture.isValid() && board->isOccupied(leftCapture) &&
      !board->isOccupiedBySameColor(leftCapture, this->color)) {
    moves.push_back(leftCapture);
  }

  if (rightCapture.isValid() && board->isOccupied(rightCapture) &&
      !board->isOccupiedBySameColor(rightCapture, this->color)) {
    moves.push_back(rightCapture);
  }

  return moves;
}

// ============================================================================
// 7. CHESS RULES VALIDATOR STRATEGIES (Standard Chess Rules Implementation)
// ============================================================================

// ChessRules: Strategy interface to check game limits, checkmate, and checks validations.
class ChessRules {
public:
  virtual ~ChessRules() {}
  virtual bool isValidMove(Move move, Board *board) = 0;
  virtual bool isInCheck(Color color, Board *board) = 0;
  virtual bool isCheckmate(Color color, Board *board) = 0;
  virtual bool isStalemate(Color color, Board *board) = 0;
  virtual bool wouldMoveCauseCheck(Move move, Board *board, Color kingColor) = 0;
};

// StandardChessRules: Classic FIDE Chess Rules checks representation algorithms.
class StandardChessRules : public ChessRules {
public:
  // isValidMove: Checks path validity coordinates checks.
  bool isValidMove(Move move, Board *board) override {
    Piece *piece = move.getPiece();
    vector<Position> possibleMoves = piece->getPossibleMoves(move.getFrom(), board);

    // Verify target matches derived options list.
    bool validDestination = false;
    for (const Position &pos : possibleMoves) {
      if (pos == move.getTo()) {
        validDestination = true;
        break;
      }
    }

    if (!validDestination) {
      return false;
    }

    // Crucial Rule: A player cannot make a move that exposes/puts their own King in Check.
    return !wouldMoveCauseCheck(move, board, piece->getColor());
  }

  // wouldMoveCauseCheck: Simulates move, checks target condition status, and rollbacks board state.
  bool wouldMoveCauseCheck(Move move, Board *board, Color kingColor) override {
    Piece *movingPiece = board->getPiece(move.getFrom());
    Piece *capturedPiece = board->getPiece(move.getTo());

    if (movingPiece == nullptr)
      return true; // Invalid move setup checker.

    // 1. Temporarily execute move representation.
    board->removePiece(move.getFrom());
    if (capturedPiece != nullptr) {
      board->removePiece(move.getTo());
    }
    board->placePiece(move.getTo(), movingPiece);

    // 2. Check if king coordinates are threatened by enemy moves.
    bool inCheck = isInCheck(kingColor, board);

    // 3. Rollback chess board configuration.
    board->removePiece(move.getTo());
    board->placePiece(move.getFrom(), movingPiece);
    if (capturedPiece != nullptr) {
      board->placePiece(move.getTo(), capturedPiece);
    }

    return inCheck;
  }

  // isInCheck: Scan if opponent pieces can capture target King coordinates square.
  bool isInCheck(Color color, Board *board) override {
    Position kingPos = board->findKing(color);
    if (kingPos.getRow() == -1)
      return false; // King coordinates missing check abort.

    Color opponentColor = (color == WHITE) ? BLACK : WHITE;
    vector<Position> opponentPieces = board->getAllPiecesOfColor(opponentColor);

    // Scan opposing moves pathways.
    for (const Position &pos : opponentPieces) {
      Piece *piece = board->getPiece(pos);
      vector<Position> moves = piece->getPossibleMoves(pos, board);
      for (const Position &targetPos : moves) {
        if (targetPos == kingPos) {
          return true; // King square threatened! check found.
        }
      }
    }
    return false;
  }

  // isCheckmate: King is in check, and no legal moves exist to escape check state.
  bool isCheckmate(Color color, Board *board) override {
    if (!isInCheck(color, board))
      return false; // Cannot be checkmate if king is not in check.

    vector<Position> pieces = board->getAllPiecesOfColor(color);
    for (const Position &pos : pieces) {
      Piece *piece = board->getPiece(pos);
      vector<Position> moves = piece->getPossibleMoves(pos, board);

      // Verify if any move escapes the check.
      for (const Position &targetPos : moves) {
        Move move(pos, targetPos, piece, board->getPiece(targetPos));
        if (isValidMove(move, board)) {
          return false; // Safe move found! Not Checkmate.
        }
      }
    }
    return true; // No moves to escape. Checkmate success!
  }

  // isStalemate: Active player is not in check, but has zero valid legal moves to make.
  bool isStalemate(Color color, Board *board) override {
    if (isInCheck(color, board))
      return false; // Cannot be stalemate if king is in check.

    vector<Position> pieces = board->getAllPiecesOfColor(color);
    for (const Position &pos : pieces) {
      Piece *piece = board->getPiece(pos);
      vector<Position> moves = piece->getPossibleMoves(pos, board);

      for (const Position &targetPos : moves) {
        Move move(pos, targetPos, piece, board->getPiece(targetPos));
        if (isValidMove(move, board)) {
          return false; // Found a valid move. Not stalemate.
        }
      }
    }
    return true; // Stalemate draw trigger success!
  }
};

// ============================================================================
// 8. CHAT SYSTEMS MODEL (Colleague model definitions)
// ============================================================================

// Message: Chat message parameters model.
class Message {
private:
  string senderId;    // Sender player identifier key.
  string content;     // Chat body string texts.
  time_t timestamp;   // Creation timestamp trace.

public:
  Message(string sId, string msg) {
    senderId = sId;
    content = msg;
    timestamp = time(0);
  }

  string getSenderId() const { return senderId; }
  string getContent() const { return content; }
  time_t getTimestamp() const { return timestamp; }

  string toString() const { 
    return "[" + senderId + "]: " + content; 
  }
};

// ============================================================================
// 9. MEDIATOR DESIGN PATTERN (Chat System Coordination)
// ============================================================================
// Mediator Pattern isliye use hota hai taaki User objects aapas me direct chat 
// transmit na karein. Match class ChatMediator acts as a central hub, jo colleagues 
// (Users) ke message coordination and transmissions cleanly handle karti hai.

// ChatMediator: Mediator Interface coordinate targets.
class ChatMediator {
public:
  virtual ~ChatMediator() {}
  virtual void sendMessage(Message *message, User *user) = 0;
  virtual void addUser(User *user) = 0;
  virtual void removeUser(User *user) = 0;
};

// Colleague: Mediator communication member template colleague base class.
class Colleague {
protected:
  ChatMediator *mediator; // Central mediator link coordinate tracking hook.

public:
  Colleague() : mediator(nullptr) {}
  virtual ~Colleague() {}
  virtual void send(Message *message) = 0;
  virtual void receive(Message *message) = 0;
  virtual void setMediator(ChatMediator *med) { mediator = med; }
  ChatMediator *getMediator() const { return mediator; }
};

// User class: Colleague se inherit karti hai taaki chat mediator system participate limits set hon.
class User : public Colleague {
private:
  string id;     // User identifier database keys.
  string name;   // User profile username.
  int score;     // Elo score ranking level indicator.

public:
  User(string userId, string userName) : Colleague() {
    id = userId;
    name = userName;
    score = 1000; // Default rating points standard setup.
  }

  string getId() const { return id; }
  string getName() const { return name; }
  int getScore() const { return score; }

  void incrementScore(int points) { score += points; }
  void decrementScore(int points) { score -= points; }

  string toString() const {
    return name + " (Score: " + to_string(score) + ")";
  }

  // send: Colleague method implementation to pass messages to mediator.
  void send(Message *message) override {
    if (mediator != nullptr) {
      mediator->sendMessage(message, this);
    }
  }

  // receive: Callback triggered by mediator when another user sends a chat message.
  void receive(Message *message) override {
    cout << "User " << name << " received message from "
         << message->getSenderId() << ": " << message->getContent() << endl;
  }
};

// Match: acts as Concrete Mediator coordinate system and also manages the chess gameplay.
class Match : public ChatMediator {
private:
  string matchId;                  // Match unique configuration id keys.
  User *whitePlayer;               // Colleague White player.
  User *blackPlayer;               // Colleague Black player.
  Board *board;                    // Dedicated Board settings parameters references.
  ChessRules *rules;               // Rules verification context pointer (Strategy Pattern).
  Color currentTurn;               // Tracks who turn is active.
  GameStatus status;               // IN_PROGRESS, COMPLETED settings state.
  vector<Move> moveHistory;        // Tracks all moves played in the match.
  vector<Message *> chatHistory;   // Tracks chat history logs for this match.

public:
  Match(string mId, User *white, User *black) {
    matchId = mId;
    whitePlayer = white;
    blackPlayer = black;
    board = new Board();
    rules = new StandardChessRules(); // Standard strategies inject.
    currentTurn = WHITE;
    status = IN_PROGRESS;

    // Register this match as the mediator for both player colleagues.
    whitePlayer->setMediator(this);
    blackPlayer->setMediator(this);

    cout << "Match started between " << whitePlayer->getName()
         << " (White) and " << blackPlayer->getName() << " (Black)" << endl;
  }

  ~Match() {
    delete board;
    delete rules;
  }

  // makeMove: Validates coordinates, executes move, and check game-over statuses checks.
  bool makeMove(Position from, Position to, User *player) {
    if (status != IN_PROGRESS) {
      cout << "Game is not in progress!" << endl;
      return false;
    }

    Color playerColor = getPlayerColor(player);
    if (playerColor != currentTurn) {
      cout << "It's not your turn!" << endl;
      return false;
    }

    Piece *piece = board->getPiece(from);
    if (piece == nullptr || piece->getColor() != playerColor) {
      cout << "Invalid piece selection!" << endl;
      return false;
    }

    Move move(from, to, piece, board->getPiece(to));

    // Rule validation strategies execution verification check.
    if (!rules->isValidMove(move, board)) {
      cout << "Invalid move!" << endl;
      return false;
    }

    // Execute valid move.
    board->movePiece(from, to);
    moveHistory.push_back(move);

    cout << player->getName() << " moved " << piece->getSymbol() << " from "
         << from.toChessNotation() << " to " << to.toChessNotation() << endl;

    board->display(); // Render board console visualization updates.

    // Game End check updates (checkmate / stalemate conditions check).
    Color opponentColor = (currentTurn == WHITE) ? BLACK : WHITE;
    if (rules->isCheckmate(opponentColor, board)) {
      endGame(player, "checkmate");
      return true;
    } else if (rules->isStalemate(opponentColor, board)) {
      endGame(player, "stalemate");
      return true;
    } else {
      currentTurn = opponentColor; // Turn shifts to other player.
      if (rules->isInCheck(opponentColor, board)) {
        cout << getPlayerByColor(opponentColor)->getName() << " is in check!" << endl;
      }
    }

    return true;
  }

  // quitGame: Allows player to resign match. Sets score penalties.
  void quitGame(User *player) {
    User *opponent = (player == whitePlayer) ? blackPlayer : whitePlayer;
    endGame(opponent, "quit");
    player->decrementScore(50); // Resign penalty.
    cout << player->getName() << " quit the game. Score decreased by 50." << endl;
  }

  // endGame: Terminates match status, updates player ratings Elo points.
  void endGame(User *winner, string reason) {
    status = COMPLETED;

    if (winner != nullptr) {
      User *loser = (winner == whitePlayer) ? blackPlayer : whitePlayer;
      winner->incrementScore(30);
      loser->decrementScore(20);
      cout << "Game ended - " << winner->getName() << " wins by " << reason << "!" << endl;
      cout << "Score update: " << winner->getName() << " +30, "
           << loser->getName() << " -20" << endl;
    } else {
      cout << "Game ended in " << reason << "! No score change." << endl;
    }
  }

  Color getPlayerColor(User *player) {
    return (player == whitePlayer) ? WHITE : BLACK;
  }

  User *getPlayerByColor(Color color) {
    return (color == WHITE) ? whitePlayer : blackPlayer;
  }

  // Mediator Pattern sendMessage: Broadcasts message details to recipients.
  void sendMessage(Message *message, User *user) override {
    chatHistory.push_back(message);

    User *recipient = (user == whitePlayer) ? blackPlayer : whitePlayer;
    recipient->receive(message); // Forward message to colleague player.
    cout << "Chat in match " << matchId << " - " << message->getContent() << endl;
  }

  void addUser(User *user) override {
    // 2-player game limit doesn't allow adding users to active match context.
  }

  void removeUser(User *user) override { 
    quitGame(user); 
  }

  string getMatchId() const { return matchId; }
  GameStatus getStatus() const { return status; }
  User *getWhitePlayer() const { return whitePlayer; }
  User *getBlackPlayer() const { return blackPlayer; }
  Board *getBoard() const { return board; }
};

// ============================================================================
// 10. MATCHMAKER SYSTEM STRATEGIES (Strategy Pattern)
// ============================================================================

// MatchingStrategy: Strategy interface to pair waiting players.
class MatchingStrategy {
public:
  virtual ~MatchingStrategy() {}
  virtual User *findMatch(User *user, vector<User *> &waitingUsers) = 0;
};

// ScoreBasedMatching: Concrete Strategy pairing players with comparable rating points Elo scores.
class ScoreBasedMatching : public MatchingStrategy {
private:
  int scoreTolerance; // Maximum score difference allowed between players.

public:
  ScoreBasedMatching(int tolerance) { 
    scoreTolerance = tolerance; 
  }

  User *findMatch(User *user, vector<User *> &waitingUsers) override {
    User *bestMatch = nullptr;
    int bestScoreDiff = INT_MAX;

    for (User *waitingUser : waitingUsers) {
      if (waitingUser->getId() != user->getId()) {
        int scoreDiff = abs(waitingUser->getScore() - user->getScore());
        // Verify Elo checks difference range limits tolerance.
        if (scoreDiff <= scoreTolerance && scoreDiff < bestScoreDiff) {
          bestMatch = waitingUser;
          bestScoreDiff = scoreDiff;
        }
      }
    }
    return bestMatch; // Returns closest waiting player profile, else nullptr.
  }
};

// ============================================================================
// 11. GAME MANAGER SINGLETON PATTERN (Central Subsystems Coordinator)
// ============================================================================

// GameManager: Single central coordinating singleton point.
class GameManager {
private:
  static GameManager *instance;       // Singleton instance pointer storage.
  map<string, Match *> activeMatches; // activeMatch list maps.
  vector<User *> waitingUsers;         // Matchmaking queue/waiting lists.
  MatchingStrategy *matchingStrategy; // Pairing algorithm (Strategy Pattern).
  int matchCounter;                   // Match id auto-increment keys counter.

  GameManager() {
    matchingStrategy = new ScoreBasedMatching(100); // Pair players within 100 Elo points.
    matchCounter = 0;
  }

public:
  // getInstance: Singleton constructor allocator checks hook.
  static GameManager *getInstance() {
    if (instance == nullptr) {
      instance = new GameManager();
    }
    return instance;
  }

  ~GameManager() {
    delete matchingStrategy;
    for (auto &pair : activeMatches) {
      delete pair.second;
    }
  }

  // requestMatch: Triggers pairing strategy verification.
  void requestMatch(User *user) {
    cout << user->getName() << " is looking for a match..." << endl;

    User *opponent = matchingStrategy->findMatch(user, waitingUsers);

    if (opponent != nullptr) {
      // 1. Remove opponent profile from waiting lists queue.
      waitingUsers.erase(remove(waitingUsers.begin(), waitingUsers.end(), opponent), waitingUsers.end());

      // 2. Create new Match mediator coordinator.
      string matchId = "MATCH_" + to_string(++matchCounter);
      Match *match = new Match(matchId, user, opponent);
      activeMatches[matchId] = match;

      cout << "Match found! " << user->getName() << " vs " << opponent->getName() << endl;
      match->getBoard()->display();
    } else {
      // Add player to waiting list.
      waitingUsers.push_back(user);
      cout << user->getName() << " added to waiting list." << endl;
    }
  }

  void makeMove(string matchId, Position from, Position to, User *player) {
    if (activeMatches.find(matchId) != activeMatches.end()) {
      Match *match = activeMatches[matchId];
      match->makeMove(from, to, player);

      // Clean match details once completed.
      if (match->getStatus() == COMPLETED) {
        delete match;
        activeMatches.erase(matchId);
        cout << "Match " << matchId << " completed and removed from active matches." << endl;
      }
    }
  }

  void quitMatch(string matchId, User *player) {
    if (activeMatches.find(matchId) != activeMatches.end()) {
      Match *match = activeMatches[matchId];
      match->quitGame(player);
      delete match;
      activeMatches.erase(matchId);
    }
  }

  void sendChatMessage(string matchId, string message, User *user) {
    if (activeMatches.find(matchId) != activeMatches.end()) {
      Match *match = activeMatches[matchId];
      Message *msg = new Message(user->getId(), message);
      match->sendMessage(msg, user);
    }
  }

  Match *getMatch(string matchId) {
    if (activeMatches.find(matchId) != activeMatches.end()) {
      return activeMatches[matchId];
    }
    return nullptr;
  }

  void displayActiveMatches() {
    cout << "\n=== Active Matches ===" << endl;
    for (auto &pair : activeMatches) {
      Match *match = pair.second;
      cout << "Match " << match->getMatchId() << ": "
           << match->getWhitePlayer()->getName() << " vs "
           << match->getBlackPlayer()->getName() << endl;
    }
    cout << "Total active matches: " << activeMatches.size() << endl;
    cout << "Users waiting: " << waitingUsers.size() << endl;
  }
};

// Initialize static singleton instance pointer.
GameManager *GameManager::instance = nullptr;

// ============================================================================
// 12. CHESS SYSTEM DEMO COMPONENT (Scholar's Mate walkthrough verification)
// ============================================================================

class ChessSystemDemo {
public:
  // demonstrateScholarsMate: Simulates a classic 4-move Scholar's Mate checkmate configuration.
  static void demonstrateScholarsMate() {
    cout << "\n=== Scholar's Mate Demo (4-move checkmate) ===" << endl;

    User *aditya = new User("DEMO_1", "Aditya");
    User *rohit = new User("DEMO_2", "Rohit");

    Match *demoMatch = new Match("DEMO_MATCH", aditya, rohit);
    demoMatch->getBoard()->display();

    // Proper Scholar's Mate sequence with correct coordinates mappings check:
    cout << "\nMove 1: White e2-e4" << endl;
    demoMatch->makeMove(Position(6, 4), Position(4, 4), aditya); // e2-e4

    cout << "\nMove 1: Black e7-e5" << endl;
    demoMatch->makeMove(Position(1, 4), Position(3, 4), rohit); // e7-e5

    cout << "\nMove 2: White Bf1-c4 (targeting f7)" << endl;
    demoMatch->makeMove(Position(7, 5), Position(4, 2), aditya); // Bf1-c4

    cout << "\nMove 2: Black Nb8-c6 (developing)" << endl;
    demoMatch->makeMove(Position(0, 1), Position(2, 2), rohit); // Nb8-c6

    cout << "\nMove 3: White Qd1-h5 (attacking f7 and h7)" << endl;
    demoMatch->makeMove(Position(7, 3), Position(3, 7), aditya); // Qd1-h5

    cout << "\nMove 3: Black Ng8-f6?? (defending h7 but exposing f7)" << endl;
    demoMatch->makeMove(Position(0, 6), Position(2, 5), rohit); // Ng8-f6

    cout << "\nMove 4: White Qh5xf7# (Checkmate!)" << endl;
    bool gameEnded = demoMatch->makeMove(Position(3, 7), Position(1, 5), aditya); // Qh5xf7#

    if (demoMatch->getStatus() != COMPLETED) {
      cout << "Note: Checkmate detection may need refinement for this position." << endl;
    }

    // Mediator chat verification logs check.
    cout << "\n=== Testing Chat Functionality ===" << endl;
    aditya->send(new Message(aditya->getId(), "Good game!"));
    rohit->send(new Message(rohit->getId(), "Thanks, that was a quick one!"));

    // Clean up demo matches structures allocations.
    delete demoMatch;
    delete aditya;
    delete rohit;
  }
};

// ============================================================================
// 13. CLIENT DRIVER ENTRY POINT
// ============================================================================
int main() {
  cout << "=== Chess System with Design Patterns Demo ===" << endl;

  // Run Scholar's Mate Demo
  ChessSystemDemo::demonstrateScholarsMate();

  // Run GameManager Singleton & Matchmaking Demo
  cout << "\n=== Game Manager Demo ===" << endl;
  GameManager *gm = GameManager::getInstance();

  User *saurav = new User("USER_1", "Saurav");
  User *manish = new User("USER_2", "Manish");
  User *abhishek = new User("USER_3", "Abishek");

  cout << "\nUsers: " << saurav->toString() << ", " << manish->toString()
       << ", " << abhishek->toString() << endl;

  // Request pairings
  gm->requestMatch(saurav);
  gm->requestMatch(manish);   // Pairs Saurav & Manish immediately.
  gm->requestMatch(abhishek); // Abhishek goes to wait list.

  gm->displayActiveMatches();

  // Memory cleanups
  delete saurav;
  delete manish;
  delete abhishek;

  // Clean singleton pointer
  delete GameManager::getInstance();
  return 0;
}
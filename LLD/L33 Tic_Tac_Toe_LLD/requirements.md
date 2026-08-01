# Tic Tac Toe LLD - Requirements

## Functional Requirements

1. User can start a game with configurable board size.
2. System should support player registration with unique symbols.
3. Players make moves turn-by-turn by giving row and column.
4. System validates moves (bounds + empty cell check).
5. System should detect winner by:
   - Any complete row,
   - Any complete column,
   - Main diagonal,
   - Anti-diagonal.
6. System should detect draw if all cells are filled and no winner.
7. System should emit notifications for:
   - Game start,
   - Every valid move,
   - Win/Draw events.

## Non-Functional Requirements

1. Code should be modular and easy to extend.
2. Game rule logic should be strategy-based.
3. Notification logic should be observer-based.

## Folder Structure

- `core/` - game engine and factory
- `enums/` - game enums
- `models/` - board, player, symbol
- `rules/` - rule interfaces and implementations
- `observers/` - observer interfaces and notifiers
- `main.cpp` - entry point

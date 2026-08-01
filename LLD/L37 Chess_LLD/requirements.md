# Chess LLD - Requirements

## Functional Requirements

1. Support standard 8x8 chess board initialization.
2. Support all piece types and legal move generation.
3. Validate moves and reject illegal moves.
4. Detect check, checkmate, and stalemate.
5. Maintain move history for a match.
6. Support chat between two players in a match.
7. Support score-based matchmaking for users.
8. Update user scores when match ends or user quits.

## Non-Functional Requirements

1. Use modular, folder-based architecture.
2. Use strategy pattern for chess rules and matchmaking.
3. Use mediator pattern for in-match chat.
4. Use singleton pattern for game manager.
5. Use `#ifndef / #define / #endif` guards in all headers.
6. Use custom namespace `chess_lld`.

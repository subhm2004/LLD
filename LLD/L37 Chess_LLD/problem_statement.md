# Chess LLD - Problem Statement

Design an online chess system that supports:

- Piece-wise move generation and validation.
- Rule enforcement for check, checkmate, and stalemate.
- Match lifecycle between two users.
- In-match chat between players.
- Score-aware matchmaking for queued users.

The design should be modular, extensible, and interview-ready with clear separation of:
- models,
- rule engine,
- chess board and match orchestration,
- strategy-based matchmaking.

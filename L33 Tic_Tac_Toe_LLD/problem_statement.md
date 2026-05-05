# Tic Tac Toe LLD - Problem Statement

Design an extensible, object-oriented Tic Tac Toe system that supports:

- Configurable board size (`N x N`).
- Multiple players (at least two in current implementation).
- Turn-based gameplay with move validation.
- Win and draw detection.
- Real-time notifications for important game events.

The system should be modular and interview-friendly, so each concern is isolated:

- `models` for entities like board, symbol, and player.
- `rules` for game logic and future rule variants.
- `observers` for notification extensions.
- `core` for game orchestration and factory-based creation.

The design should allow easy extension to:

- New rule variants (e.g., timed mode, wildcard mode).
- Alternate observers (UI notifier, file logger, websocket notifier).
- Bot player strategy modules.

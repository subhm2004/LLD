# Snake and Ladder LLD - Requirements

## Functional Requirements

1. Support `N x N` board creation.
2. Support standard board setup with predefined snakes/ladders.
3. Support random board setup with difficulty levels.
4. Support custom setup by counts or exact positions.
5. Support multiplayer turn-based gameplay.
6. Enforce exact-roll win condition.
7. Notify game events using observer pattern.

## Non-Functional Requirements

1. Modular code with folder-based structure.
2. Strategy pattern for board setup and game rules.
3. Factory for game creation variants.
4. Use header guards with `#ifndef / #define / #endif`.
5. Use `snake_ladder_lld` namespace.
6. Include `<bits/stdc++.h>` and `using namespace std;` in files.

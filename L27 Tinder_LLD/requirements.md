# Requirements: Tinder-Like Matching System

## Functional Requirements

1. User should be able to register with profile and location.
2. System should show nearby profiles within configurable distance.
3. User should be able to swipe left/right on another profile.
4. Mutual right swipe should create a match.
5. Matched users should be able to chat.
6. System should support super-like.
7. System should enforce daily swipe limit.
8. System should support unmatch and block user flow.

## Non-Functional Requirements

1. Matching logic should be modular and extensible.
2. Core flows should be deterministic and in-memory.
3. Code should separate domain models from orchestration.
4. Design should be interview-friendly and easy to explain.

## Assumptions

1. Authentication is out of scope.
2. Media moderation and recommendation ML are out of scope.
3. Persistence/database is out of scope.
4. Single process execution model is sufficient.

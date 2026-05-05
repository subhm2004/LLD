# Requirements: LinkedIn-Like System

## Functional Requirements

1. User can register with profile details.
2. User can send connection request to another user.
3. User can accept connection request.
4. Connected users can create posts.
5. Users can like posts.
6. System should provide personalized feed containing own and connections' posts.

## Non-Functional Requirements

1. Design should be modular and extensible.
2. Feed retrieval should be efficient for in-memory usage.
3. Core operations should include validation for invalid entities.
4. Separation of concerns between domain models and feed logic.

## Assumptions

1. Authentication and authorization are out of scope.
2. Comments, shares, and notifications are out of current scope.
3. Persistence/database is out of scope.
4. Ranking is simplified with likes and recency.

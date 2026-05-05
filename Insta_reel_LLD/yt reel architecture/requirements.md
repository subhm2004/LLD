# Requirements: Insta/YouTube Reel System

## Functional Requirements

1. User can register on the platform.
2. User can follow other users.
3. User can upload a reel with:
   - reel id
   - owner id
   - caption
   - duration
4. User can view a reel.
5. User can like and unlike a reel.
6. User can comment on a reel.
7. System can generate feed for a user based on:
   - own reels
   - followed users' reels
8. Feed should return top N reels.

## Non-Functional Requirements

1. Design should be modular and easy to extend.
2. Core operations should be efficient for in-memory usage.
3. Code should follow OOP principles and separation of concerns.
4. Basic validation should be present for invalid user/reel ids.

## Assumptions

1. Authentication/authorization is out of scope.
2. Media storage and streaming are out of scope.
3. Network and distributed scaling are out of scope.
4. Persistence is not required; data is in-memory.

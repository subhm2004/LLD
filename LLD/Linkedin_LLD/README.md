# LinkedIn LLD (C++)

A modular, interview-oriented low-level design of a LinkedIn-style professional networking platform.
This project focuses on clean separation of concerns, extensibility, and realistic domain modeling for core networking and feed workflows.

## Overview

The system supports end-to-end social graph and engagement flow:

- register user profiles
- send/accept/reject connection requests
- maintain connection state and request lifecycle
- create and like posts
- generate personalized feed from self + accepted connections
- rank feed by engagement and recency

The implementation is in-memory and deterministic, making it easy to reason about in interviews and simple to extend into production-like architecture later.

## Design Goals

- **Modularity**: domain models, orchestration logic, and feed strategy are separated.
- **Readability**: straightforward C++ classes with explicit responsibilities.
- **Extensibility**: easy to plug in features like comments, notifications, job posts, and persistence.
- **Correctness-first**: strict validations and state checks for invalid flows.

## Project Structure

```text
Linkedin_LLD/
├── main.cpp
├── core/
│   └── LinkedinSystem.h
├── services/
│   └── FeedService.h
├── models/
│   ├── UserProfile.h
│   ├── ConnectionRequest.h
│   └── Post.h
└── enums/
    └── ConnectionStatus.h
```

## Core Modules

- `LinkedinSystem`
  - central orchestration layer
  - handles user onboarding, request workflow, posting, likes, and feed retrieval
- `FeedService`
  - builds personalized feed
  - ranks posts using likes first, then recency
- `UserProfile`
  - professional identity abstraction (`userId`, name, headline)
- `ConnectionRequest`
  - tracks directed request state (`PENDING`, `ACCEPTED`, `REJECTED`)
- `Post`
  - author/content metadata + engagement
  - duplicate-like prevention via per-post liker set

## Implemented Features

- User registration with profile metadata
- Connection request creation with validations
- Request state transitions:
  - `PENDING -> ACCEPTED`
  - `PENDING -> REJECTED`
- Duplicate pending request prevention between same user pair
- Post creation for valid users
- Like operation with duplicate-like protection
- Personalized feed:
  - includes own posts + accepted connections' posts
  - sorted by like count (desc), then creation time (desc)
- Defensive validations:
  - invalid user checks
  - invalid request checks
  - invalid post checks
  - invalid state transitions

## Sample Flow (Demonstrated in `main.cpp`)

1. Register users (`U1`, `U2`, `U3`)
2. Send multiple connection requests
3. Accept some requests and reject others
4. Create posts by different users
5. Like posts (including duplicate-like scenario)
6. Fetch feed for a user and print ranked output

## Complexity Notes

- user lookup: `O(1)` average using hash maps
- request lookup by id: `O(1)` average
- feed build: `O(P log P)` where `P` is candidate posts in feed (sorting step)
- like operation: `O(1)` average

## Build and Run

```bash
g++ -std=c++17 main.cpp -o linkedin_app
./linkedin_app
```

## Interview Discussion Points

- Why feed logic is extracted into a dedicated service (`FeedService`)
- Trade-off of in-memory pointer-based post storage vs persistence layer
- How to evolve design for:
  - comments and shares
  - notification fanout
  - follow-vs-connect graph variants
  - pagination and feed caching
  - DB-backed repositories and APIs

## Future Enhancements

- comments and threaded replies
- post share/repost
- profile search and recommendation
- mutual connections
- pending request inbox view
- job posting and application module
- repository layer with SQL/NoSQL persistence

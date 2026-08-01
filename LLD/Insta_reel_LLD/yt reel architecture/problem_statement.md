# Problem Statement: Insta/YouTube Reel System

Design a Low-Level System for a short-video platform similar to Instagram Reels / YouTube Shorts.

The system should allow users to:

- Upload short reels with metadata (caption, duration)
- Watch reels
- Like and unlike reels
- Comment on reels
- Follow other users
- Get a personalized feed

The goal is to model the core domain objects and service interactions in a clean, extensible, object-oriented design using C++.

## Scope

- In-memory implementation only
- Single-machine simulation
- Focus on LLD, not distributed system concerns

## Key Design Objective

Build a modular design where features like recommendation strategy, moderation, ads, and analytics can be added later with minimal code changes.

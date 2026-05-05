# Insta/YouTube Reel Architecture (LLD in C++)

This folder contains a Low-Level Design implementation for a short-video platform (Insta Reels / YouTube Shorts style).

## Implemented Components

- `User`: register + follow other users
- `Reel`: metadata, views, unique viewers, likes, comments
- `ReelPlatformService`: orchestration layer for all operations
- Feed generation for a user based on follow graph + simple ranking

## Features Covered

- Upload reel
- View reel
- Like / unlike reel
- Comment on reel
- Follow users
- Personalized feed generation

## Structure

- `main.cpp`
- `services/ReelPlatformService.h`
- `models/Reel.h`
- `models/User.h`
- `models/Comment.h`

## Run

```bash
g++ -std=c++17 main.cpp -o reels_app
./reels_app
```

# Spotify LLD Project Explanation

## 1) Project Overview

This project is a C++ low-level design (LLD) implementation of a music player system inspired by Spotify-like behavior.

The system supports:
- Song library management
- Playlist creation and song association
- Multiple audio output devices (Bluetooth, wired speaker, headphones)
- Multiple playback strategies (sequential, random, custom queue)
- Basic player controls (play, pause, previous, play-all)

The entry point is `C++ Code/MusicPlayerSystem/MusicPlayerApplication/main.cpp`, which demonstrates the full flow end-to-end.

## 2) Folder and Module Responsibilities

### `models/`
- `Song.h`: Represents a song with title, artist, and file path.
- `Playlist.h`: Represents a playlist with a name and list of song pointers.

### `core/`
- `AudioEngine.h`: Core playback state handler.
  - Tracks current song
  - Handles play/resume/pause transitions
  - Delegates actual output to an `IAudioOutputDevice`

### `device/`
- `IAudioOutputDevice.h`: Common interface for all output devices.
- `BluetoothSpeakerAdapter.h`, `WiredSpeakerAdapter.h`, `HeadphonesAdapter.h`:
  - Adapter layer converting `Song` into payload strings
  - Calling concrete external APIs

### `external/`
- `BluetoothSpeakerAPI.h`, `WiredSpeakerAPI.h`, `HeadphonesAPI.h`:
  - Simulated vendor/device APIs (print-based output)
  - Different play methods per integration style

### `factories/`
- `DeviceFactory.h`:
  - Centralized creation of adapter objects based on `DeviceType`

### `strategies/`
- `PlayStrategy.h`: Strategy interface for playback order.
- `SequentialPlayStrategy.h`: Linear order playback.
- `RandomPlayStrategy.h`: Random, no-repeat-until-exhausted playback.
- `CustomQueueStrategy.h`: Allows explicit queueing of songs for upcoming playback.

### `managers/`
- `PlaylistManager.h`: Singleton registry and access for playlists.
- `DeviceManager.h`: Singleton holder for currently connected output device.
- `StrategyManager.h`: Singleton provider for playback strategy instances.

### Root orchestration classes
- `MusicPlayerFacade.h`: Main orchestration facade combining managers + engine.
- `MusicPlayerApplication.h`: High-level application API and singleton wrapper over facade calls.
- `main.cpp`: Demo workflow execution.

## 3) Execution Flow (Main Scenario)

1. Create application singleton via `MusicPlayerApplication::getInstance()`.
2. Add songs to in-memory song library.
3. Create playlist and add songs by title lookup.
4. Connect output device using `DeviceType`.
5. Play/pause/resume a single song.
6. Select each strategy and load playlist.
7. Play all tracks according to current strategy.
8. In custom queue mode, enqueue selected songs to play next.
9. In sequential mode, try previous-track navigation.

## 4) Key Design Patterns Used

- **Singleton Pattern**
  - Used in `MusicPlayerApplication`, `MusicPlayerFacade`, `PlaylistManager`, `DeviceManager`, `StrategyManager`.
  - Ensures one shared stateful instance for each manager/orchestrator.

- **Facade Pattern**
  - `MusicPlayerFacade` hides orchestration complexity from callers.
  - Client code avoids handling low-level interactions directly.

- **Strategy Pattern**
  - `PlayStrategy` defines playback behavior contract.
  - Different strategy classes encapsulate ordering logic independently.

- **Adapter Pattern**
  - Device adapters map internal interface (`IAudioOutputDevice`) to external vendor APIs.
  - Keeps playback engine independent from concrete hardware API signatures.

- **Factory Pattern**
  - `DeviceFactory` centralizes creation logic for audio output device adapters.
  - Reduces object creation complexity in managers/facade.

## 5) Important Class Interactions

- `MusicPlayerApplication` performs song-title lookup and delegates operations to `MusicPlayerFacade`.
- `MusicPlayerFacade` coordinates:
  - `PlaylistManager` for playlist loading
  - `StrategyManager` for playback algorithm selection
  - `DeviceManager` for connected output device
  - `AudioEngine` for current playback state
- `AudioEngine` never directly knows device implementations; it uses `IAudioOutputDevice`.
- Device adapters then call concrete external API objects.

## 6) Error Handling and Guard Rails

The code throws `runtime_error` for invalid operations, such as:
- Adding unknown song title to playlist
- Loading nonexistent playlist
- Playing without connecting a device
- Pausing when song mismatch or no active song
- Navigating playback when playlist is not loaded or empty
- Enqueuing null songs

These checks improve API safety and demonstration clarity.

## 7) Current Constraints / Notable Observations

- Entire implementation is header-only and uses raw pointers heavily.
- No explicit memory cleanup (destructors / smart pointers), so memory leaks are possible in long-lived usage.
- No persistence layer (library/playlists are in-memory only).
- No thread-safety in singleton initialization or shared state access.
- Playback is simulation (console logs), not real audio streaming.
- Some previous-track conditions in strategies are strict and may skip expected index `0` edge cases.

## 8) How to Extend This Design

- Add smart pointers (`unique_ptr` / `shared_ptr`) and proper destructors.
- Add `RepeatOne`, `RepeatAll`, and `ShuffleWithSeed` strategies.
- Add seek/stop/next controls and richer playback state machine.
- Add user accounts and per-user playlist ownership.
- Add repository/storage abstraction for persistence.
- Add unit tests for each strategy and manager edge cases.

## 9) Quick Recap

This is a clean LLD-focused demonstration of a music player that intentionally emphasizes object-oriented design patterns (Facade + Strategy + Adapter + Factory + Singleton) and separation of responsibilities over production-ready runtime concerns.

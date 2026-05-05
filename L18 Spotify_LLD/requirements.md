# Requirements for Music Player System

## Functional Requirements

1. **Song Library Management**
   - System shall allow creating songs with title, artist, and file path.
   - System shall store songs in an in-memory library.
   - System shall support lookup of songs by title.

2. **Playlist Management**
   - System shall allow creation of playlists by name.
   - System shall prevent duplicate playlist names.
   - System shall allow adding existing songs from library to a playlist.
   - System shall return an error when adding songs to a nonexistent playlist.

3. **Audio Device Management**
   - System shall support connecting one output device at a time.
   - System shall support device types: Bluetooth speaker, wired speaker, and headphones.
   - System shall allow switching connected device at runtime.
   - System shall expose whether any output device is currently connected.

4. **Playback Controls**
   - System shall allow playing a single selected song.
   - System shall allow pausing the currently playing song.
   - System shall allow resuming playback of the paused current song.
   - System shall prevent pausing when no song is playing.

5. **Playlist Playback**
   - System shall allow loading a selected playlist for playback.
   - System shall require a playback strategy before playlist playback.
   - System shall support play-all operation based on the active strategy.
   - System shall support playing previous track when strategy supports it.

6. **Playback Strategies**
   - System shall support sequential playback strategy.
   - System shall support random playback strategy.
   - System shall support custom queue playback strategy.
   - System shall allow strategy selection at runtime.

7. **Custom Queue Behavior**
   - System shall allow enqueueing songs to be played next.
   - System shall prioritize queued songs over default sequential traversal in custom queue mode.

8. **System-Level Validations**
   - System shall reject playback when no audio device is connected.
   - System shall reject operations referencing unknown songs.
   - System shall reject operations on unloaded or empty playlists where applicable.
   - System shall communicate validation failures via exceptions/messages.

## Non-Functional Requirements

1. **Modularity and Separation of Concerns**
   - System should maintain clear boundaries between models, managers, strategies, device adapters, and orchestration layers.
   - Business logic should remain independent from external vendor API signatures.

2. **Extensibility**
   - System should allow adding new playback strategies with minimal changes to existing code.
   - System should allow adding new device types with minimal impact by using adapter + factory abstractions.

3. **Maintainability**
   - Code should remain readable and consistent with OOP principles.
   - Responsibilities should remain concentrated in dedicated classes (single-responsibility intent).

4. **Reliability**
   - Invalid operations should fail fast with clear runtime error messages.
   - Core APIs should enforce preconditions (playlist loaded, device connected, song existence).

5. **Performance (Current Scope)**
   - In-memory operations should provide responsive behavior for small to medium dataset sizes typical of LLD demos.
   - Playback strategy operations should avoid unnecessary full scans where possible.

6. **Scalability (Design-Level)**
   - Architecture should support future introduction of persistence, caching, and multi-user features without major rewrites.

7. **Testability**
   - Strategy logic, manager validation, and facade orchestration should be testable as independent units.
   - External APIs should remain mockable through adapter interfaces.

8. **Portability**
   - Implementation should stay compatible with standard C++ toolchains and not rely on platform-specific APIs for core behavior.

9. **Usability (Developer/Console)**
   - System should provide clear console messages to indicate playback actions, device connection events, and completion states.

10. **Future Production Hardening (Recommended)**
    - System should evolve toward smart pointers and deterministic cleanup to avoid leaks.
    - System should include thread safety if shared singletons are used in concurrent contexts.
    - System should include structured logging and robust error taxonomy for production usage.

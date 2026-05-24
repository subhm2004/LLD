# Google Docs / Collaborative Editor LLD (C++17)

In-memory **collaborative document editor** — share permissions, revision-checked edits, live sync (Observer), cursor presence, undo.

## Quick start

```bash
./compile.sh
./collab_editor_app
```

## Docs

| File | Purpose |
|------|---------|
| [`problem_statement.md`](./problem_statement.md) | Scope |
| [`requirements.md`](./requirements.md) | FR / NFR |

## Architecture

```
Google_Docs_Collaborative_Editor_LLD/
├── core/CollaborativeEditorSystem.h   # Facade
├── services/
│   ├── UserRegistryService.h
│   ├── DocumentService.h
│   ├── SharingService.h
│   ├── EditService.h
│   ├── PresenceService.h
│   └── UndoRedoService.h              # Memento snapshots
├── collaboration/
│   ├── IEditListener.h
│   └── EditNotificationHub.h          # Observer broadcast
└── main.cpp
```

## Main APIs

| API | Description |
|-----|-------------|
| `registerUser` | Create user |
| `createDocument` | Owner creates doc |
| `shareDocument` | VIEW / COMMENT / EDIT |
| `joinSession` / `leaveSession` | Subscribe to live edits |
| `insertText` / `deleteText` | Apply edit at revision |
| `updateCursor` / `listCursors` | Presence |
| `undoLastEdit` | Roll back snapshot |

## Design patterns

| Pattern | Where |
|---------|--------|
| **Facade** | `CollaborativeEditorSystem` |
| **Observer** | `IEditListener` + `EditNotificationHub` |
| **Memento** | `UndoRedoService` snapshot stack |
| **Service layer** | Document / Share / Edit / Presence |

## Interview talking points

- **Revision lock** vs **OT/CRDT** for conflict resolution
- **Permission model** (owner + shared collaborators)
- **Session hub** simulates WebSocket fan-out to peers

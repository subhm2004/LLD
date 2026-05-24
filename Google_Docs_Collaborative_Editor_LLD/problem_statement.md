# Problem Statement — Google Docs / Collaborative Editor

Design an in-memory **collaborative document editor** (Google Docs–style) where multiple users can edit the same document concurrently.

## Scope

- Create documents and register users
- Share documents with permission levels: **VIEW**, **COMMENT**, **EDIT**
- Real-time edit sync via **Observer** (broadcast insert/delete to active session)
- **Revision-based** conflict detection (stale client must refresh)
- **Cursor presence** for active collaborators
- **Undo** via document snapshot stack (Memento)

## Out of scope (extensions)

- Full Operational Transform / CRDT
- WebSocket transport, persistence, comments thread UI
- Rich text formatting, images, suggestions mode

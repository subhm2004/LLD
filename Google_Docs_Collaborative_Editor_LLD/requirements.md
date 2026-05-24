# Requirements — Google Docs / Collaborative Editor

## Functional Requirements

1. **User Management**
   - System shall allow registering users with display name and email.
   - System shall reject operations referencing unknown users.

2. **Document Management**
   - System shall allow an owner to create a document with a title.
   - System shall store document body as a single in-memory string.
   - System shall maintain a monotonic revision counter per document.

3. **Sharing & Permissions**
   - System shall allow the owner to share a document with another user.
   - System shall support permission levels: VIEW, COMMENT, EDIT.
   - System shall grant the owner implicit EDIT permission.
   - System shall reject edits from users without EDIT permission.

4. **Collaboration Session**
   - System shall allow users with access to join an active editing session.
   - System shall allow users to leave a session.
   - System shall list users currently subscribed to a document session.

5. **Text Edits**
   - System shall support INSERT at a valid index with non-empty text.
   - System shall support DELETE at a valid index with positive length within bounds.
   - System shall require the client to supply the expected current revision.
   - System shall reject edits when expected revision does not match document revision.

6. **Live Sync (Observer)**
   - System shall broadcast applied edits to all other active session listeners on the same document.
   - System shall not echo edits back to the originating user via the listener channel.

7. **Cursor Presence**
   - System shall allow collaborators to update cursor index on a document.
   - System shall list active cursor positions per document.

8. **Undo**
   - System shall support undo for users with EDIT permission using snapshot stack.
   - System shall restore prior content and revision on successful undo.

9. **Read Access**
   - System shall allow users with any granted permission to read document content and revision.

## Non-Functional Requirements

1. **In-memory only** — no database or network layer in this LLD.
2. **Thread safety** — single-threaded demo; production would add per-document locks.
3. **Extensibility** — interfaces for listeners; services separated for OT/CRDT swap later.
4. **Clear errors** — validation failures via exceptions with descriptive messages.

## Interview Extensions

- Operational Transform or CRDT for concurrent edits without strict revision reject
- Comment threads (COMMENT permission)
- Document version history and named snapshots
- WebSocket gateway + `CollaborativeEditorSystem` as backend facade

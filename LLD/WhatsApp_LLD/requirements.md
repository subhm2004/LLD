# Requirements — WhatsApp Messaging System

## Functional Requirements

1. **User Management**
   - System shall allow registering users with name and phone number.
   - System shall track online/offline status and last seen timestamp.

2. **Direct Messaging**
   - System shall support one-to-one chats between two users.
   - System shall allow sending text and media-typed messages (image, video, document).
   - System shall assign monotonic sequence numbers per chat for ordering.

3. **Group Messaging**
   - System shall allow creating groups with admin and participants.
   - System shall support sending messages in group chats.

4. **Message Delivery States**
   - System shall track message state: SENT, DELIVERED, READ.
   - System shall allow updating message status after send.

5. **Message Deletion / Unsend**
   - System shall support **Delete for me**: hide a message only from the requesting user's timeline.
   - System shall support **Delete for everyone**: replace message content with a tombstone for all participants.
   - System shall allow only the **sender** to delete for everyone.
   - System shall enforce a **time window** (default 1 hour from `sentAt`) for delete-for-everyone.
   - System shall reject delete-for-everyone when the window has expired.
   - System shall reject delete-for-everyone by non-senders.
   - System shall keep the message record in storage (for ordering/audit) but filter visibility per user.
   - System shall expose per-user visible timelines via `getDirectMessagesForUser` / `getGroupMessagesForUser`.

6. **Chat History**
   - System shall store encrypted message payloads in chat history.
   - System shall allow fetching recent chats and per-user filtered message lists.

7. **Notifications**
   - System shall notify receiver on new direct messages (extensible notification engine).

8. **Encryption (Chat Session)**
   - System shall support pluggable `EncryptionService` (Strategy pattern).
   - System shall provide `ChatSessionEncryptionService` with per-chat derived keys (`masterSecret` + `chatId`).
   - System shall encrypt message body **before** persisting to chat store (at-rest ciphertext).
   - System shall decrypt when returning `getDirectMessagesForUser` / `getGroupMessagesForUser`.
   - Ciphertext format: `WA1:` prefix + Base64(XOR(plaintext, sessionKey)) — LLD demo cipher, not AES production.
   - System shall not decrypt tombstone text for delete-for-everyone messages.

## Non-Functional Requirements

1. High availability and low latency (conceptual; in-memory demo).
2. Scalable message delivery architecture (extensible to pub/sub).
3. Extensible model for media messages and deletion rules.
4. Consistent message ordering per chat (sequence numbers).
5. End-to-end encryption support (extensible via `EncryptionService`).
6. Message ordering guaranteed per conversation.

## Message Model — Deletion Design (Interview)

| Field / behavior | Delete for me | Delete for everyone |
| ---------------- | ------------- | ------------------- |
| `deletedForUserIds_` | Actor userId added | — |
| `deletedForEveryone_` | — | `true` for all viewers |
| `content_` | Unchanged in store | Cleared; UI shows placeholder |
| Visibility API | Hidden for that user only | Tombstone for all participants |

## Interview Extensions

- Sync deletion events to other clients via WebSocket (Observer)
- Soft-delete vs hard-delete retention policies
- Admin “delete for everyone” in enterprise groups
- CRDT for offline delete reconciliation

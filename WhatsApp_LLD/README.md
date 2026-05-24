# WhatsApp LLD

## Overview
This module contains Low Level Design for a WhatsApp-like messaging system.

## Scope
- 1:1 messaging
- Group messaging
- Message delivery states
- **Delete for me** / **Delete for everyone** (Message model + `MessageDeletionService`)
- Basic media/message abstractions

## Encryption

| Component | Role |
|-----------|------|
| `EncryptionService` | Strategy interface — `encrypt` / `decrypt` |
| `ChatSessionEncryptionService` | Per-chat key from `masterSecret + chatId` |
| `crypto/ChatSessionCipher` | XOR stream + Base64 (`WA1:` prefix) |

Messages stored **encrypted**; timeline APIs return **decrypted** text.

## Message deletion APIs

| API | Description |
|-----|-------------|
| `deleteDirectMessage(chatId, messageId, userId, DeletionType)` | Delete for me or everyone |
| `getDirectMessagesForUser(chatId, userId)` | Per-user filtered timeline |
| `deleteGroupMessage(...)` | Same rules in groups |

**Rules:** Delete-for-everyone = sender only, within 1h of `sentAt`. Delete-for-me = any participant.

## Project Structure
- `models/` : core entities (`Message` deletion state)
- `services/` : `ChatService`, `MessageDeletionService`
- `enums/` : `DeletionType`, `MessageStatus`, `MessageType`
- `core/` : `WhatsAppSystem` facade

## Build & Run
```bash
g++ -std=c++17 main.cpp -o whatsapp
./whatsapp
```

# Requirements

## Functional Requirements
1. User can send message to another user.
2. User can create a group and send messages in group.
3. System tracks message state: SENT, DELIVERED, READ.
4. System stores chat history.
5. User can fetch recent chats.
6. User can see online/last seen status.
7. User can send media messages (image, video, doc).
8. One-to-one and group both support.

## Non-Functional Requirements
1. High availability and low latency.
2. Scalable message delivery architecture.
3. Extensible model for media messages.
4. Consistent message ordering per chat (logical ordering).
5. End-to-end encryption support (extensible).
6. Message ordering guaranteed per conversation.

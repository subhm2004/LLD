# Requirements

## Functional Requirements

1. Register user with unique phone number and display name.
2. Update user profile (name, optional email, business flag).
3. Lookup caller by phone — return name, spam score, tags, whether blocked for requesting user.
4. Sync contacts — batch of phone numbers enriched with directory names if found.
5. Report phone number as spam with reason (telemarketer, scam, etc.).
6. Block and unblock phone numbers for a user.
7. Search directory by name prefix (case-insensitive).
8. Search by exact phone number.
9. Log call (incoming/outgoing) and fetch recent call history.
10. List numbers blocked by a user.

## Non-Functional Requirements

1. **Extensibility** — swap spam scoring and search via Strategy.
2. **Low latency lookup** — O(1) phone hash map for caller ID.
3. **Consistent normalization** — same phone format for keys.
4. **Facade** — single `TruecallerSystem` entry for clients.
5. Scalable to DB later via repository interfaces (optional extension).

## Design Patterns

| Pattern | Use |
|---------|-----|
| **Facade** | `TruecallerSystem` |
| **Strategy** | `ISpamScoringStrategy`, spam report handling |
| **Service layer** | Lookup, Contact, Block, Search, CallLog |

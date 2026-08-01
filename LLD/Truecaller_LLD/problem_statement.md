# Truecaller — Low-Level Design

## Problem

Design a simplified **caller identification and spam protection** system like Truecaller.

Users register with a phone number, build a public profile, sync contacts, and look up unknown callers. The system supports spam reporting, blocking, and search by name or number.

## Functional scope (this LLD)

1. User registration and profile management (name, phone, email optional).
2. Global phone directory — map normalized phone → public profile.
3. **Caller ID lookup** — given a number, return name, spam score, tags, blocked status.
4. Contact sync — user uploads contacts; system enriches with directory names.
5. Report spam — increment spam score and tag.
6. Block / unblock numbers per user.
7. Search users by name (prefix) and exact number lookup.
8. Call log — record and list incoming/outgoing calls per user.

## Out of scope (HLD / extensions)

- Real-time telephony integration, SMS OTP, distributed graph DB.
- ML-based spam model — use pluggable `ISpamScoringStrategy`.
- Privacy/GDPR consent flows — mention in interview only.

## Assumptions

- One phone number per registered user (unique).
- Phone numbers normalized to E.164-style digits (demo: `+91...`).
- In-memory storage; thread-safety not required unless extended.
- Spam score 0–100; threshold 70 = likely spam.

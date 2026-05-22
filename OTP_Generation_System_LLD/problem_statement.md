# Problem Statement: OTP Generation System

Design a low-level **OTP (One-Time Password) generation and verification** system for login, signup, and payment flows.

## Functional requirements

1. **Generate** a secure OTP (default 6-digit numeric).
2. **Send** OTP via **SMS** or **Email** (simulated channels).
3. **Store** OTP with **expiry** (e.g. 5 minutes) and **max retry attempts** (e.g. 3).
4. **Verify** user-entered OTP — success / wrong code / expired / locked.
5. **Resend** OTP — invalidate previous pending OTP for same user + purpose.
6. **Rate limit** OTP requests per user (e.g. max 3 per 10 minutes).
7. Support multiple **purposes**: LOGIN, SIGNUP, PAYMENT, PASSWORD_RESET.

## Non-functional

- In-memory storage (interview scope).
- Modular: generator strategy, notification channels, services, facade.
- C++17, extensible for Redis + hashing in production.

## Out of scope

- Real Twilio/SMTP integration
- HMAC / bcrypt storage (mention in interview)
- Distributed OTP across regions

# Requirements — OTP Generation System

## Entities

| Entity | Fields |
|--------|--------|
| `OTPRecord` | otpId, userId, destination, otpCode, channel, purpose, status, createdAt, expiresAt, attemptsRemaining |
| User | userId (string) |

## Enums

| Enum | Values |
|------|--------|
| `OTPChannel` | SMS, EMAIL |
| `OTPStatus` | PENDING, VERIFIED, EXPIRED, LOCKED |
| `OTPPurpose` | LOGIN, SIGNUP, PAYMENT, PASSWORD_RESET |

## API (Facade — `OTPGenerationSystem`)

| Method | Description |
|--------|-------------|
| `sendOtp(userId, destination, channel, purpose)` | Rate check → invalidate old → generate → store → notify → return otpId |
| `verifyOtp(otpId, enteredOtp)` | Match, expiry, attempts, mark VERIFIED |
| `resendOtp(otpId)` | New OTP for same user/destination/channel/purpose |
| `printOtpStatus(otpId)` | Debug status |

## Design patterns

| Pattern | Where |
|---------|--------|
| **Strategy** | `IOTPGeneratorStrategy` — Numeric / Alphanumeric |
| **Strategy** | `INotificationChannel` — SMS / Email |
| **Facade** | `OTPGenerationSystem` |
| **Service layer** | Generator, Storage, Verification, RateLimit, Dispatcher |

## Business rules

- One active **PENDING** OTP per (userId + purpose) — old invalidated on new send.
- Wrong verify → decrement attempts; 0 attempts → **LOCKED**.
- Expired OTP → cannot verify.
- Verified OTP → cannot reuse.

## Production extensions

- Store **hash(otp + salt)** not plain text
- Redis TTL for expiry
- CAPTCHA before send
- Per-IP rate limit

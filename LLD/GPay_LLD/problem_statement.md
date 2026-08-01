# Google Pay (GPay) — Low-Level Design

## Problem

Design a simplified **UPI-based P2P payment app** like Google Pay: users link accounts, pay via UPI ID / phone, scan QR, request money, and view transaction history.

## Functional scope

1. Register user with phone + unique UPI ID (VPA).
2. Link bank account with balance (in-memory).
3. Add beneficiaries (saved VPAs).
4. **Send money** — debit sender, credit receiver, ledger entry.
5. **Scan & pay** — pay merchant VPA from QR payload.
6. **Request money** — create pending request; payer can fulfill.
7. PIN check before outgoing payment.
8. Daily transfer limit per user.
9. Transaction history (sent / received).

## Out of scope

- Real NPCI/UPI switch, bank APIs, OAuth.
- Split bills, rewards, international FX.

## Assumptions

- INR only; amounts in paise precision via `double` (demo).
- UPI ID format: `name@okbank` (validated loosely).
- Idempotent `clientRequestId` optional on transfer.

# Requirements

## Functional Requirements

1. Register user (name, phone, UPI ID).
2. Link bank account to user with initial balance.
3. Add / list beneficiaries.
4. Send money to UPI ID with PIN verification.
5. Send money by phone (resolve to UPI ID).
6. Pay merchant via QR string (`upi://pay?pa=merchant@ok&am=100`).
7. Create money request; accept request (payer pays).
8. View wallet balance and linked accounts.
9. List transactions for user (filter sent/received).
10. Reject invalid: insufficient balance, self-transfer, duplicate UPI, over daily limit.

## Non-Functional Requirements

1. **Facade** — `GPaySystem` single entry.
2. **Strategy** — `IPaymentRailStrategy` (bank account vs wallet balance).
3. **Extensible** — new payment rails without changing facade.
4. O(1) lookup: UPI → user, phone → user.
5. ACID-like transfer in memory (debit + credit atomic in one method).

## Design Patterns

| Pattern | Use |
|---------|-----|
| Facade | `GPaySystem` |
| Strategy | Bank vs wallet debit/credit |
| Factory | `TransactionFactory` |
| Service layer | Transfer, QR, Request, Ledger, Limits |

# GPay LLD (C++17)

Google Pay–style **UPI P2P payments**: register, link bank, send money, scan QR, request money, ledger.

## Quick start

```bash
./compile.sh
./gpay_app
```

## Docs

| File | Purpose |
|------|---------|
| [`problem_statement.md`](./problem_statement.md) | Scope + assumptions |
| [`requirements.md`](./requirements.md) | FR / NFR + patterns |

## Architecture

```
GPay_LLD/
├── core/GPaySystem.h           # Facade
├── models/                     # User, BankAccount, Wallet, Transaction, MoneyRequest
├── enums/                      # TransactionStatus, TransactionType, PaymentRail
├── services/                   # Transfer, Ledger, PIN, Limits, RequestMoney
├── strategies/                 # Bank vs Wallet payment rail
├── factories/                  # TransactionFactory, PaymentRailFactory
└── utils/UpiUtils.h            # UPI ID + QR parse
```

## Main APIs

| API | Description |
|-----|-------------|
| `registerUser` | Phone + UPI ID + PIN |
| `linkBankAccount` | Primary linked account + balance |
| `sendMoney` | Pay to UPI ID (PIN required) |
| `sendMoneyToPhone` | Resolve phone → UPI |
| `scanAndPay` | Parse `upi://pay?pa=...&am=...` |
| `requestMoney` / `fulfillMoneyRequest` | Collect flow |
| `getTransactionHistory` | Sent + received |

## Design patterns

| Pattern | Use |
|---------|-----|
| **Facade** | `GPaySystem` |
| **Strategy** | `BankAccountRailStrategy` vs `WalletRailStrategy` |
| **Factory** | `PaymentRailFactory`, `TransactionFactory` |
| **Service layer** | Transfer orchestration, ledger, limits |

## vs `L23 Payment_gateway_system_LLD`

| GPay LLD | Payment Gateway L23 |
|----------|---------------------|
| Consumer UPI app (P2P, QR) | Merchant payment gateway (Paytm/Razorpay) |
| User balances + ledger | External gateway + banking proxy |

## Interview extensions

- NPCI UPI switch idempotency + callback webhooks
- Split payments, mandates, recurring
- Fraud engine + device binding
- Double-entry ledger + reconciliation

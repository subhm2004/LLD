# Razorpay Payment Gateway LLD (C++17)

Razorpay-only **merchant** integration: orders, payments, capture, signed webhooks, refunds.

> Multi-gateway pattern demo → [`L23 Payment_gateway_system_LLD`](../L23%20Payment_gateway_system_LLD/)

## Quick start

```bash
./compile.sh
./razorpay_app
```

## Docs

| File | Purpose |
|------|---------|
| [`problem_statement.md`](./problem_statement.md) | Scope + goals |
| [`requirements.md`](./requirements.md) | Functional + non-functional requirements |

## Architecture

```
Razorpay_LLD/
├── core/RazorpayPaymentSystem.h       # Facade (only public entry)
├── clients/RazorpayApiClient.h         # Mock REST (no HTTP)
├── services/
│   ├── OrderService.h
│   ├── PaymentService.h
│   ├── CaptureService.h
│   ├── RefundService.h
│   ├── WebhookService.h
│   └── IdempotencyService.h
├── flows/CheckoutFlowTemplate.h        # Template Method checkout
├── strategies/IPaymentMethodValidator.h
├── factories/EntityIdFactory.h
├── crypto/SignatureVerifier.h
├── models/ · enums/
└── main.cpp
```

## Main APIs (facade)

| API | Requirements § |
|-----|----------------|
| `createOrder(paise, currency, receipt, idemKey?)` | §1 Order Management |
| `initiatePayment(orderId, method, instrument, …)` | §2 Payment Initiation |
| `capturePayment(paymentId, paise)` | §4 Payment Capture |
| `checkout(CheckoutInput)` | §7 Checkout Flow |
| `processWebhook(payload, signature)` | §6 Webhook Processing |
| `refundPayment(paymentId, paise)` | §5 Refund Management |
| `getOrder` / `getPayment` / `getRefund` | §8 Query and Facade API |

## Payment instruments (demo)

| Method | Format |
|--------|--------|
| UPI | `user@okaxis` |
| CARD | `4111111111111111\|12/28\|123` |
| WALLET | `phonepe` |

## Design patterns

| Pattern | Where |
|---------|--------|
| **Facade** | `RazorpayPaymentSystem` |
| **Template Method** | `CheckoutFlowTemplate` |
| **Strategy** | `UpiValidator`, `CardValidator`, `WalletValidator` |
| **Factory** | `EntityIdFactory`, `PaymentMethodValidatorFactory` |

## Interview extensions

- Real HMAC-SHA256 webhook verification (OpenSSL)
- Persistent store + idempotency in Redis/DB
- Payment Links / Checkout.js session id
- Reconciliation and settlement reports

**UML:** [§33 Razorpay](../docs/SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md#33-razorpay-payment-gateway) · **Patterns:** [PROJECT_DESIGN_PATTERNS § Razorpay](../docs/PROJECT_DESIGN_PATTERNS.md#razorpay-payment-gateway)

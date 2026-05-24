# Requirements for Razorpay Payment Gateway System

## Functional Requirements

1. **Order Management**
   - System shall allow creating orders with amount (in paise), currency, and receipt reference.
   - System shall generate unique order identifiers with the `order_` prefix.
   - System shall store orders in an in-memory registry.
   - System shall initialize new orders in `created` status.
   - System shall reject order creation when amount is not positive or currency is empty.
   - System shall support idempotent order creation using an optional idempotency key.
   - System shall return the same order when the same idempotency key is reused for create-order.

2. **Payment Initiation**
   - System shall allow initiating a payment against an existing order.
   - System shall generate unique payment identifiers with the `pay_` prefix.
   - System shall support payment methods: UPI, CARD, and WALLET.
   - System shall validate payment instrument details before initiating payment.
   - System shall set payment status to `authorized` on successful initiation (demo simulation).
   - System shall set payment status to `failed` with a reason when initiation fails.
   - System shall reject payment initiation when the parent order is already paid.
   - System shall move order status from `created` to `attempted` when payment starts.
   - System shall support idempotent payment initiation using an optional idempotency key.
   - System shall return the same payment when the same idempotency key is reused for initiate-payment.

3. **Payment Instrument Validation**
   - System shall validate UPI instruments as VPA format (`user@bank`).
   - System shall validate CARD instruments as `16-digit|MM/YY|CVV`.
   - System shall validate WALLET instruments as a non-empty alphanumeric provider name (e.g. `paytm`, `phonepe`).
   - System shall reject invalid instrument formats before calling the mock payment API.
   - System shall use pluggable validators per payment method (Strategy) so new methods can be added with minimal changes.

4. **Payment Capture**
   - System shall allow capturing an authorized payment for a specified amount in paise.
   - System shall reject capture when payment is not in `authorized` status.
   - System shall reject capture when amount is zero or exceeds the payment amount.
   - System shall set payment status to `captured` and record captured amount after successful capture.
   - System shall set parent order status to `paid` after successful capture.
   - System shall invoke mock API `POST /v1/payments/{id}/capture` on capture.

5. **Refund Management**
   - System shall allow partial or full refunds on captured payments.
   - System shall generate unique refund identifiers with the `rfnd_` prefix.
   - System shall track refund status (`created`, `processed`, `failed`).
   - System shall accumulate refunded amount on the payment for partial refunds.
   - System shall set payment status to `refunded` when total refunded amount reaches captured amount.
   - System shall reject refunds that exceed remaining captured balance.
   - System shall reject refunds on payments that are not captured (or not eligible for further refund).
   - System shall invoke mock API refund endpoint on refund creation.

6. **Webhook Processing**
   - System shall accept webhook payloads with an `X-Razorpay-Signature` (signature header passed to facade).
   - System shall verify webhook signature before updating any internal state.
   - System shall reject webhooks with invalid signature and shall not mutate order/payment/refund state.
   - System shall handle event `payment.authorized` by marking payment as authorized.
   - System shall handle event `payment.captured` by capturing authorized payments for full amount.
   - System shall handle event `payment.failed` by marking payment as failed.
   - System shall handle event `refund.processed` by marking refund as processed.

7. **Checkout Flow**
   - System shall provide an end-to-end checkout operation for the happy path.
   - System shall execute checkout in order: validate input → create order → initiate payment → capture payment.
   - System shall skip capture when payment initiation fails.
   - System shall validate amount, currency, receipt, and instrument as part of checkout input validation.

8. **Query and Facade API**
   - System shall expose order, payment, and refund lookup by identifier through the facade.
   - System shall expose all merchant-facing operations only through `RazorpayPaymentSystem` (facade).
   - System shall communicate validation and business rule failures via exceptions with clear messages.

## Non-Functional Requirements

1. **Modularity and Separation of Concerns**
   - System should maintain dedicated services: `OrderService`, `PaymentService`, `CaptureService`, `RefundService`, `WebhookService`, and `IdempotencyService`.
   - Business logic should remain independent of real Razorpay HTTP/SDK signatures.
   - Services should not bypass collaborators by mutating another service's internal storage directly.

2. **Extensibility**
   - System should allow adding new payment method validators via Strategy + factory without changing `PaymentService` core flow.
   - System should allow extending webhook event handling with minimal changes to `WebhookService`.
   - Architecture should support swapping mock `RazorpayApiClient` for a real HTTP client in production.

3. **Maintainability**
   - Code should remain readable and consistent with OOP principles used across the LLD repository.
   - Responsibilities should remain concentrated in dedicated classes (single-responsibility intent).
   - All implementation code should live under namespace `razorpay_lld`.

4. **Reliability**
   - Invalid operations should fail fast with clear runtime error messages.
   - Core APIs should enforce preconditions (order exists, payment authorized before capture, signature valid before webhook handling).
   - Idempotency should prevent duplicate orders and duplicate payments under retry.

5. **Identity and API Conventions**
   - System should generate `order_`, `pay_`, and `rfnd_` identifiers only through `EntityIdFactory`.
   - System should use amount in paise (integer) to align with Razorpay API conventions.
   - Mock client should log Razorpay-style REST paths without performing real network I/O.

6. **Security (Design-Level)**
   - Webhook processing should be fail-closed: no state change on signature verification failure.
   - Production evolution should use HMAC-SHA256 on raw body with constant-time signature comparison.

7. **Performance (Current Scope)**
   - In-memory operations should provide responsive behavior for demo and interview dataset sizes.
   - Checkout and capture paths should avoid unnecessary duplicate API mock calls.

8. **Testability**
   - Service layers, validators, idempotency, and facade orchestration should be testable as independent units.
   - Mock API client should remain replaceable for integration tests.

9. **Portability**
   - Implementation should build with standard C++17 toolchain via `compile.sh`.
   - System should not rely on platform-specific APIs for core payment flow logic.

10. **Usability (Developer/Console)**
    - Demo (`razorpay_app`) should print clear console output for orders, payments, captures, webhooks, and refunds.
    - Instrument format examples: UPI `user@okaxis`, CARD `4111111111111111|12/28|123`, WALLET `phonepe`.

11. **Future Production Hardening (Recommended)**
    - System should evolve toward persistent storage for orders, payments, and refunds.
    - System should store idempotency keys in Redis/DB with TTL and unique constraints.
    - System should add structured logging, metrics, and reconciliation jobs for settlement.
    - System should add PCI-compliant handling for card data (tokenization; never store raw PAN/CVV).

## Out of Scope (Current Implementation)

- Checkout.js / mobile SDK UI
- Multi-gateway routing (see `L23 Payment_gateway_system_LLD` for Paytm + Razorpay pattern demo)
- Real TLS, settlement reports, Route, subscriptions, Smart Collect
- Production-grade HMAC implementation (demo uses simplified signature stand-in)

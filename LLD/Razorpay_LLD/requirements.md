# Razorpay Payment Gateway LLD - Requirements

## Functional Requirements

1. System should allow creating orders with amount in **paise**, currency, and receipt reference.
2. System should generate unique order identifiers with the `order_` prefix.
3. System should store orders in an in-memory registry with initial status `created`.
4. System should reject order creation when amount is not positive or currency is empty.
5. System should support **idempotent order creation** using an optional idempotency key (same key returns the same order).
6. System should allow initiating a payment against an existing order.
7. System should generate unique payment identifiers with the `pay_` prefix.
8. System should support payment methods **UPI**, **CARD**, and **WALLET**.
9. System should validate payment instrument details before initiation (UPI VPA, CARD format, WALLET provider name).
10. System should set payment status to `authorized` on successful initiation and `failed` with a reason on failure (demo simulation).
11. System should reject payment initiation when the parent order is already paid.
12. System should move order status from `created` to `attempted` when payment starts.
13. System should support **idempotent payment initiation** using an optional idempotency key.
14. System should allow capturing an authorized payment for a specified amount in paise.
15. System should reject capture when payment is not `authorized`, or when capture amount is invalid.
16. System should set payment status to `captured`, record captured amount, and mark the parent order as `paid` after successful capture.
17. System should allow **partial or full refunds** on captured payments.
18. System should generate unique refund identifiers with the `rfnd_` prefix and track refund status.
19. System should reject refunds that exceed remaining captured balance or are not eligible.
20. System should accept webhooks with signature verification **before** any internal state change (fail-closed on invalid signature).
21. System should handle webhook events: `payment.authorized`, `payment.captured`, `payment.failed`, and `refund.processed`.
22. System should provide an end-to-end **checkout** flow: validate input → create order → initiate payment → capture payment.
23. System should skip capture when payment initiation fails.
24. System should expose order, payment, and refund lookup by identifier.
25. Merchant-facing operations should go through a single **facade** API (`RazorpayPaymentSystem`).

## Non-Functional Requirements

1. Code should be modular and easy to extend (dedicated services; mock API client swappable for real HTTP).
2. Use design patterns where appropriate:
   - Facade (`RazorpayPaymentSystem`),
   - Template Method (checkout flow),
   - Strategy (payment method validators),
   - Factory (entity IDs, validator creation),
   - Service layer (order, payment, capture, refund, webhook, idempotency).

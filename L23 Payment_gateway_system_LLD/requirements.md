# Payment Gateway LLD - Requirements

## Functional Requirements

1. System should accept a payment request with sender, receiver, amount, and currency.
2. System should support multiple payment gateways (Paytm, Razorpay, PayPal).
3. Each gateway should run its own validation rules.
4. System should process payment via provider-specific banking integration.
5. System should confirm payment after successful initiation.
6. System should support retry logic when payment fails.
7. Client should use a single controller/service API independent of provider internals.
8. Retry behavior should be pluggable via strategy classes in code.
9. System should support **linear retry** — wait a fixed interval between each retry attempt.
10. System should support **exponential back-off** — increase wait time exponentially between retries (e.g. base × 2^attempt).
11. System should support registering a subscription (subscriber, receiver, amount, currency, gateway, billing interval).
12. A billing cycle should charge all active subscriptions through the same gateway payment flow (with retries).
13. Subscriptions should be cancellable.
14. For Paytm UPI payments, the system should show payer and payee **names** along with **source and destination UPI IDs** (which UPI sent to which UPI).
15. Paytm should display the **transaction timestamp** when the payment is confirmed.
16. Paytm should generate a **unique reference number** for each confirmed transaction.
17. For Razorpay payments, the system should show payer and payee **names** along with **source and destination bank account numbers** (which account sent to which account).
18. Razorpay should display the **transaction completion timestamp** when the payment is confirmed.
19. Razorpay should generate a **unique payment ID** for each confirmed transaction.
20. For PayPal payments, the system should show payer and payee **names** along with **source and destination PayPal emails** (which wallet sent to which wallet).
21. PayPal should display the **transaction timestamp** when the payment is confirmed.
22. PayPal should generate a **unique transaction ID** for each confirmed payment.
23. PayPal should validate **international currencies** (USD, EUR, GBP) with amount greater than zero.

## Non-Functional Requirements

1. Code should be modular and easy to extend.
2. Use design patterns where appropriate:
   - Strategy (banking backend behavior, retry backoff),
   - Template Method (gateway flow),
   - Proxy (retry),
   - Factory (gateway creation),
   - Singleton (controller/service/factory instances).

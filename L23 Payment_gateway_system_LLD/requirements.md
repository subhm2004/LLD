# Payment Gateway LLD - Requirements

## Functional Requirements

1. System should accept a payment request with sender, receiver, amount, and currency.
2. System should support multiple payment gateways (Paytm, Razorpay).
3. Each gateway should run its own validation rules.
4. System should process payment via provider-specific banking integration.
5. System should confirm payment after successful initiation.
6. System should support retry logic when payment fails.
7. Client should use a single controller/service API independent of provider internals.

## Non-Functional Requirements

1. Code should be modular and easy to extend.
2. Use design patterns where appropriate:
   - Strategy (banking backend behavior),
   - Template Method (gateway flow),
   - Proxy (retry),
   - Factory (gateway creation),
   - Singleton (controller/service/factory instances).
3. Header files must use `#ifndef / #define / #endif` guards.
4. Follow project conventions:
   - Use `namespace payment_gateway_lld`,
   - Include `<bits/stdc++.h>`,
   - Use `using namespace std;`.

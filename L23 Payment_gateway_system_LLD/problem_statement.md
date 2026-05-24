# Payment Gateway LLD - Problem Statement

Design a payment gateway platform that can route payment requests through multiple providers (for example, Paytm and Razorpay) while keeping the checkout API unified for clients.

> **Razorpay-only lifecycle** (orders, capture, webhooks, refunds): see [`Razorpay_LLD/`](../Razorpay_LLD/).

The system should support:

- A common payment request model (`sender`, `receiver`, `amount`, `currency`).
- Plug-and-play payment gateways with provider-specific validation and processing logic.
- Retry handling for transient failures.
- A factory-driven gateway creation flow.
- A centralized service and controller layer to expose one unified entry point.

The design should be modular and extensible so new gateway providers can be added with minimal changes in existing components.

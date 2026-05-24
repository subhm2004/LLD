# Problem Statement: Razorpay Payment Gateway System

Design an in-memory **merchant payment backend** integrated with **Razorpay only** (not a multi-PSP router).

The system shall support the full Razorpay checkout lifecycle: **create order → initiate payment → capture → webhooks → refund**, with amount in **paise**, **idempotent** create APIs, and **mandatory webhook signature verification** before any state change.

## Goals

- Model real Razorpay concepts (`order_`, `pay_`, `rfnd_` ids, authorize/capture split, partial refunds).
- Keep a clean **service-oriented** design with a single **facade** for external callers.
- Use a **mock** `RazorpayApiClient` (no real HTTP) suitable for LLD interviews.

## Out of scope

Checkout.js / SDK UI, PCI storage, settlement, Route, subscriptions, production HMAC.

## Related work in this repo

| Project | Purpose |
|---------|---------|
| [`requirements.md`](./requirements.md) | Full FR / NFR (ground truth) |
| [`L23 Payment_gateway_system_LLD`](../L23%20Payment_gateway_system_LLD/) | Multi-gateway **design patterns** (Paytm + Razorpay stubs) |

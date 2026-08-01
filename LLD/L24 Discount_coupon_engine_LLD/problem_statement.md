# Problem Statement: Discount & Coupon Engine

Design an e-commerce **discount engine** that applies multiple promotional rules to a shopping cart and computes the final payable amount.

## Functional requirements

1. Maintain a cart with products (name, category, price) and quantities.
2. Support discount calculation strategies: **flat**, **percentage**, and **percentage with cap**.
3. Register multiple coupons in a chain and apply them in order.
4. Coupon types:
   - **Seasonal** — category-based percentage off eligible items
   - **Loyalty** — percentage off for loyalty members
   - **Bulk purchase** — flat off when cart crosses a threshold
   - **Banking** — bank + minimum spend, percent off with maximum cap
5. List applicable coupons for a cart before applying.
6. Thread-safe coupon registry (concurrent registration / lookup).

## Out of scope

- Payment capture
- Inventory / stock
- Persistent coupon database
- User authentication

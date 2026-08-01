# Problem Statement: E-commerce Cart + Checkout

Design an in-memory e-commerce checkout flow where users browse products, manage a cart, apply coupons, reserve inventory, pay via multiple rails, and receive order confirmation.

## Functional requirements

1. Register users and delivery addresses.
2. Maintain product catalog with per-SKU stock.
3. Cart operations — add item, update quantity, remove item, view cart.
4. Validate stock before checkout and **reserve inventory** during payment.
5. Apply coupon codes with minimum cart value and expiry rules.
6. Compute price breakup — subtotal, discount, shipping, payable amount.
7. Checkout via pluggable payment methods — UPI, credit card, COD.
8. On payment success — confirm reservation, create order, clear cart.
9. On payment failure — release reservation, mark order failed.
10. Idempotent checkout using optional `clientRequestId`.
11. Send order confirmation notification (demo console output).

## Out of scope

- Real payment gateway / PCI compliance
- Persistent database
- Multi-warehouse inventory
- Returns / refunds workflow
- Recommendation engine

## Assumptions

- INR only; amounts in `double` (demo).
- Free shipping when payable subtotal (after discount) >= Rs 999; else flat Rs 49 shipping.
- COD orders are confirmed with payment status `PENDING` (pay on delivery).
- Card ending with `0000` is declined in demo.

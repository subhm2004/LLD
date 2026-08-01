# Requirements — E-commerce Cart + Checkout

## Functional

- [x] User + address registration
- [x] Product catalog with stock quantity
- [x] Add / update / remove cart items
- [x] Inventory reservation during checkout
- [x] Coupon validation (min cart, expiry)
- [x] Price breakup (subtotal, discount, shipping, payable)
- [x] Payment via Strategy — UPI, card, COD
- [x] Order creation with status + payment status
- [x] Rollback inventory on payment failure
- [x] Idempotent checkout via `clientRequestId`
- [x] Console notification on confirm

## Non-functional (demo scope)

- In-memory storage only
- Single-process, no distributed locks
- C++17, `./compile.sh` build

## Design patterns

| Pattern | Where |
|---------|--------|
| **Facade** | `EcommerceCheckoutSystem` |
| **Strategy** | Payment rails, discount calculation |
| **Factory** | `PaymentStrategyFactory` |
| **Service layer** | Cart, Inventory, Checkout, Pricing, Coupon |

## Key classes

`Product`, `Cart`, `CartItem`, `Order`, `Coupon`, `InventoryService`, `CheckoutService`, `PaymentService`

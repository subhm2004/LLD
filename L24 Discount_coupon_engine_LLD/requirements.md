# Requirements: Discount_coupon_engine_LLD

## Functional

| Component | Responsibility |
|-----------|----------------|
| `Cart` | Add items, track original/current totals, loyalty & bank metadata |
| `CouponManager` | Register coupons, list applicable, apply chain |
| `IDiscountStrategy` | Compute discount amount for a base value |
| Concrete coupons | Applicability rules + strategy delegation |

## Non-functional

- C++17, header-only modular layout
- Namespace: `discount_coupon_lld`
- Thread-safe `CouponManager` via `std::mutex`
- In-memory demo (no persistence)

## Design patterns

| Pattern | Where |
|---------|-------|
| **Strategy** | `IDiscountStrategy` — flat / percent / capped percent |
| **Chain of Responsibility** | `Coupon` linked list — sequential apply |
| **Singleton** | `DiscountStrategyManager`, `CouponManager` (Meyers) |
| **Factory** | `DiscountStrategyManager::createStrategy` |

## Legacy reference

Original monolithic implementation preserved at [`C++ Code/DiscountCoupon.cpp`](./C%20%2B%2B%20Code/DiscountCoupon.cpp).

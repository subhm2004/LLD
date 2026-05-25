# Discount Coupon Engine LLD - Requirements

## Functional Requirements

1. System should maintain a cart with products (name, category, unit price) and quantities.
2. Cart should track **original total** and **current total** after discounts are applied.
3. Cart should support loyalty-member flag and payment bank metadata for coupon eligibility.
4. System should allow **registering new coupons at runtime** via `CouponManager::registerCoupon`.
5. System should support **cart-level** discounts (applied on full / current cart total) and **category-level** discounts (applied only on eligible items in a category).
6. System should support discount math via pluggable strategies: **flat**, **percentage**, and **percentage with cap**.
7. System should register multiple coupons in a **chain** and apply them in registration order.
8. **Seasonal offer** — category-level percentage off items in a given category.
9. **Loyalty discount** — cart-level percentage off current total when the user is a loyalty member.
10. **Bulk purchase discount** — cart-level flat off when original cart total crosses a minimum threshold.
11. **Banking coupon** — cart-level percentage off with maximum cap when payment bank matches and minimum spend is met.
12. System should list all **applicable** coupon names for a cart before applying discounts.
13. System should apply the coupon chain and return the **final payable amount** (current total not below zero).
14. Coupons that are not applicable for a cart should be skipped without changing the total.
15. Coupons should support **combinable** (stack on updated total) or **exclusive** (`isCombinable() == false` stops the rest of the chain).

## Non-Functional Requirements

1. Code should be modular and easy to extend (header-based layout, namespace `discount_coupon_lld`).
2. Coupon registry should be **thread-safe** for concurrent registration and lookup (`std::mutex`).
3. Demo runs in-memory with no persistence (no DB or payment integration).
4. Use design patterns where appropriate:
   - Strategy (`IDiscountStrategy` — flat / percent / capped percent),
   - Chain of Responsibility (`Coupon` linked list, sequential apply),
   - Singleton (`CouponManager`, `DiscountStrategyManager`),
   - Factory (`DiscountStrategyManager::createStrategy`).

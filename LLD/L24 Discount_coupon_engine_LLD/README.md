# Discount & Coupon Engine LLD

E-commerce discount engine — **Strategy** (flat / percent / capped), **Chain of Responsibility** (coupon chain), thread-safe **CouponManager**.

> Original single-file version kept in [`C++ Original Code/DiscountCoupon.cpp`](./C%20%2B%2B%20Original%20Code/DiscountCoupon.cpp).

**Full file map:** [`PROJECT_STRUCTURE.md`](./PROJECT_STRUCTURE.md)

---

## Folder Structure

```
L24 Discount_coupon_engine_LLD/
├── C++ Code/                    # Legacy monolithic reference (unchanged)
│   └── DiscountCoupon.cpp
├── core/
│   └── CouponManager.h
├── coupons/
│   ├── Coupon.h
│   ├── SeasonalOffer.h
│   ├── LoyaltyDiscount.h
│   ├── BulkPurchaseDiscount.h
│   └── BankingCoupon.h
├── enums/
│   └── StrategyType.h
├── factories/
│   └── DiscountStrategyManager.h
├── models/
│   ├── Product.h
│   ├── CartItem.h
│   └── Cart.h
├── strategies/
│   ├── IDiscountStrategy.h
│   ├── FlatDiscountStrategy.h
│   ├── PercentageDiscountStrategy.h
│   └── PercentageWithCapStrategy.h
├── compile.sh
├── main.cpp
├── problem_statement.md
└── requirements.md
```

---

## Build & Run

```bash
./compile.sh
./discount_coupon_app
```

---

## Patterns

| Pattern | Classes |
|---------|---------|
| Strategy | `IDiscountStrategy`, `FlatDiscountStrategy`, … |
| Chain of Responsibility | `Coupon` → `SeasonalOffer` → `LoyaltyDiscount` → … |
| Singleton | `CouponManager`, `DiscountStrategyManager` |

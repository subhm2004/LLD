# L24 Discount Coupon Engine — Project Structure

> Legacy reference (unchanged): [`C++ Original Code/`](./C%20%2B%2B%20Original%20Code/)

```
L24 Discount_coupon_engine_LLD/
│
├── main.cpp                          # Demo: register coupons, build cart, apply chain
├── compile.sh                        # Build: g++ -std=c++17 main.cpp → discount_coupon_app
├── requirements.md                   # Functional + non-functional requirements
├── problem_statement.md              # High-level problem description
├── PROJECT_STRUCTURE.md              # This file — folder/file map
├── README.md                         # Overview, patterns, quick start
│
├── core/
│   └── CouponManager.h               # Singleton registry, thread-safe chain, applyAll / getApplicable
│
├── coupons/
│   ├── Coupon.h                      # CoR base: linked list, isCombinable, applyDiscount
│   ├── SeasonalOffer.h               # Category-level % discount
│   ├── LoyaltyDiscount.h             # Cart-level % (loyalty member)
│   ├── BulkPurchaseDiscount.h        # Cart-level flat (min cart total)
│   └── BankingCoupon.h               # Cart-level % with cap, exclusive (non-combinable)
│
├── strategies/
│   ├── IDiscountStrategy.h           # Strategy interface: calculate(amount)
│   ├── FlatDiscountStrategy.h        # Fixed rupee off
│   ├── PercentageDiscountStrategy.h  # Percentage off base
│   └── PercentageWithCapStrategy.h   # Percentage with maximum cap
│
├── factories/
│   └── DiscountStrategyManager.h     # Singleton factory: createStrategy(FLAT / PERCENT / PERCENT_WITH_CAP)
│
├── models/
│   ├── Product.h                     # name, category, price
│   ├── CartItem.h                    # product + quantity, line total
│   └── Cart.h                        # items, original/current total, loyalty, payment bank
│
├── enums/
│   └── StrategyType.h                # FLAT, PERCENT, PERCENT_WITH_CAP
│
└── C++ Original Code/                # Do not modify — monolithic reference only
    ├── DiscountCoupon.cpp
    ├── Requirements.png
    ├── README.md
    └── Notes/
        ├── Requirements.jpeg
        └── UML.jpeg
```

## File roles (quick reference)

| Folder / file | Responsibility |
|---------------|----------------|
| `main.cpp` | Entry point and demo flow |
| `core/CouponManager.h` | Register coupons at runtime, list applicable, run chain |
| `coupons/Coupon.h` | Chain of Responsibility + combinable / exclusive flag |
| `coupons/*.h` | Concrete coupon types (rules + discount calculation) |
| `strategies/*.h` | Discount math (Strategy pattern) |
| `factories/DiscountStrategyManager.h` | Create strategy instances |
| `models/*.h` | Cart, product, line items |
| `enums/StrategyType.h` | Strategy type enum |

## Build & run

```bash
./compile.sh
./discount_coupon_app
```

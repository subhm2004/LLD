# Problem Statement: Stock Exchange (Order Matching)

Design an in-memory **stock exchange** with per-symbol **order books** and **price-time priority** matching.

## Functional requirements

1. List **symbols** (e.g. RELIANCE, TCS).
2. **Place orders**: LIMIT and MARKET, BUY and SELL.
3. **Match** incoming orders against resting opposite-side orders (best price first, FIFO at same price).
4. Support **partial fills**; remainder rests on the book for LIMIT orders.
5. **Cancel** open/partial resting orders.
6. View **order book** snapshot (top bid/ask levels).
7. Record and query executed **trades**.

## Assumptions

- Single exchange process; in-memory storage.
- Trade price = **resting (maker) order** price.
- No short-selling / margin checks (interview scope).
- `double` prices; demo quantities are integers.

## Out of scope

- Persistence, distributed matching, regulatory reporting
- Stop-loss, IOC/FOK, derivatives

# Requirements: Stock Exchange LLD

## Functional

| ID | Requirement |
|----|-------------|
| FR-1 | Register users and list symbols |
| FR-2 | Place LIMIT and MARKET buy/sell orders |
| FR-3 | Price-time priority matching (best price, FIFO per level) |
| FR-4 | Partial fills; remainder rests on book for LIMIT |
| FR-5 | Cancel open/partial orders |
| FR-6 | Order book snapshot (bids/asks) |
| FR-7 | Trade ledger per symbol |

## Non-functional

| ID | Requirement |
|----|-------------|
| NFR-1 | C++17, `compile.sh`, in-memory |
| NFR-2 | Per-symbol mutex in matching engine |
| NFR-3 | Trade at resting (maker) price |

## Patterns

| Pattern | Use |
|---------|-----|
| **Facade** | `StockExchangeSystem` |
| **Factory** | `OrderFactory` |
| **Service layer** | Order book, matching, registry, ledger |

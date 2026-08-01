# Stock Exchange LLD (C++17)

In-memory **limit order book** with price-time priority matching.

## Build & run

```bash
cd Stock_Exchange_LLD
chmod +x compile.sh
./compile.sh
./stock_exchange_app
```

Or:

```bash
g++ -std=c++17 -pthread main.cpp -o stock_exchange_app && ./stock_exchange_app
```

## Architecture

```
Stock_Exchange_LLD/
├── core/StockExchangeSystem.h    # Facade
├── services/
│   ├── OrderBookService.h        # Bid/ask price levels
│   ├── MatchingEngineService.h   # Price-time matching
│   ├── OrderRegistryService.h
│   └── TradeLedgerService.h
├── models/ Order, Trade, Symbol
├── factories/OrderFactory.h
└── main.cpp
```

## Patterns

| Pattern | Use |
|---------|-----|
| **Facade** | `StockExchangeSystem` |
| **Factory** | `OrderFactory` |
| **Service layer** | Book, matching, registry, ledger |

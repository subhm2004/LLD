# Design Patterns Used in Stock Exchange LLD

Stock Exchange LLD system me order creation, matching algorithms, trade logs, aur user ledger coordination ko decoupled rakhne ke liye niche diye gaye patterns ka use kiya gaya hai:

---

## 1. Facade Design Pattern

### Concept
Facade pattern subsystem component modules (jaise order matching, transaction logs auditing, order book tracking) ki complex internal API interactions ko single simplified controller wrapper class me wrap kar deta hai.

### Implementation
- [StockExchangeSystem](file:///Users/shubham/Desktop/LLD/Stock_Exchange_LLD/core/StockExchangeSystem.h) class main facade implementation hai.
- Client applications direct multiple standalone modules (jaise bids/asks ledger [OrderBookService](file:///Users/shubham/Desktop/LLD/Stock_Exchange_LLD/services/OrderBookService.h), matching engine [MatchingEngineService](file:///Users/shubham/Desktop/LLD/Stock_Exchange_LLD/services/MatchingEngineService.h), aur trade ledger [TradeLedgerService](file:///Users/shubham/Desktop/LLD/Stock_Exchange_LLD/services/TradeLedgerService.h)) ko directly coordinate nahi karti.
- Client system facade ke simple methods call karta hai:
  - `placeOrder(...)`
  - `registerMember(...)`
  - `printOrderBook(...)`

---

## 2. Factory Design Pattern

### Concept
Factory design pattern concrete class instantiation logic ko centralize aur abstract karta hai.

### Implementation
- [OrderFactory](file:///Users/shubham/Desktop/LLD/Stock_Exchange_LLD/factories/OrderFactory.h) class specific parameters check ke base par concrete order models instanciate karti hai (`MarketOrder`, `LimitOrder`, etc.).
- Future me agar dynamic parameters validations add karne hon, toh factory method central boundary boundary verification control block ki tarah behave karegi.

---

## 3. Delegation Pattern & Single Responsibility Principle (SRP)

### Concept
System modules ko independent and specialized service levels me divide kiya gaya hai.

### Implementation
- **Order Matching**: Bid/Ask queues validation matching calculations [MatchingEngineService](file:///Users/shubham/Desktop/LLD/Stock_Exchange_LLD/services/MatchingEngineService.h) handle karti hai.
- **Trades Registry**: Completed executions auditing records management [TradeLedgerService](file:///Users/shubham/Desktop/LLD/Stock_Exchange_LLD/services/TradeLedgerService.h) manage karti hai.

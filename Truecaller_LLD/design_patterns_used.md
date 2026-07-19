# Design Patterns Used in Truecaller LLD

Truecaller LLD system me contacts search directories, spam reports scoring algorithms, aur global user profiles management ko cleanly decoupling rakhne ke liye niche diye gaye patterns ka use kiya gaya hai:

---

## 1. Facade Design Pattern

### Concept
Facade pattern subsystem key multiple independent helper classes (jaise contacts search, spam records logger, search history auditing, and profile validations) ko single unified entry point class me wrap kar deta hai.

### Implementation
- [TruecallerSystem](file:///Users/shubham/Desktop/LLD/Truecaller_LLD/core/TruecallerSystem.h) class main facade implementation hai.
- Client applications direct multiple modules (jaise contacts indexing database, spam reporting ledger, blacklist counts, and users search logs) se separately talk nahi karti.
- Client directly facade levels methods call karta hai:
  - `registerUser(...)`
  - `searchByName(...)`
  - `searchByPhoneNumber(...)`
  - `reportSpam(...)`

---

## 2. Strategy Design Pattern

### Concept
Strategy design pattern interchangeable processing algorithms ko interfaces me group karta hai taaki runtime par dynamic parameters validation changes seamlessly implement ho sakein.

### Implementation
- [ISpamScoringStrategy](file:///Users/shubham/Desktop/LLD/Truecaller_LLD/strategies/ISpamScoringStrategy.h) abstract base strategy interface hai.
- [DefaultSpamScoringStrategy](file:///Users/shubham/Desktop/LLD/Truecaller_LLD/strategies/DefaultSpamScoringStrategy.h) is interface ka concrete implementation strategy hai jo spam reports count, user verified status, aur verification metrics evaluate karke threshold score calculate karta hai.
- Future me advanced heuristics pricing or AI spam checking models integrate karne par systems configuration change dynamically ho sakti hai.

---

## 3. Delegation Pattern & Single Responsibility Principle (SRP)

### Concept
SRP follow karte hue specialized operations ko dynamic separated domain services me delegate kiya jata hai.

### Implementation
- **Contacts lookup**: Name / Number based indexes search logic [SearchService](file:///Users/shubham/Desktop/LLD/Truecaller_LLD/services/SearchService.h) handles karta hai.
- **Reporting logs**: Spam report analytics logging [SpamService](file:///Users/shubham/Desktop/LLD/Truecaller_LLD/services/SpamService.h) check karta hai.

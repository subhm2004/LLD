# Design Patterns Used in URL Shortener LLD

URL Shortener LLD system me short code generation algorithms, mappings indexing, redirect validation, aur click analytics auditing ko simple aur decoupled rakhne ke liye niche diye gaye patterns ka use kiya gaya hai:

---

## 1. Facade Design Pattern

### Concept
Facade pattern subsystems ki multiple business components (jaise validation engine, hash code generator algorithms, click counters auditor, and dynamic mappings database tables) ko single unified entry point wrapper class me hide kar deta hai.

### Implementation
- [UrlShortnerService](file:///Users/shubham/Desktop/LLD/URL_Shortner_LLD/services/UrlShortnerService.h) class main facade implementation hai.
- Client application (jaise [main.cpp](file:///Users/shubham/Desktop/LLD/URL_Shortner_LLD/main.cpp)) encoding base62 utilities, logs click counter, index lookups, or validation steps ko direct separate manage nahi karti.
- Client simply facade class ke clean APIs call karta hai:
  - `shortenUrl(...)`
  - `resolveShortUrl(...)`
  - `printStats(...)`

---

## 2. Delegation Pattern & Single Responsibility Principle (SRP)

### Concept
SRP implementation ko satisfy karne ke liye logical conversion calculations ko dynamic static class algorithm block me delegate kiya gaya hai.

### Implementation
- **Unique Hash Code Generation**: URL shortener key conversion logic ko [Base62Encoder](file:///Users/shubham/Desktop/LLD/URL_Shortner_LLD/utils/Base62Encoder.h) helper utility class me delegate kiya gaya hai.
- Is utility class ka focus sirf integer value database counters ko stable Base62 string layout representation me encode karna hai, jisse core service implementation cleanup rehta hai.

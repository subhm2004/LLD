# Design Patterns Used in Logger LLD

Logger LLD system me thread-safe global access, log levels sequential processing, log formats, aur target outputs ko highly modular rakhne ke liye niche diye gaye design patterns ka use kiya gaya hai:

---

## 1. Singleton Design Pattern

### Concept
Singleton pattern ye ensure karta hai ki application memory me class ka ek hi dynamic instance create ho, aur use globally access karne ke liye ek global access point provide kiya jaye.

### Implementation
- [Logger](file:///Users/shubham/Desktop/LLD/Logger_LLD/Logger.h) class is pattern ka direct implementation hai (Meyers Singleton approach).
- Constructor ko private rakha gaya hai aur copy constructor/assignment operators (`delete`) disable kiye gaye hain.
- Static method `getInstance()` block levels par single unique thread-safe instance lazy initialize karke reference return karta hai.

---

## 2. Chain of Responsibility Design Pattern

### Concept
Chain of Responsibility pattern requests ko processing handlers ki ek chain me pass karta hai. Har handler incoming parameters aur criteria ke base par processing logic decide karta hai.

### Implementation
- [LogHandler](file:///Users/shubham/Desktop/LLD/Logger_LLD/handlers/LogHandler.h) base handler interface hai jo link routing handle karta hai.
- Logs levels handlers is sequence me linked hain:
  `InfoHandler` $\rightarrow$ `DebugHandler` $\rightarrow$ `WarnHandler` $\rightarrow$ `ErrorHandler` $\rightarrow$ `FatalHandler`
- Jab koi log message (jaise `LogLevel::ERROR`) submit hota hai, toh har handler check karta hai ki kya log message level handler ke capability bounds me fall karta hai (level $\ge$ handler target level). Agar ha, toh message process hota hai, aur request chain me aage flow ho jati hai.

---

## 3. Strategy Design Pattern

### Concept
Strategy pattern client context me complex processing algorithms (jaise formatting or outputs distribution) ko separated interchangeable classes interfaces me divide karta hai.

### Implementation
Logger system me do strategies work karti hain:
1. **Formatting Strategy**:
   - [LogFormatter](file:///Users/shubham/Desktop/LLD/Logger_LLD/formatter/LogFormatter.h) strategy interface hai.
   - [PlainTextFormatter](file:///Users/shubham/Desktop/LLD/Logger_LLD/formatter/PlainTextFormatter.h) aur [JsonFormatter](file:///Users/shubham/Desktop/LLD/Logger_LLD/formatter/JsonFormatter.h) concrete strategies hain jo string convert layout structures return karti hain.
2. **Appender/Destination Strategy**:
   - [LogAppender](file:///Users/shubham/Desktop/LLD/Logger_LLD/appenders/LogAppender.h) strategy interface hai.
   - [ConsoleAppender](file:///Users/shubham/Desktop/LLD/Logger_LLD/appenders/ConsoleAppender.h) aur [FileAppender](file:///Users/shubham/Desktop/LLD/Logger_LLD/appenders/FileAppender.h) concrete write strategies hain jo destination targets par streams log karti hain.

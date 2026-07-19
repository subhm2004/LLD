# Design Patterns Used in ATM LLD

ATM LLD system me code structure ko clean, maintainable aur decoupled rakhne ke liye niche diye gaye patterns aur architecture techniques ka use kiya gaya hai:

---

## 1. Facade Design Pattern

### Concept
Facade pattern complex systems ke multiple sub-systems ke aage ek simplified public interface provide karta hai. Isse client code ko subsystem components se directly link/interact nahi karna padta.

### Implementation
- [ATMSystem](file:///Users/shubham/Desktop/LLD/ATM_LLD/core/ATMSystem.h) class is system me main **Facade** ki tarah kaam karti hai.
- Client code (jaise [main.cpp](file:///Users/shubham/Desktop/LLD/ATM_LLD/main.cpp)) bank account balance check karne ya cash withdrawal ke liye sirf facade ke simple methods call karta hai:
  - `atm->login(...)`
  - `atm->checkBalance(...)`
  - `atm->withdrawCash(...)`
- Internal complex operations (jaise `AuthenticationService` se pin match validation, `CashDispenser` se vault notes deduct calculations, aur `BankAccount` balance debit values update) complete wrap/hide rehte hain client se.

---

## 2. Delegation Pattern & Single Responsibility Principle (SRP)

### Concept
SRP ke mutabik har class ya unit ka sirf ek hi clear reason-to-change hona chahiye. Delegation ke through responsibility specific utility services ko di jati hai.

### Implementation
- **Authentication**: Card validation aur PIN checking ka core kaam `ATMSystem` khud nahi karta, balki use [AuthenticationService](file:///Users/shubham/Desktop/LLD/ATM_LLD/services/AuthenticationService.h) ko delegate kar deta hai.
- **Cash Dispensation**: Denominations calculation aur cash vault management ka logic [CashDispenser](file:///Users/shubham/Desktop/LLD/ATM_LLD/services/CashDispenser.h) handle karta hai.

---

## 3. Session State Management (Simplified State Pattern)

### Concept
State management user session transitions (jaise login, checked state, transaction processing, logout) ko secure conditions me control karta hai.

### Implementation
- `ATMSystem` internally user session ko track karne ke liye state identifier variable `currentCardNumber_` ko use karta hai.
- Jab user login successfully complete karta hai, tabhi active session set hota hai. Har balance inquiry ya cash withdrawal request se pehle verification endpoint `getCurrentAccountOrThrow` run hota hai jo session validity verify karta hai. 
- Logout call karne par state reference safely clear kar di jati hai.

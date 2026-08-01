# Design Patterns Used in Uber LLD

Uber LLD system me ride matching algorithms, dynamic pricing calculation, transaction processing, aur driver matching subsystems ko decoupled aur maintainable rakhne ke liye niche diye gaye patterns ka use kiya gaya hai:

---

## 1. Facade Design Pattern

### Concept
Facade pattern subsystems ki multiple component services (jaise matching service, pricing calculation, dynamic transaction processors, aur verification SMS gateways) ko single simplified public API entrance class me wrap kar deta hai.

### Implementation
- [UberSystem](file:///Users/shubham/Desktop/LLD/Uber_LLD/core/UberSystem.h) class main facade interface hai.
- Client applications direct multiple helper modules (jaise nearest driver searcher [DriverMatchingService](file:///Users/shubham/Desktop/LLD/Uber_LLD/services/DriverMatchingService.h), pricing formulas [FareService](file:///Users/shubham/Desktop/LLD/Uber_LLD/services/FareService.h), trip verification keys [OtpService](file:///Users/shubham/Desktop/LLD/Uber_LLD/services/OtpService.h), and settlements [PaymentService](file:///Users/shubham/Desktop/LLD/Uber_LLD/services/PaymentService.h)) ko individually invoke nahi karti.
- Client directly facade methods call karta hai:
  - `requestTrip(...)`
  - `startTrip(...)`
  - `completeTrip(...)`

---

## 2. Delegation Pattern & Single Responsibility Principle (SRP)

### Concept
SRP follow karte hue system ke primary business domains (jaise matchmaking, billing, aur security) ko independent dedicated micro-services layers me delegate kiya jata hai.

### Implementation
- **Driver Matchmaking**: Pick-up locations and coordinates ke criteria par closest driver select karne ka logic [DriverMatchingService](file:///Users/shubham/Desktop/LLD/Uber_LLD/services/DriverMatchingService.h) me delegate kiya gaya hai.
- **Fare Calculations**: Distance and time values ke dynamic price calculations [FareService](file:///Users/shubham/Desktop/LLD/Uber_LLD/services/FareService.h) handles karta hai.
- **OTP Verification**: Security transaction pin numbers management [OtpService](file:///Users/shubham/Desktop/LLD/Uber_LLD/services/OtpService.h) manage karti hai.

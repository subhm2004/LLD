# Design Patterns Used in Ride Sharing App LLD

Ride Sharing LLD system me drivers matching, pricing calculation, aur rides lifecycle coordination ko clean rakhne ke liye niche diye gaye design patterns aur best practices use kiye gaye hain:

---

## 1. Facade Design Pattern

### Concept
Facade pattern sub-systems ke multiple interfaces ko single unified entry point class me wrap kar deta hai, jisse systems integration client code ke liye bahut simple ho jata hai.

### Implementation
- [RideSharingService](file:///Users/shubham/Desktop/LLD/Ride_sharing_app_LLD/core/RideSharingService.h) is system me **Facade** ki tarah kaam karti hai.
- Client application (jaise [main.cpp](file:///Users/shubham/Desktop/LLD/Ride_sharing_app_LLD/main.cpp)) riders registry, drivers location tracking, nearest driver searching, fare calculation (`PricingService`), aur ride details manage karne ke liye kisi separate module se direct contact nahi karti. 
- Client direct `RideSharingService` ke simplified methods call karta hai:
  - `registerRider(...)`
  - `registerDriver(...)`
  - `requestRide(...)`
  - `startRide(...)`
  - `completeRide(...)`

---

## 2. Delegation Pattern & Single Responsibility Principle (SRP)

### Concept
SRP follow karte hue, core logical calculations (jaise fare calculations) ko dedicated service algorithms classes me delegate kiya jata hai.

### Implementation
- **Pricing Strategy**: Fare pricing logic ko [PricingService](file:///Users/shubham/Desktop/LLD/Ride_sharing_app_LLD/services/PricingService.h) class me delegate kiya gaya hai.
- Isse agar aage chal kar hume distance calculation formula me changes karne hon ya *Surge Pricing Strategy* introduce karni ho, toh hum bina main ride booking class ko disturb kiye changes kar sakte hain (Open-Closed Principle mapping).

---

## 3. State Machine (Driver & Ride Lifecycle)

### Concept
System variables lifecycle transitions ko explicit predefined enum states me lock karke runtime validation errors prevent karta hai.

### Implementation
- **Ride States**: Ride lifecycle [RideStatus](file:///Users/shubham/Desktop/LLD/Ride_sharing_app_LLD/enums/RideStatus.h) enum states (`ACCEPTED` $\rightarrow$ `STARTED` $\rightarrow$ `COMPLETED`) ke standard transition flow ko follow karta hai.
- **Driver States**: Driver status [DriverStatus](file:///Users/shubham/Desktop/LLD/Ride_sharing_app_LLD/enums/DriverStatus.h) (`AVAILABLE` / `ON_RIDE`) update flow ride status transitions ke sync me coordinate hota hai.

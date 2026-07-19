# Design Patterns Used in OYO Hotel Booking LLD

OYO Hotel Booking LLD system me dynamic pricing logic, rooms availability checks, aur check-in/check-out workflow controls ko manage karne ke liye niche diye gaye patterns aur principles ka use kiya gaya hai:

---

## 1. Strategy Design Pattern

### Concept
Strategy pattern algorithms family ko define karke unhe dynamic interfaces me encapsulate karta hai taaki runtime par behaviors ko switch kiya ja sake.

### Implementation
- [IPricingStrategy](file:///Users/shubham/Desktop/LLD/OYO_Hotel_Booking_LLD/strategies/IPricingStrategy.h) abstract base strategy interface hai.
- Is interface ko do pricing policies/strategies implement karti hain:
  - [StandardPricingStrategy](file:///Users/shubham/Desktop/LLD/OYO_Hotel_Booking_LLD/strategies/StandardPricingStrategy.h) (Normal default flat night rates)
  - [WeekendSurchargePricingStrategy](file:///Users/shubham/Desktop/LLD/OYO_Hotel_Booking_LLD/strategies/WeekendSurchargePricingStrategy.h) (Saturday aur Sunday bookings par 1.25x surge charges)
- [PricingService](file:///Users/shubham/Desktop/LLD/OYO_Hotel_Booking_LLD/services/PricingService.h) pricing strategy pointer hold karti hai, jise `useWeekendPricing` method call karke runtime par dynamically switch kiya ja sakta hai (Strategy switching).

---

## 2. Facade Design Pattern

### Concept
Facade pattern subsystems ki multiple modular APIs ko wrap karke client ke samne ek single simplified unified wrapper class provide karta hai.

### Implementation
- [OYOHotelBookingSystem](file:///Users/shubham/Desktop/LLD/OYO_Hotel_Booking_LLD/core/OYOHotelBookingSystem.h) class main **Facade** interface hai.
- Client applications direct multiple utility services (jaise hotel ratings sorting, booking availability check filters, pricing calculations, notifications triggers) ko individually coordinate nahi karti.
- Client sirf Facade class ke standard interfaces call karta hai:
  - `createBooking(...)`
  - `cancelBooking(...)`
  - `checkIn(...)`
  - `checkOut(...)`

---

## 3. State Management (Room & Booking Lifecycle States)

### Concept
System entities states transitions ko predefined rules (state machine) ke synchronization me bind kiya jata hai taaki dynamic workflow errors check safely execute hon.

### Implementation
- **Booking Lifecycle**: Booking status (`CONFIRMED` $\rightarrow$ `CHECKED_IN` $\rightarrow$ `CHECKED_OUT`) transition boundaries explicitly guarded hain. (e.g. check-in se pehle checkout lock exception throws, check-in ke baad cancellation block exceptions checking).
- **Room Lifecycle**: Room status updates (`RoomStatus::AVAILABLE` / `RoomStatus::BOOKED`) automatically bookings confirm ya cancellation triggers par map updates synchronize hoti hain.

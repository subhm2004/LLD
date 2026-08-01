# Design Patterns Used in Movie Ticket Booking System LLD

Movie Ticket Booking LLD system me dynamic pricing calculation, booking workflows coordination, aur cinema shows management ko modular rakhne ke liye niche diye gaye patterns ka use kiya gaya hai:

---

## 1. Facade Design Pattern

### Concept
Facade pattern subsystems ki multiple management logic classes (jaise catalog search, seat inventory lock, payment gateway, transaction logs) ko wrap karke client applications ke samne ek single simple public interface provide karta hai.

### Implementation
- `MovieTicketSystem` (core/ facade entry point) main facade ki tarah kaam karti hai.
- Client applications direct multiple managers (jaise shows schedule database [ShowManager](file:///Users/shubham/Desktop/LLD/Movie_Ticket_Booking_System/managers/ShowManager.h), booking ledger [BookingManager](file:///Users/shubham/Desktop/LLD/Movie_Ticket_Booking_System/managers/BookingManager.h), aur category lookups [CatalogManager](file:///Users/shubham/Desktop/LLD/Movie_Ticket_Booking_System/managers/CatalogManager.h)) se direct coordinate nahi karti.
- Client system facade ke simple methods call karta hai:
  - `searchMovies(...)`
  - `getAvailableSeats(...)`
  - `createBooking(...)`

---

## 2. Strategy Design Pattern

### Concept
Strategy design pattern context class me complex algorithms and policies (jaise ticket dynamic pricing) ko standard interchangeable strategy classes me abstract karta hai.

### Implementation
- [PricingStrategy](file:///Users/shubham/Desktop/LLD/Movie_Ticket_Booking_System/strategies/PricingStrategy.h) abstract base pricing strategy interface hai.
- [StandardPricingStrategy](file:///Users/shubham/Desktop/LLD/Movie_Ticket_Booking_System/strategies/StandardPricingStrategy.h) is interface ka concrete implementation hai jo seat category (Gold, Premium, Normal) aur show timing ke according pricing calculate karta hai.
- Future me *Peak Hour Surcharges* ya *Discount Coupon Pricing* dynamically switch karne par system flow simple rehta hai.

---

## 3. State Management & Resource Locking (Seat Allocation)

### Concept
System variables lifecycle transitions ko explicit state machine me maintain karta hai taaki double booking conflict issues check verify ho sakein.

### Implementation
- **Seat Status**: Seats status (`AVAILABLE` / `BOOKED`) dynamic updates update flows ko coordinate karti hai.
- **Booking Status**: Bookings lifecycle checks status changes track karti hain.

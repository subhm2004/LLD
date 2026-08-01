# Requirements: OYO_Hotel_Booking_LLD

## Functional

| API | Description |
|-----|-------------|
| `registerGuest` | Add guest |
| `addHotel` / `addRoom` | Inventory |
| `searchHotelsByCity` | Filter + sort by rating |
| `getAvailableRooms` | Date-range availability |
| `createBooking` | Reserve room, compute price |
| `cancelBooking` | Release room |
| `checkIn` / `checkOut` | Stay lifecycle |

## Non-functional

- C++17, modular headers
- In-memory `unordered_map` storage
- Date model: integer **days** (simplified calendar)
- Overlap rule: `[checkIn, checkOut)` half-open interval

## Design patterns

| Pattern | Where |
|---------|-------|
| **Facade** | `OYOHotelBookingSystem` |
| **Strategy** | `IPricingStrategy` — standard vs weekend |
| **Service** | `AvailabilityService`, `PricingService`, `NotificationService` |

## Booking states

`PENDING` → `CONFIRMED` → `CHECKED_IN` → `CHECKED_OUT`  
`CANCELLED` from `CONFIRMED` only (before check-in)

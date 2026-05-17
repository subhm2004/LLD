# OYO_Hotel_Booking_LLD

OYO-style **hotel booking** — search by city, room availability, booking lifecycle, Strategy-based pricing.

> **UML diagrams:** [Class + Sequence — Section 21](../SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md#21-oyo-hotel-booking)

## Structure

```
OYO_Hotel_Booking_LLD/
├── core/OYOHotelBookingSystem.h   # Facade
├── models/                        # Hotel, Room, Guest, Booking
├── services/                      # Availability, Pricing, Notification
├── strategies/                    # Standard + Weekend surcharge
├── enums/
├── main.cpp
└── compile.sh
```

## Patterns

- **Facade** — `OYOHotelBookingSystem`
- **Strategy** — `IPricingStrategy`
- **Service layer** — availability & pricing

## Build & run

```bash
cd OYO_Hotel_Booking_LLD
./compile.sh
./oyo_hotel_app
```

## Demo flow

1. Register guests, add hotels & rooms  
2. Search Jaipur, list available rooms  
3. Book → overlap fails → weekend pricing → check-in/out → cancel  

## Extensions

- Coupon / loyalty discounts (Decorator)
- Payment service + refund on cancel
- Room inventory count per type (not 1:1 room entity)
- Real date class (`chrono`) instead of int days

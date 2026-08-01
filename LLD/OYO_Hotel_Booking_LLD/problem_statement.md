# Problem Statement: OYO-Style Hotel Booking System

Design an in-memory hotel booking platform (OYO-like) where guests can search hotels, check room availability by dates, book rooms, and manage stay lifecycle.

## Functional requirements

1. Register guests with contact details.
2. Onboard hotels (name, city, address, rating) and rooms (type, price per night, floor).
3. Search hotels by city (sorted by rating).
4. List available rooms for a hotel and date range (check-in day, check-out day).
5. Create booking — validate no overlapping stay on same room.
6. Cancel booking (before check-in).
7. Check-in and check-out flow.
8. Pluggable pricing (standard per night, weekend surcharge).
9. Send notifications on confirm/cancel (demo console output).

## Out of scope

- Real payment gateway
- Persistent database
- Multi-property inventory sync
- Dynamic pricing ML / coupons

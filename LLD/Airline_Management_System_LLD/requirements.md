# Requirements — Airline Management System

## User roles

| Role | Permissions |
|------|-------------|
| `PASSENGER` | Search, book, pay, cancel (own flow via API) |
| `AIRLINE_STAFF` | Assign aircraft, assign crew, delay flights |
| `ADMIN` | Schedule flights, add infrastructure |

## Core entities

| Entity | Key fields |
|--------|------------|
| `Flight` | id, source, destination, departureDay, time, aircraftId, status |
| `Aircraft` | id, model, seat counts by class |
| `Seat` | id, flightId, class, status, price |
| `Passenger` | id, name, contact, baggageKg |
| `Booking` | id, passenger, flight, seatIds, status, amount |
| `Payment` | id, bookingId, amount, status |
| `CrewMember` | id, name, role |

## API (Facade)

| Method | Role | Description |
|--------|------|-------------|
| `searchFlights(src, dest, day)` | Any | Scheduled flights |
| `scheduleFlight` | Admin | Add flight |
| `assignAircraftToFlight` | Staff | Bind aircraft + generate seats |
| `assignCrewToFlight` | Staff | Crew roster |
| `createBooking` | Any | Lock seats, PENDING |
| `payForBooking` | Any | CONFIRMED + notify |
| `cancelBooking` | Any | Release seats + refund if paid |
| `changeFlight` | Any | Reschedule confirmed booking |
| `delayFlight` | Staff | Update schedule |

## Business rules

- Booking allowed only if aircraft + minimum crew (pilot + cabin) assigned.
- Baggage max **30 kg** per passenger.
- Seat double-booking prevented via `mutex` + status check.
- Cancel CONFIRMED → `REFUNDED` + seat release.

## Patterns

| Pattern | Where |
|---------|--------|
| Facade | `AirlineManagementSystem` |
| Strategy | `IPricingStrategy` / seat class fares |
| Service layer | Search, seats, crew, payment, notify |
| Mutex | Concurrent booking |

## Extensions

- Waitlist, meal selection, loyalty miles
- DB + optimistic locking (version on seat row)
- Kafka events `FlightDelayed`, `BookingConfirmed`

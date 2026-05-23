# Problem Statement: IRCTC Train Booking

Design a low-level **train ticket booking** system (IRCTC-style) with segment-based seat allocation and concurrent bookings.

## Functional requirements

1. **Search trains** by train number + date, or by source station, destination station, and date.
2. **View available seat count** for a train, source, destination, and date.
3. **Book confirmed tickets** — handle **concurrent** booking requests fairly (mutex per train run).
4. **Seat segment reuse**: the same physical seat may be sold to multiple passengers on **non-overlapping** route segments (e.g. A books BOM→BRC, B books RTM→NDLS on seat S1).
5. **Cancel** a booked ticket (seat segment freed for that interval).

## Assumptions

- Train runs once per day; reverse journey uses a **different train number**.
- Single coach; fixed seat IDs (`S1`…`Sn`).
- No berth/coach preference; no waitlist — only **CONFIRMED** bookings.
- Stations are ordered; journey must follow forward direction on the route.
- Intervals are **half-open** `[fromIdx, toIdx)` for overlap detection.

## Out of scope

- Payment, RAC/WL, dynamic pricing
- Multi-coach / berth types
- Persistent database (in-memory for interview)

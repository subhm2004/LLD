# IRCTC Train Booking LLD (C++17)

In-memory **train reservation** with **segment-based seat reuse** and **mutex-protected** concurrent booking.

## Quick start

```bash
chmod +x compile.sh
./compile.sh
./irctc_app
```

One-liner (must run from this folder):

```bash
g++ -std=c++17 -pthread main.cpp -o irctc_app && ./irctc_app
```

Or with explicit include root: `g++ -std=c++17 -pthread -I. main.cpp -o irctc_app`

## Architecture

```
IRCTC_LLD/
├── core/IRCTCSystem.h           # Facade
├── models/Train.h, TicketBooking.h, User.h
├── services/
│   ├── TrainCatalogService.h
│   ├── TrainSearchService.h
│   ├── SeatInventoryService.h
│   ├── SeatAllocationService.h  # segment overlap logic
│   └── BookingService.h         # per-run mutex
├── factories/BookingFactory.h
├── utils/SegmentUtils.h
└── main.cpp
```

## Segment overlap (core idea)

Journey `[from, to)` overlaps existing booking `[a, b)` iff:

`from < b && a < to`

Same seat **S1** can host **BOM→BRC** and **RTM→NDLS** because intervals do not overlap.

## Main APIs

| API | Description |
|-----|-------------|
| `addTrainRun(train, seatCount)` | Register daily run + coach seats |
| `searchByRoute(src, dest, date)` | List matching trains |
| `searchByTrainNumber(num, date)` | Single train lookup |
| `getAvailableSeatCount(...)` | Seats free for segment |
| `bookTicket(user, train, date, src, dest, seat?)` | Confirmed PNR |
| `cancelTicket(pnr, user)` | Release segment |

## Interview extensions

- RAC / waitlist queue
- Tatkal quota + locking window
- Dynamic pricing by segment
- Persistent seat ledger (Repository)
- Fair queue instead of mutex (ticket counter)

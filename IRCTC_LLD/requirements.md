# Requirements: IRCTC LLD

## Functional

| ID | Requirement |
|----|-------------|
| FR-1 | Search trains by route + date |
| FR-2 | Search train by number + date |
| FR-3 | Show available seat count for a segment |
| FR-4 | Book ticket (auto or preferred seat) |
| FR-5 | Reject overlapping segment on same seat |
| FR-6 | Allow non-overlapping reuse of same seat |
| FR-7 | Cancel booking |
| FR-8 | Thread-safe booking under concurrent load |

## Non-functional

| ID | Requirement |
|----|-------------|
| NFR-1 | C++17, `compile.sh`, pthread demo |
| NFR-2 | Per-train-run mutex for fair serialized booking |

## Patterns

| Pattern | Use |
|---------|-----|
| **Facade** | `IRCTCSystem` |
| **Factory** | `BookingFactory` |
| **Service layer** | Catalog, Search, Inventory, Allocation, Booking |

# Requirements — Concurrent HashMap LLD

## Must have

- [x] `IConcurrentMap` interface
- [x] `CoarseGrainedHashMap` — 1 global mutex
- [x] `StripedHashMap` — lock striping (N stripes)
- [x] `ConcurrentHashMap` alias → striped (default)
- [x] C++17 `std::unordered_map`, `std::optional`, `std::hash`
- [x] `#ifndef` include guards on all headers
- [x] Statistics: gets, hits, misses, puts
- [x] Demos: basic ops, parallel puts, hot key, coarse vs striped benchmark

## Extensions

- [ ] `shared_mutex` per stripe for read-heavy stripes
- [ ] Lock-free bucket (CAS linked list)
- [ ] Resize/rehash with global freeze lock
- [ ] `computeIfAbsent` atomic API

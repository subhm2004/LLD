# Concurrent HashMap LLD (C++17)

Thread-safe `unordered_map` — **coarse-grained** vs **lock striping** (fine-grained).

## Quick run

```bash
cd Concurrent_HashMap_LLD
./compile.sh
./concurrent_hashmap_app
```

## Structure

```
Concurrent_HashMap_LLD/
├── core/
│   ├── IConcurrentMap.h          # Interface
│   ├── CoarseGrainedHashMap.h    # Approach 1 — 1 mutex
│   ├── StripedHashMap.h          # Approach 2 — lock striping
│   └── ConcurrentHashMap.h       # Default alias → StripedHashMap
├── stats/MapStatistics.h
├── main.cpp
├── CONCURRENT_HASHMAP_COMPLETE.md
├── problem_statement.md
└── requirements.md
```

## API

```cpp
StripedHashMap map(16);  // 16 stripes

map.put("key", "value");
auto v = map.get("key");     // optional<string>
map.remove("key");
map.size();
```

## Which approach when?

| Approach | Parallelism | Complexity |
|----------|-------------|------------|
| **Coarse** | Low — one lock | Easiest |
| **Striping** | High — different stripes parallel | Interview sweet spot |
| **CAS** | Highest (expert) | Hardest |

## Full design

[`CONCURRENT_HASHMAP_COMPLETE.md`](./CONCURRENT_HASHMAP_COMPLETE.md)

## Related

- [`Thread_Safe_Cache_with_TTL_LLD`](../Thread_Safe_Cache_with_TTL_LLD/) — TTL + `shared_mutex`
- [`Multi_threading_C++/Concurrency_Challenges/Deadlock/`](../Multi_threading_C++/Concurrency_Challenges/Deadlock/)

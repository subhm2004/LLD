# Why LRU Over LFU? — Detailed Discussion

> **Context:** This repo has both [`LRU_Cache_LLD/`](./) (recency) and [`LFU_Cache_LLD/`](../LFU_Cache_LLD/) (frequency).  
> Interview me dono policies compare karni aati hain — yeh doc **kab LRU choose karo, kab nahi** explain karta hai.

---

## Table of Contents

1. [One-line difference](#one-line-difference)
2. [What LRU optimizes](#what-lru-optimizes)
3. [What LFU optimizes](#what-lfu-optimizes)
4. [When LRU is the better choice](#when-lru-is-the-better-choice)
5. [When LFU wins (honest comparison)](#when-lfu-wins-honest-comparison)
6. [Walkthrough examples](#walkthrough-examples)
7. [Production systems: what they actually use](#production-systems-what-they-actually-use)
8. [LRU weaknesses & mitigations](#lru-weaknesses--mitigations)
9. [Implementation trade-offs in this repo](#implementation-trade-offs-in-this-repo)
10. [Interview script (2 minutes)](#interview-script-2-minutes)
11. [Decision checklist](#decision-checklist)
12. [Further reading](#further-reading)

---

## One-line difference

| Policy | Eviction question |
|--------|-------------------|
| **LRU** | *"Kaunsa item sabse pehle use hua tha (time ke hisaab se)?"* |
| **LFU** | *"Kaunsa item sabse kam baar use hua tha (count ke hisaab se)?"* |

**LRU = temporal locality.**  
**LFU = popularity / hot-key stability.**

---

## What LRU optimizes

LRU assumes the **future looks like the recent past**:

- Jo abhi-abhi access hua, woh jaldi dubara access hoga.
- Purana / dormant data safe eviction candidate hai.

**Best fit workloads:**

- Web page caching (user abhi jo dekh raha hai, wohi repeat hoga thodi der)
- DB buffer pool (recent queries ke pages)
- OS page cache (working set shifts slowly)
- Session / user-specific caches (current cart, current feed chunk)
- CPU / CDN edge caches jahan **recency** matter karti hai

**Core idea:** Time order ≈ probability of next access.

---

## What LFU optimizes

LFU assumes **frequency predicts future value**:

- Ek baar bhi zyada access = valuable.
- Rare spikes ke baad long idle key ko bhi "hot" maan sakta hai (problem).

**Best fit workloads:**

- API rate-limit counters (same keys hit millions of times)
- CDN origin shield for viral content
- Database query plan cache (popular queries stay)
- Ad / recommendation feature caches with stable hot entities

**Core idea:** Access count ≈ long-term importance.

---

## When LRU is the better choice

### 1. Strong temporal locality

Agar access pattern **bursty + recent** hai:

```
t=1: A, B, C loaded
t=2: A, A, A  (hot now)
t=3: need space → evict C or B, NOT A
```

LRU correctly keeps **A** (just used).  
LFU might still evict **A** if historically **B** had higher count from an old session.

### 2. Working set shifts over time

User/session change, new time window, new query batch:

| Phase | Hot keys |
|-------|----------|
| Morning | `user:reports` |
| Afternoon | `user:checkout` |

LRU **adapts quickly** — purane morning keys naturally fall to LRU tail.  
LFU **remembers old frequency** — morning keys can block cache for hours unless you add aging.

### 3. Simplicity & predictability

| Aspect | LRU | LFU |
|--------|-----|-----|
| Mental model | Easy — linked list order | Harder — freq buckets + `minFreq` |
| Implementation | `map` + `list` + `splice` | freq maps + tie-break LRU |
| Bug surface | Lower | Higher (`minFreq` drift, stale freq) |
| Interview time | 15–20 min complete | Often needs extra explanation |

**Production teams often pick LRU** because operability and on-call debugging are simpler.

### 4. Scan / one-time access patterns

One-off full table scan:

- **LRU:** Scan keys enter and leave quickly — cache not polluted long-term.
- **LFU:** Scan can inflate frequencies → **cache pollution** until decay/aging added.

### 5. `get()` semantics match eviction

In this repo's [`LRUCacheCore.h`](./cache/LRUCacheCore.h):

- Every **hit** promotes key to MRU via `list::splice`.
- Eviction = `list::back()` (true LRU).

Policy and access path are **aligned** — no orphan frequency state.

---

## When LFU wins (honest comparison)

LFU is better when:

| Scenario | Why LFU wins |
|----------|--------------|
| Stable hot keys (CDN, top APIs) | Viral key stays despite occasional idle gaps |
| Long-tail + few superstars | Protect `/api/home` from eviction by cold keys |
| Frequency is the real signal | Billing tier, leaderboard, global counters |

### Classic LRU failure (LFU fixes)

Capacity = 2.

```
1. Access A once
2. Access B once
3. Access A 1000 times   → A is clearly hot
4. Access C (new)        → need eviction
```

| Policy | Evicts | Keeps |
|--------|--------|-------|
| **LRU** (if B touched after last A burst) | Possibly **A** | B, C |
| **LFU** | **B** (freq 1) | A, C |

Yahan **LFU correct** hai — A ko protect karna tha.

### LFU problems (why LRU is often default)

| Problem | Effect |
|---------|--------|
| **Stale frequency** | Ek din pehle viral post aaj bhi cache block kare |
| **New key cold start** | Naya hot item low freq se start → early eviction |
| **Tie-breaking** | Same freq → still need LRU within bucket |
| **Complexity** | More moving parts in code and ops |

**Mitigation:** LFU with **aging** (periodic freq halving) ≈ hybrid policy — interview me mention karo.

---

## Walkthrough examples

### Example A — Session cart (LRU wins)

Capacity = 3. User shopping session:

| Step | Operation | LRU order (MRU→LRU) | Notes |
|------|-----------|---------------------|-------|
| 1 | put `cart`, `profile`, `deals` | cart, profile, deals | |
| 2 | get `cart` × 5 | cart, deals, profile | cart hot **now** |
| 3 | put `checkout` (4th key) | checkout, cart, deals | evict **profile** |

**LFU risk:** Agar `profile` kal se freq=100 hai, LFU evicts `cart` instead — bad UX.

**Verdict:** **LRU** — recent session matters.

---

### Example B — API gateway (LFU can win)

Capacity = 3. Keys = API routes. Traffic:

| Route | Requests/day |
|-------|----------------|
| `/health` | 10M |
| `/v1/users` | 2M |
| `/v1/orders` | 1M |
| `/v1/rare` | 10 |

`/health` idle for 5 min between bursts but always returns.

| Policy | Behavior |
|--------|----------|
| **LRU** | `/health` might evict during idle gap → thundering herd on origin |
| **LFU** | `/health` stays (highest freq) |

**Verdict:** **LFU** (or LRU + TTL + pin for `/health`).

---

### Example C — Same repo demo (capacity 3)

From [`main.cpp`](./main.cpp) style trace:

```
put A, B, C
get A (many times)
put D  → eviction
```

| Policy | Typical eviction |
|--------|------------------|
| LRU | Key least recently touched among `{B,C}` |
| LFU | Key with lowest frequency count |

Run both projects side by side:

```bash
cd LRU_Cache_LLD && ./compile.sh && ./lru_cache_app
cd ../LFU_Cache_LLD && ./compile.sh && ./lfu_cache_app
```

---

## Production systems: what they actually use

| System | Policy | Comment |
|--------|--------|---------|
| **Linux page cache** | LRU-like (approx) | `active` / `inactive` lists — not pure LRU |
| **Memcached** | LRU | Simple, battle-tested |
| **Redis** | `allkeys-lru`, `volatile-lru`, **LFU variants** (`allkeys-lfu`) | Configurable — both exist |
| **CPU L1/L2** | LRU-ish | Hardware approximations |
| **CDN** | Often LFU + TTL | Hot content protection |

**Takeaway:** LRU is the **default teaching default**; mature systems expose **both** or approximations (ARC, 2Q, W-TinyLFU).

### Hybrid policies (interview bonus points)

| Algorithm | Idea |
|-----------|------|
| **LRU-K** | K-th reference distance |
| **2Q** | Separate queues for one-hit wonders vs hot |
| **ARC** | Adaptive balance recency + frequency |
| **W-TinyLFU** | Window LRU + admission filter (Caffeine cache) |

> *"I'd start with LRU; if metrics show hot-key retention issues, sample LFU or W-TinyLFU."*

---

## LRU weaknesses & mitigations

| Weakness | Mitigation |
|----------|------------|
| One-hit wonder evicts useful data briefly | 2Q / LRU-K |
| Scan pollutes cache | Segmented LRU, Bloom admission |
| No frequency signal | LFU hybrid, manual pin |
| Single mutex bottleneck (this LLD) | Sharded caches `hash(key) % N` |
| No TTL | `expiresAt` per entry + lazy expiry on get |

This project's [`ThreadSafeLRUCache`](./cache/ThreadSafeLRUCache.h) intentionally uses **one mutex** to teach correctness first.

---

## Implementation trade-offs in this repo

| Topic | LRU (`LRU_Cache_LLD`) | LFU (`LFU_Cache_LLD`) |
|-------|----------------------|----------------------|
| Core DS | `unordered_map` + `list` | `unordered_map` + freq buckets |
| On `get` hit | `splice` to front — **mutates** | Increase frequency — **mutates** |
| Eviction | `pop_back()` on list | `pop_back` on `minFreq` bucket |
| Thread safety | Mutex on all ops | Same pattern |
| Code lines | Fewer | More (`minFreq`, tie LRU) |
| Interview draw time | Faster | Slower |

**Why this repo teaches LRU first:**

1. Maps directly to `list::splice` interview question.
2. Foundation for `ThreadSafeLRUCache` decorator pattern.
3. LFU builds on same `ICache` interface — compare policies fairly.

---

## Interview script (2 minutes)

> **Q: Why LRU over LFU?**

**Answer structure:**

1. **Define both** — LRU = recency, LFU = frequency.
2. **State assumption** — "It depends on access pattern."
3. **LRU when** — temporal locality, shifting working set, simpler ops, scan-heavy workloads.
4. **LFU when** — stable hot keys, long-term popularity, protecting viral endpoints.
5. **This implementation** — hash map + list, O(1) get/put, mutex because get updates order.
6. **Extension** — Redis supports both; W-TinyLFU for admission; sharding for scale.

**Red flag to avoid:** "LRU is always better."  
**Green flag:** "I'd pick LRU for session-like recency; LFU when hot keys are stable across time."

---

## Decision checklist

Use **LRU** if most are true:

- [ ] Recent accesses predict next accesses
- [ ] Working set changes per user / session / time window
- [ ] You want simpler code and debugging
- [ ] One-time or scan traffic should not dominate cache forever
- [ ] Team familiarity matters (Memcached-style mental model)

Use **LFU** (or hybrid) if most are true:

- [ ] Few keys dominate total traffic for long periods
- [ ] Hot keys go idle briefly but must stay cached
- [ ] Frequency is a business metric (SLA on top routes)
- [ ] You can invest in aging / decay for stale counts

**Neither?** Consider **TTL + LRU**, **pinned keys**, or **ARC / W-TinyLFU**.

---

## Further reading

| Resource | Link |
|----------|------|
| LRU implementation (this repo) | [`cache/LRUCacheCore.h`](./cache/LRUCacheCore.h) |
| LFU implementation (compare) | [`../LFU_Cache_LLD/cache/LFUCacheCore.h`](../LFU_Cache_LLD/cache/LFUCacheCore.h) |
| UML diagrams | [Section 18 — LRU](../docs/SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md#18-thread-safe-lru-cache) · [Section 19 — LFU](../docs/SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md#19-thread-safe-lfu-cache) |
| Redis eviction policies | [Redis docs — eviction](https://redis.io/docs/reference/eviction/) |
| Caffeine (W-TinyLFU) | [GitHub — ben-manes/caffeine](https://github.com/ben-manes/caffeine/wiki) |

---

<p align="center">
  <b>LRU ≠ always best — but LRU is the right default when recency drives your workload.</b><br/>
  <sub>Compare both implementations in this repo, then justify your choice in interviews with data.</sub>
</p>

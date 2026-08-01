# ⚡ HLD Cheatsheet — Last-Minute Revision

> Interview se **15 min pehle** poora revise karne ke liye. Numbers + formulas + decision tables +
> har concept ek line me + har case study 3-line summary. Detail ke liye respective files dekho.

---

## 📊 1. Latency Numbers (every programmer should know)

| Operation | Time | Yaad rakhne ka |
|---|---|---|
| L1 cache reference | ~1 ns | Fastest |
| Branch mispredict | ~3 ns | |
| L2 cache reference | ~4 ns | |
| Mutex lock/unlock | ~17 ns | |
| **Main memory (RAM) reference** | ~100 ns | RAM = 100× L1 |
| Compress 1KB (Zippy) | ~2 µs | |
| Send 1KB over 1 Gbps network | ~10 µs | |
| **SSD random read** | ~16 µs | SSD ~150× RAM |
| Read 1MB sequentially from RAM | ~250 µs | |
| Round trip in same datacenter | ~500 µs | |
| Read 1MB sequentially from SSD | ~1 ms | |
| **Disk (HDD) seek** | ~10 ms | Disk = 100,000× RAM |
| Read 1MB from disk | ~20 ms | |
| **Round trip India ↔ US (network)** | ~150 ms | Cross-continent |

**Mota-moti yaad rakho:**
- **RAM = 100 ns, SSD = 16 µs, Disk seek = 10 ms, Cross-country network = 150 ms.**
- **Memory >> SSD >> Disk >> Network.** Isi liye cache (RAM) itna powerful.
- Disk seek 10ms ≈ 10 million ns ≈ **100,000× slower than RAM.**

---

## 🔢 2. Numbers & Powers of 2 (estimation)

| Power | Approx | Naam |
|---|---|---|
| 2^10 | ~1 Thousand | 1 KB |
| 2^20 | ~1 Million | 1 MB |
| 2^30 | ~1 Billion | 1 GB |
| 2^32 | ~4 Billion | (IPv4 range) |
| 2^40 | ~1 Trillion | 1 TB |
| 2^50 | ~10^15 | 1 PB |

**Time:** 1 day = **86,400 sec** (~10^5). 1 month ≈ 2.5M sec. 1 year ≈ 31.5M sec.
**Char sizes:** ASCII char = 1 byte; UUID = 16 bytes; timestamp = 8 bytes.

---

## 🧮 3. Estimation Formulas (back-of-envelope)

```
QPS (avg)      = Total requests per day / 86,400
Peak QPS       ≈ avg QPS × 2 to 3   (spikes)
Read QPS       = Write QPS × read:write ratio
Storage/day    = writes/day × size per record
Storage/N yr   = storage/day × 365 × N   (× replication factor)
Bandwidth      = QPS × payload size
Cache size     = 20% of daily reads × item size   (80/20 rule)
Servers needed = Peak QPS / (QPS one server handles)
```

**Common ratios / assumptions (interview me bolo):**
- Read:write = **100:1** (social/read-heavy), 1:1 (chat), write-heavy (logging/IoT).
- Peak = 2-3× average. Replication factor = 3. Cache hit rate target = 80-90%.
- One SQL server ~ few thousand QPS; Redis ~ 100K+ ops/s.

---

## 🎯 4. Availability "Nines"

| Availability | Downtime/year | Downtime/month |
|---|---|---|
| 99% (two 9s) | 3.65 days | 7.2 hrs |
| 99.9% (three 9s) | 8.76 hrs | 43 min |
| 99.99% (four 9s) | 52 min | 4.3 min |
| 99.999% (five 9s) | 5.26 min | 26 sec |

> **Error budget = 100% − SLO.** SLA = customer contract (breach → penalty).

---

## 🗄️ 5. DECISION TABLE — SQL vs NoSQL

| Choose **SQL** jab… | Choose **NoSQL** jab… |
|---|---|
| Strong consistency / **ACID transactions** (payments, banking) | Massive scale / horizontal (billions rows) |
| Complex queries, **joins**, relations | Simple access (key-value, lookups) |
| Structured, fixed schema | Flexible / evolving schema |
| Moderate scale, vertical OK | Very high write throughput |
| **Examples:** Postgres, MySQL | **Examples:** Cassandra, MongoDB, DynamoDB, Redis |

**NoSQL types:** Key-Value (Redis/DynamoDB) · Document (MongoDB) · Column-family (Cassandra) · Graph (Neo4j).
→ [SQL vs NoSQL](./SQL_vs_NoSQL.md)

---

## 📰 6. DECISION TABLE — Push vs Pull Fanout (feed)

| | **Push (fanout-on-write)** | **Pull (fanout-on-read)** | **Hybrid** ⭐ |
|---|---|---|---|
| Kaam | Post → sabke feed cache me | Read pe followed-users se merge | Normal=push, celeb=pull |
| Read | ⚡ Fast (pre-computed) | Slow (merge on read) | ⚡ Fast |
| Write | Slow (celeb storm) | ⚡ Fast | Balanced |
| Best | Few followers | Celebs / inactive users | **Everyone (real Twitter)** |

→ [Twitter/News Feed](./System_Design_Case_Studies/05_Twitter_News_Feed.md)

---

## 🔒 7. DECISION TABLE — Optimistic vs Pessimistic Locking

| | **Optimistic** | **Pessimistic** |
|---|---|---|
| Kaise | Version/conditional update, no lock; conflict pe retry | Lock lelo (`SELECT FOR UPDATE`), phir kaam |
| Best jab | Conflicts **kam** (high concurrency, low contention) | Conflicts **zyada** (same row pe rush) |
| Cost | Retries on conflict | Lock wait / contention, deadlock risk |
| Example | Doc edit, inventory (low clash) | Seat booking flash sale, bank balance |

→ [Concurrency Control](./Concurrency_Control.md)

---

## 💾 8. DECISION TABLE — Cache Write Strategies

| Strategy | Kaise | Trade-off |
|---|---|---|
| **Cache-aside** (lazy) | App cache miss → DB → cache fill | Simple, most common; first-read slow |
| **Write-through** | Write → cache + DB (sync) | Consistent, write slow |
| **Write-back** | Write → cache, DB async later | Fast write, data-loss risk |
| **Eviction** | LRU (common) / LFU / FIFO | Memory full pe kya hataao |

→ [Caching](./08_Caching_and_Distributed_Caching.md)

---

## ⚖️ 9. DECISION TABLE — Consistency & CAP

| | Matlab |
|---|---|
| **CP** (consistency + partition-tolerance) | Partition me availability chhodo (banking, booking). Ex: Zookeeper, HBase |
| **AP** (availability + partition-tolerance) | Partition me stale data OK (social feed, cart). Ex: Cassandra, DynamoDB |
| **Strong consistency** | Har read latest write dekhta |
| **Eventual consistency** | Thodi der baad sab converge (feeds, DNS) |
| **PACELC** | Partition → C/A; Else → Latency/Consistency |

→ [CAP Theorem](./11_CAP_Theorem.md)

---

## 🌐 10. Quick Decision one-liners

| Sawaal | Jawaab |
|---|---|
| L4 vs L7 load balancer? | L4 = fast, IP/port (TCP); L7 = smart, content/path-based (HTTP) |
| Long polling vs WebSocket vs SSE? | WS = bidirectional; SSE = server→client only; polling = fallback |
| REST vs gRPC vs GraphQL? | REST = standard/public; gRPC = fast internal (protobuf); GraphQL = flexible client queries |
| Message queue vs pub-sub? | Queue = 1 consumer per msg (task); Pub-sub = all subscribers (fanout) |
| Kafka vs RabbitMQ? | Kafka = high-throughput log/streaming, replay; RabbitMQ = flexible routing, low-latency tasks |
| Horizontal vs vertical scaling? | Vertical = bigger machine (limit + SPOF); horizontal = more machines (needs stateless) |
| Proxy vs reverse proxy? | Forward = client-side (hide client); reverse = server-side (hide servers, LB/cache/SSL) |
| Sync vs async replication? | Sync = consistent, slow; async = fast, lag/data-loss risk |

---

## 🧩 11. Patterns & Algorithms (ek line me)

**Scaling & Reliability**
- **Consistent hashing** — node add/remove pe sirf 1/N keys remap (cache/shard distribution).
- **Sharding** — data ko horizontal partitions me baanto (by range/hash/geo); watch hotspots.
- **Replication** — copies for availability + read scaling; sync vs async; quorum W+R>N.
- **Circuit breaker** — dependency baar-baar fail → calls turant reject (cascading failure roko).
- **Bulkhead** — resources isolate (ek dependency ka failure baaki ko na le doobe).
- **Retry + exponential backoff + jitter** — transient fail pe smart retry (idempotent only).
- **Rate limiting** — token bucket (bursts) / sliding window; distributed via Redis atomic.
- **Idempotency** — same request N baar = ek hi effect (idempotency key + dedup).

**Data & Consistency**
- **Saga** — distributed txn as local steps + compensating actions (choreography/orchestration).
- **2PC** — atomic commit across services (blocking, coordinator SPOF).
- **CQRS** — read model ≠ write model (scale reads/writes separately).
- **Event sourcing** — state = append-only event log; replay for state/history.
- **Outbox** — DB write + event atomically (dual-write problem fix).
- **CDC** — DB changes ko stream karo (Debezium) — sync to search/cache/lake.

**Data structures / algorithms**
- **Bloom filter** — "definitely not / maybe yes" membership, O(1), no false negatives.
- **B+Tree** — read + range queries (Postgres/MySQL); **LSM-Tree** — write-heavy (Cassandra).
- **Inverted index** — word→docs (search engines).
- **Geohash/Quadtree/S2** — 2D proximity → 1D index ("nearby").
- **Merkle tree** — data diff/sync (replication anti-entropy).
- **Snowflake ID** — distributed unique roughly-sorted IDs (timestamp+machine+seq).

---

## 🏗️ 12. Case Study 3-Line Summaries

**TinyURL** — Read-heavy KV; **Base62(counter)** via distributed ID (Snowflake/ranges) = collision-free codes; NoSQL sharded by code + **Redis cache + read replicas + CDN**; 302 redirect for analytics.

**Twitter/Feed** — Feed = **fanout**; **hybrid** (normal push, celeb pull+merge) solves celebrity storm; async fanout (Kafka+workers) → Redis timelines (tweet IDs) → hydrate on read; cursor pagination.

**WhatsApp/Chat** — **WebSocket** + gateway servers + **registry (user→gateway)** + pub-sub routing; **persist-before-deliver** + at-least-once + msg_id dedup; presence via Redis heartbeat (on-demand); offline → queue + push.

**Uber** — **Geospatial (geohash/quadtree)** nearby → rank by ETA → **lock driver** (no double-match); location = in-memory Redis GEO, region-sharded, ephemeral (1.25M writes/s); WebSocket tracking; idempotent payment.

**YouTube/Netflix** — Upload → **async transcoding** (chunk → parallel workers → multi-quality) → object store; **CDN + adaptive bitrate (HLS/DASH)** = smooth playback; video in object store (not DB); view count async.

**Instagram** — **Media (object store + CDN + pre-gen sizes) + feed (hybrid fanout)**; pre-signed upload; Redis timelines (post IDs) → hydrate; stories=TTL, explore=ML, counts async/eventual.

**Rate Limiter** — **Token bucket / sliding window counter**; distributed = shared **Redis** counter + **atomic INCR/Lua** (race-free); place at API Gateway; 429 + Retry-After; fail open vs closed.

**Notification** — **Queue-based async fanout** → per-channel workers → providers (APNs/FCM/SMS/email); at-least-once + dedup + **DLQ**; **priority queues** (OTP > marketing); provider outage → circuit breaker + fallback.

**Distributed Cache** — Single node = HashMap + **LRU/LFU**; distribution = **consistent hashing** (+virtual nodes); **replication + failover**; handle stampede (single-flight), hot key (replicate), penetration (Bloom filter).

**Google Docs** — Core = concurrent edit conflict: **OT** (transform ops, Google Docs) vs **CRDT** (commutative unique-ID ops, Figma); **WebSocket** broadcast + optimistic local UI; op log (event sourcing) + snapshots; partition by document.

**Ticketmaster** — **No double-booking**: atomic conditional update (`WHERE status=AVAILABLE`) / locking, ACID SQL = single truth; **seat hold + TTL**; **idempotency** (no double charge); **CP over AP**; flash sale → waiting room; payment via Saga.

---

## 🧭 13. RESHADED Framework (interview flow)

```
R - Requirements   (functional + non-functional + scope; read:write ratio poocho)
E - Estimation     (QPS, storage, bandwidth)
S - Schema/Data    (SQL vs NoSQL, data model)
H - High-level     (components + data flow — draw karo)
A - API            (key endpoints)
D - Deep dive      (1-2 components in detail)
E - Evaluate       (bottlenecks, SPOF, scaling)
D - Discuss        (trade-offs: CAP, cost, consistency)
```

**Golden rules:**
1. Requirements + estimation pehle (5 min) — bina scope ke design mat karo.
2. High-level pehle, phir deep-dive — interviewer ko drive karne do.
3. Har choice ka **trade-off** bolo (no perfect answer; "X kyunki Y, but Z ka cost").
4. Bottleneck → solution (cache, replica, shard, queue, CDN).
5. Numbers use karo (estimation), hand-wave nahi.

---

> 📚 Full detail: [HLD index](./README.md) · [Case Studies](./System_Design_Case_Studies/README.md) · [Advanced Topics](./Advanced_Topics/README.md) · [Interview guide](./HLD_Interview.md)
>
> **Isko interview se pehle 15 min me revise kar. All the best bhai! 🚀**

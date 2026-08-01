# 🏛️ High Level Design (HLD) / System Design — Complete Interview Guide

> Ye guide web research se banayi hai — FAANG-style system design interviews ke liye. Iska bhai
> `LLD_Interview.md` bhi padho (LLD = ek component ke andar ka code, HLD = poore system ka
> architecture). Language: **Hinglish**.

---

## 📑 Table of Contents

1. [HLD kya hai + interview me kya dekha jaata hai](#h1)
2. [HLD crack karne ka framework (RESHADED)](#h2)
3. [Back-of-envelope capacity estimation](#h3)
4. [Scalability fundamentals](#h4)
5. [Load Balancing](#h5)
6. [Caching (poora)](#h6)
7. [Databases — SQL vs NoSQL, indexing, sharding, replication](#h7)
8. [CAP theorem + consistency models](#h8)
9. [Message queues + async + event-driven](#h9)
10. [Networking — DNS, HTTP, WebSockets, REST/gRPC/GraphQL](#h10)
11. [Microservices + API gateway + resilience](#h11)
12. [Storage systems (blob/object/CDN)](#h12)
13. [Security + auth (OAuth/JWT)](#h13)
14. [Observability (logging/metrics/tracing)](#h14)
15. [Classic HLD problems (TinyURL, Twitter, Uber...)](#h15)
16. [Rapid-fire Q&A (100+)](#h16)
17. [Building blocks toolbox](#h17)
18. [Common trade-offs](#h18)
19. [Numbers cheat sheet](#h19)
20. [Glossary](#h20)
21. [Do's & Don'ts](#h18)
22. [Distributed systems deep concepts](#h22)
23. [Deep-dive: WhatsApp (full)](#h23)
24. [Deep-dive: Uber (full)](#h24)
25. [Mock interview transcript (URL shortener)](#h25)
26. [More deep-dive examples (Twitter, Instagram, YouTube, Payment, Dropbox, Docs...)](#h26)
27. [Database internals + storage engines](#h27)
28. [Scenario-based Q&A](#h28)
29. [Deep-dive: Ad aggregator, Zoom](#h29)
30. [Technology decision matrices](#h30)
31. [Kafka + Redis + Elasticsearch deep](#h31)
32. Must-know one-liners + distributed patterns

**⚡ PART II — ADVANCED / DEEP TOPICS**
- A1. [Sharding — deep dive](#a1) (shard key, resharding, cross-shard, hot shard)
- A2. [Replication — deep dive](#a2) (topologies, sync/async, conflict resolution, quorum)
- A3. [Distributed transactions](#a3) (2PC, 3PC, Saga, TCC, Outbox)
- A4. [Consensus + consistency](#a4) (Raft, linearizability vs serializability, PACELC)
- A5. [Probabilistic data structures](#a5) (Bloom, Count-Min, HyperLogLog, Merkle)
- A6. [Consistent hashing deep](#a6) (virtual nodes, bounded loads, rendezvous)
- A7. [Geospatial indexing](#a7) (geohash, quadtree, S2, H3, R-tree)
- A8. [Search systems](#a8) (inverted index, TF-IDF/BM25, ranking)
- A9. [Stream processing](#a9) (windowing, watermarks, exactly-once, Lambda/Kappa)
- A10. [Change Data Capture (CDC)](#a10)
- A11. [Multi-region / geo-distribution](#a11)
- A12. [Advanced caching](#a12) (multi-level, near cache, warming)
- A13. [Networking deep](#a13) (TLS, HTTP/2/3, QUIC)
- A14. [Security deep](#a14) (encryption, OAuth/JWT/mTLS, attacks)
- A15. [Resilience](#a15) (autoscaling, load shedding, backpressure, bulkhead)
- A16. [NoSQL data modeling](#a16) (access-pattern driven, single-table)
- A17. [Specialized DBs](#a17) (time-series, OLAP/columnar, graph, vector)
- A18. [Infra / Kubernetes / service mesh](#a18)
- A19. [Capacity planning + cost optimization](#a19)
- A20. [Reliability engineering (SRE)](#a20) (SLA/SLO/SLI, error budget, chaos, DR)

---

<a name="h1"></a>
## 1. HLD kya hai + interview me kya dekha jaata hai

**High Level Design (HLD)** = poore system ka architecture — kaunse components (services, DB,
cache, queue, load balancer), wo kaise baat karte hain, aur system **scale, available, aur
reliable** kaise rahega. Code nahi, boxes + arrows + trade-offs.

### HLD vs LLD
| | HLD | LLD |
|---|---|---|
| Scope | poora system | ek service/module ke andar |
| Output | architecture diagram | class diagram, code |
| Focus | scalability, DB, cache, network | OOP, design patterns, SOLID |
| Sawaal | "Design Instagram" | "Design a parking lot" |
| Level | 30,000 ft view | ground level code |

### Interviewer kya dekhta hai (scoring)
1. **Requirements + scope** — functional + non-functional clarify kiya?
2. **Estimation** — scale ka andaaza (QPS, storage, bandwidth)?
3. **High-level architecture** — components + data flow clear?
4. **Deep dive** — ek-do components me detail (DB schema, API, caching)?
5. **Bottlenecks + scaling** — single points of failure, scaling strategy?
6. **Trade-offs** — CAP, consistency vs availability, cost vs performance?
7. **Communication** — structured, clear, collaborative?

> ⭐ **HLD me koi "sahi answer" nahi hota** — trade-offs matter karte hain. "Ye choose kiya
> kyunki... aur iska cost ye hai" — yahi maturity dikhata hai.

### Typical structure (45-60 min)
```
0-5 min    : Requirements (functional + non-functional)
5-10 min   : Capacity estimation (QPS, storage, bandwidth)
10-15 min  : API design + data model
15-30 min  : High-level architecture (draw components)
30-45 min  : Deep dive (1-2 components) + scaling
45-60 min  : Bottlenecks, failure handling, trade-offs
```

---

<a name="h2"></a>
## 2. HLD crack karne ka framework — RESHADED

Har system design problem pe ye steps lagao (naam yaad rakho: **R-E-S-H-A-D-E-D**):

### R — Requirements (functional + non-functional)
**Functional (system kya karega):**
- "TinyURL: long URL → short URL, short → redirect. Custom alias? Expiry?"
- "Twitter: tweet post, timeline, follow, like?"

**Non-functional (kaise karega):**
- **Scale:** kitne users? DAU (daily active users)?
- **Read vs write ratio:** read-heavy (Twitter 100:1) ya write-heavy?
- **Latency:** kitni fast? (< 200ms?)
- **Availability:** 99.9% ya 99.99%?
- **Consistency:** strong ya eventual chalega?

### E — Estimation (capacity)
QPS (queries per second), storage, bandwidth, memory (cache). Neeche section 3 me detail.

### S — Storage / data model (schema)
Kaunsa data, kaunse tables/collections, relationships. SQL ya NoSQL?

### H — High-level design (architecture diagram)
Components draw karo: client → LB → API servers → cache → DB → queue → workers. Data flow dikhao.

### A — API design
Key endpoints. REST/gRPC. Request/response format.
```
POST /shorten  { longUrl }        -> { shortUrl }
GET  /{shortCode}                  -> 301 redirect
```

### D — Deep dive (1-2 components)
Interviewer ke interest ke hisaab se ek component detail me — DB sharding, cache strategy,
consistent hashing, etc.

### E — Evaluate (bottlenecks + scaling)
Single points of failure? Kahan bottleneck? Kaise scale (add cache, shard DB, CDN)?

### D — Discuss trade-offs
CAP choice, consistency vs availability, cost, complexity. "Ye kiya kyunki..., cost ye hai."

> ⭐ **Har step pe interviewer se confirm karo** — collaborative raho, ek taraffa mat bolo.

---

<a name="h3"></a>
## 3. Back-of-envelope capacity estimation

Interview me numbers estimate karne padte hain. Ye yaad rakho:

### Powers of 2 (data sizes)
| Power | Approx | Name |
|---|---|---|
| 2^10 | ~1 thousand | 1 KB |
| 2^20 | ~1 million | 1 MB |
| 2^30 | ~1 billion | 1 GB |
| 2^40 | ~1 trillion | 1 TB |
| 2^50 | ~1 quadrillion | 1 PB |

### Latency numbers (har engineer ko pata hone chahiye)
| Operation | Time |
|---|---|
| L1 cache reference | ~1 ns |
| Main memory (RAM) reference | ~100 ns |
| SSD random read | ~16 μs (16,000 ns) |
| Round trip within datacenter | ~500 μs |
| Disk (HDD) seek | ~10 ms |
| Round trip India → US | ~150 ms |
> **Takeaway:** memory >> SSD >> disk >> network. Isliye caching (memory) itni important hai.

### Availability (nines)
| Availability | Downtime/year |
|---|---|
| 99% (two nines) | ~3.65 days |
| 99.9% (three nines) | ~8.76 hours |
| 99.99% (four nines) | ~52 minutes |
| 99.999% (five nines) | ~5 minutes |

### QPS estimation ka formula
```
DAU (daily active users) × actions per user per day
--------------------------------------------------  = average QPS
86,400 (seconds in a day)

Peak QPS ≈ 2-3 × average QPS
```

### Example — TinyURL estimation
```
Assume: 100M new URLs/day (write), 100:1 read:write ratio
Writes/sec = 100M / 86400 ≈ 1,160 writes/sec
Reads/sec  = 116,000 reads/sec (100x)  -> READ-HEAVY -> caching critical

Storage: 100M URLs/day × 365 days × 5 years = 182 billion URLs
Per URL ≈ 500 bytes -> 182B × 500 = ~91 TB over 5 years

Cache: 20% URLs = 80% traffic (Pareto). Cache hot 20%:
  Daily reads = 116K/s × 86400 ≈ 10B reads/day
  20% unique ≈ 2B URLs hot... cache top few GB of hottest
```

### Estimation tips
- Round numbers use karo (86400 ≈ 100K seconds/day for quick math)
- Read:write ratio poocho — decides caching importance
- Storage = records × size × retention period × replication factor
- Bandwidth = QPS × payload size

---

<a name="h4"></a>
## 4. Scalability fundamentals

### 4.1 — Vertical vs Horizontal scaling
| | Vertical (scale up) | Horizontal (scale out) |
|---|---|---|
| Kaise | ek machine ko bada karo (more CPU/RAM) | zyada machines add karo |
| Limit | hardware limit (ceiling) | practically unlimited |
| Cost | mehnga (high-end hardware) | commodity hardware sasta |
| Failure | single point of failure | fault tolerant (ek mari to baaki) |
| Complexity | simple (no code change) | complex (distributed, LB, coordination) |
> **Modern systems horizontal scale karte** — commodity machines, fault tolerant. Par isके
> liye **stateless services** chahiye.

### 4.2 — Stateless vs Stateful
- **Stateless:** server request ke beech koi state nahi rakhta. Koi bhi server koi bhi request
  handle kar sakta → easy horizontal scaling, LB simple.
- **Stateful:** server client ka state rakhta (session in memory) → wahi server chahiye
  (sticky session) → scaling mushkil.
> **Best practice:** services stateless banao, state ko **external store** (Redis, DB) me rakho.
> Session Redis me, file S3 me — server sirf compute.

### 4.3 — Scaling ka general roadmap (single server → millions)
```
1. Single server (app + DB ek jagah)
2. DB alag karo (app server | DB server)
3. Load balancer + multiple app servers (stateless)
4. Cache add karo (Redis) — read load kam
5. DB read replicas (read scaling)
6. DB sharding (write scaling)
7. CDN (static content)
8. Message queue (async processing)
9. Microservices (team + component scaling)
```

### 4.4 — Single Point of Failure (SPOF) hatao
Har component ka **redundancy** rakho:
- LB → multiple LBs (active-passive)
- App servers → multiple (LB behind)
- DB → replicas + failover
- Cache → cluster
> "Agar ye component mar jaye to kya hoga?" — har box pe ye poocho.

### 4.5 — Consistent Hashing ⭐ (bahut poocha)
**Problem:** N servers me data distribute karna. Simple `hash(key) % N` — ek server add/remove
karo to **saari keys remap** ho jaati (massive cache miss / data movement).

**Solution — Consistent Hashing:** servers aur keys dono ek **hash ring** (0 to 2^32) pe map.
Key clockwise nearest server ko jaati. Server add/remove pe sirf **1/N keys** move hoti (adjacent).

```
        [Server A]
       /          \
   key3            key1
     |    RING      |
   key2            [Server B]
       \          /
        [Server C]
```
- **Virtual nodes:** har server ko ring pe kai points pe rakho — better load distribution.
- **Kahan:** distributed cache (Memcached), DB sharding (Cassandra, DynamoDB), load balancers.

---

<a name="h5"></a>
## 5. Load Balancing

**Load balancer (LB)** incoming traffic ko multiple servers pe distribute karta — no single
server overwhelmed, high availability, fault tolerance.

### 5.1 — LB algorithms
| Algorithm | Kaise | Kab |
|---|---|---|
| **Round Robin** | bari-bari har server | servers equal capacity |
| **Weighted Round Robin** | capacity ke hisaab se zyada | servers unequal |
| **Least Connections** | jisके paas kam active connections | long-lived connections |
| **Least Response Time** | fastest responding server | latency-sensitive |
| **IP Hash** | client IP se server decide | sticky session chahiye |
| **Consistent Hash** | hash ring | cache affinity |
> Repo: `LoadBalancer_LLD` me Round Robin + Least Connections implemented hai.

### 5.2 — Layer 4 vs Layer 7 LB
| | L4 (Transport) | L7 (Application) |
|---|---|---|
| Kis pe decide | IP + port (TCP/UDP) | HTTP content (URL, headers, cookies) |
| Speed | tez (no packet inspection) | thoda slow (inspect) |
| Smartness | kam | zyada (path-based routing, SSL termination) |
| Example | AWS NLB | AWS ALB, Nginx |
> **L7 fayda:** `/api/*` ek service ko, `/images/*` doosri ko route kar sakta. SSL termination,
> compression, caching bhi.

### 5.3 — Health checks
LB har server ko periodically ping karta (`/health`). Unhealthy server ko rotation se hatao —
traffic sirf healthy servers ko. Automatic failover.

### 5.4 — LB redundancy
LB khud SPOF na bane → **active-passive** (primary + standby, failover) ya **active-active**
(dono kaam karte, DNS round-robin). Heartbeat se failure detect.

### 5.5 — Sticky sessions
Agar server stateful ho to same client ko same server bhejo (cookie/IP hash). ⚠ Scaling
mushkil karta — better: stateless + external session store.

---

<a name="h6"></a>
## 6. Caching (poora — HLD ka dil)

**Cache** = frequently accessed data ko fast storage (memory) me rakho — latency kam, DB load kam.

### 6.1 — Cache kahan ho sakta (levels)
```
Client (browser cache) → CDN (edge) → Load Balancer → 
  Application (in-memory / local cache) → Distributed cache (Redis) → Database
```
Har level pe cache = har level pe load kam.

### 6.2 — Caching strategies ⭐ (bahut poocha)
| Strategy | Read | Write | Trade-off |
|---|---|---|---|
| **Cache-aside (lazy)** | app cache check → miss → DB → cache me daalo | app DB likhta, cache invalidate | most common; first read slow |
| **Read-through** | cache khud DB se load karta | — | app simple, cache library handle |
| **Write-through** | — | cache + DB dono ek saath likho | consistent par write slow |
| **Write-back (write-behind)** | — | cache me likho, DB baad me async | fast write par data loss risk |
| **Write-around** | — | seedha DB, cache skip | write-heavy me cache pollution kam |

**Cache-aside (sabse common) flow:**
```
READ:  data = cache.get(key)
       if miss: data = db.get(key); cache.set(key, data, TTL)
       return data
WRITE: db.update(key, value); cache.delete(key)   // invalidate (stale na rahe)
```

### 6.3 — Eviction policies
Cache full → kaunsa hatao?
- **LRU (Least Recently Used)** — sabse purana used. Most common. (Repo: `LRU_Cache_LLD`)
- **LFU (Least Frequently Used)** — sabse kam access. (Repo: `LFU_Cache_LLD`)
- **FIFO** — sabse pehle aaya.
- **TTL** — time-based expiry.

### 6.4 — ⭐ Cache invalidation (2 hardest problems in CS)
"Cache me stale data na rahe" — mushkil:
- **TTL (time-to-live)** — data auto-expire. Simple par thodi der stale.
- **Write-through invalidation** — write pe cache update/delete.
- **Event-based** — DB change → event → cache invalidate.
> Trade-off: chhoti TTL = fresh but zyada DB hits. Badi TTL = kam DB but stale risk.

### 6.5 — Cache problems (interview me poochte)
- **Thundering herd / Cache stampede:** popular key expire → ek saath hazaar requests DB pe.
  Fix: lock (ek request DB se laaye, baaki wait), ya probabilistic early refresh.
- **Cache penetration:** non-existent keys baar-baar query (DB miss har baar). Fix: null values
  bhi cache karo (short TTL), ya Bloom filter.
- **Hot key:** ek key pe bahut load (celebrity tweet). Fix: replicate hot key across nodes, local cache.
- **Cache avalanche:** bahut saari keys ek saath expire → DB spike. Fix: TTL me randomness (jitter).

### 6.6 — Redis vs Memcached
| | Redis | Memcached |
|---|---|---|
| Data types | rich (list, set, hash, sorted set) | simple key-value |
| Persistence | haan (RDB/AOF) | nahi (pure memory) |
| Replication | haan | nahi (native) |
| Use | complex (leaderboard, pub-sub, queue) | simple caching |
> Redis default choice (versatile). Memcached simple pure-cache ke liye.

### 6.7 — CDN (Content Delivery Network)
Static content (images, videos, CSS, JS) ko **edge servers** (user ke paas) pe cache karo.
- User ko nearest edge se milta → latency kam, origin load kam.
- **Push CDN:** content pehle se push. **Pull CDN:** first request pe origin se pull + cache.
- Examples: Cloudflare, Akamai, CloudFront.

---

<a name="h7"></a>
## 7. Databases — SQL vs NoSQL, indexing, sharding, replication

### 7.1 — SQL vs NoSQL ⭐
| | SQL (RDBMS) | NoSQL |
|---|---|---|
| Schema | fixed (rigid) | flexible (schema-less) |
| Scaling | vertical (hard to shard) | horizontal (built-in) |
| Consistency | strong (ACID) | eventual (BASE) usually |
| Relationships | joins, foreign keys | denormalized, embedded |
| Query | powerful SQL, joins | limited (key-based mostly) |
| Examples | MySQL, PostgreSQL | MongoDB, Cassandra, DynamoDB, Redis |
| Kab | complex queries, transactions (banking) | huge scale, flexible data (feeds, logs) |

**NoSQL types:**
- **Key-Value:** Redis, DynamoDB (cache, session)
- **Document:** MongoDB (JSON docs, flexible)
- **Column-family:** Cassandra, HBase (write-heavy, time-series)
- **Graph:** Neo4j (relationships, social network)

### 7.2 — ACID (SQL transactions)
- **Atomicity** — poora ya kuch nahi (all-or-nothing)
- **Consistency** — valid state se valid state (constraints hold)
- **Isolation** — concurrent transactions ek doosre ko disturb na karein
- **Durability** — commit ke baad data permanent (crash pe bhi)

### 7.3 — BASE (NoSQL)
- **Basically Available** — hamesha respond (shayad stale)
- **Soft state** — state badal sakti bina input (eventual sync)
- **Eventual consistency** — thodi der me sab replicas consistent
> ACID = consistency prioritize. BASE = availability prioritize. CAP theorem se juda.

### 7.4 — Indexing ⭐
**Index** = data structure (usually B-tree) jo lookup fast karta (O(log n) vs O(n) scan).
```sql
CREATE INDEX idx_email ON users(email);   -- email lookup ab O(log n)
```
- **Fayda:** read fast (WHERE, JOIN, ORDER BY).
- **Cost:** write slow (index bhi update), extra storage.
- **B-tree index:** range queries (`<`, `>`, BETWEEN) ke liye.
- **Hash index:** exact match ke liye (`=`).
- **Composite index:** multiple columns `(last_name, first_name)` — order matters.
> **Rule:** frequently queried columns pe index, par bahut zyada indexes = slow writes.

### 7.5 — Normalization vs Denormalization
- **Normalization:** data ko tables me todo, no duplication (3NF). Consistent, par joins mehnge.
- **Denormalization:** data duplicate karo (embed) — reads fast (no join), par updates multiple jagah.
> **OLTP** (transactions) = normalized. **OLAP/read-heavy** (analytics, feeds) = denormalized.

### 7.6 — Replication (read scaling + availability)
Data ki copies multiple servers pe:
- **Master-Slave (Primary-Replica):** master writes, slaves reads. Read scaling. ⚠ replication lag.
- **Master-Master:** dono writes. Conflict resolution chahiye. Higher availability.
- **Sync vs Async replication:** sync = consistent but slow. async = fast but lag (stale reads).
```
        writes          reads (scaled)
Client ──────> Master ──────> Replica 1
                  │  replicate  Replica 2
                  └───────────> Replica 3
```

### 7.7 — Sharding (write scaling) ⭐
Data ko **multiple DB instances** me baato (horizontal partition). Har shard ka subset.

**Sharding strategies:**
| Strategy | Kaise | Trade-off |
|---|---|---|
| **Range-based** | key range se (A-M shard1, N-Z shard2) | simple, par hotspots (uneven) |
| **Hash-based** | hash(key) % N | even distribution, par range query mushkil |
| **Consistent hashing** | hash ring | resharding aasan (1/N move) |
| **Directory-based** | lookup table (key → shard) | flexible, par lookup overhead |
| **Geo-based** | location se | latency kam, par uneven |

**Sharding ke problems:**
- **Cross-shard queries/joins** — mushkil (multiple shards se data). Avoid ya scatter-gather.
- **Resharding** — data grow → shards add → data move (consistent hashing helps).
- **Hotspots** — ek shard pe zyada load (celebrity). Fix: better shard key.
- **Transactions across shards** — distributed transaction (2PC / saga) needed.

### 7.8 — Partitioning vs Sharding
- **Partitioning:** ek DB ke andar data split (horizontal/vertical partitions).
- **Sharding:** partitions ko **alag machines** pe (partitioning + distribution).

### 7.9 — SQL vs NoSQL — kaunsa kab (decision)
```
SQL choose karo agar:
  - complex queries/joins chahiye
  - strong consistency + transactions (banking, inventory)
  - relationships important
  - moderate scale

NoSQL choose karo agar:
  - massive scale (horizontal)
  - flexible/evolving schema
  - eventual consistency ok (feeds, logs, analytics)
  - simple access patterns (key lookup)
```

---

<a name="h8"></a>
## 8. CAP Theorem + Consistency Models

### 8.1 — CAP Theorem ⭐⭐ (guaranteed poocha)
Ek distributed system **teeno** guarantee nahi kar sakta, sirf **2 out of 3**:
- **C — Consistency:** har read latest write dekhe (sab nodes same data).
- **A — Availability:** har request response paaye (shayad stale).
- **P — Partition tolerance:** network partition (nodes ke beech communication toot) me bhi chale.

**Reality:** distributed system me **P zaroori** hai (network fail hoga hi). To choice hai **C ya A**:
```
Partition hui to:
  CP system -> consistency chuno (kuch requests fail/reject, par stale nahi)
              Examples: MongoDB, HBase, Redis, banking
  AP system -> availability chuno (respond karo, shayad stale)
              Examples: Cassandra, DynamoDB, DNS, social feeds
```

**Kab kya choose:**
- **CP:** banking, inventory, booking (stale data = galat) — consistency > availability.
- **AP:** social feed, likes count, analytics (thodi stale ok) — availability > consistency.

### 8.2 — PACELC (CAP ka extension)
> **P**artition hui to **A** ya **C**; **E**lse (normal) **L**atency ya **C**onsistency.
Yaani normal operation me bhi trade-off — low latency (async replication) vs strong consistency
(sync replication). DynamoDB = PA/EL (availability + low latency).

### 8.3 — Consistency models (spectrum)
| Model | Matlab |
|---|---|
| **Strong consistency** | har read latest write (jaise single machine) |
| **Eventual consistency** | thodi der me sab consistent (temporarily stale) |
| **Read-your-writes** | apni likhi cheez turant dikhe (dusron ki der se) |
| **Monotonic reads** | ek baar naya data dekha to purana nahi dikhega |
| **Causal consistency** | related events ka order preserve |
> **Trade-off:** strong = slow (coordination), eventual = fast (no wait). Choose per use case.

### 8.4 — Quorum (consistency tune karo)
`W + R > N` → strong consistency (N = replicas, W = write acks, R = read acks).
```
N=3 replicas.  W=2, R=2 -> W+R=4 > 3 -> strong (overlap guaranteed)
W=1, R=1 -> fast but eventual (may read stale)
W=3, R=1 -> fast reads, slow writes (read-heavy tune)
```
> Cassandra/DynamoDB me W, R, N tunable — per-query consistency.

### 8.5 — Consensus (distributed agreement)
Multiple nodes ek value pe agree karein (leader election, replication):
- **Paxos** — classic, complex.
- **Raft** — Paxos ka simpler version (leader-based, understandable). etcd, Consul use karte.
- **2PC (Two-Phase Commit)** — distributed transaction (prepare → commit). Blocking, SPOF (coordinator).

---

<a name="h9"></a>
## 9. Message Queues + Async + Event-Driven

### 9.1 — Message queue kyun?
Synchronous coupling todo. Producer message queue me daalta, consumer apni speed se process karta.
- **Decoupling** — producer/consumer independent.
- **Async** — producer wait nahi karta (fast response).
- **Buffering** — traffic spike absorb (queue me jama, dheere process).
- **Reliability** — consumer down ho to messages queue me safe.
```
Producer ──> [ Message Queue ] ──> Consumer(s)
             (buffer + decouple)
```

### 9.2 — Use cases
- Email/SMS/push notifications (async — user wait na kare)
- Video processing (upload → queue → transcode workers)
- Order processing (order → queue → inventory/payment/shipping)
- Log aggregation, analytics events

### 9.3 — Kafka vs RabbitMQ
| | Kafka | RabbitMQ |
|---|---|---|
| Type | distributed log (streaming) | traditional message broker |
| Throughput | very high (millions/sec) | high |
| Message retention | disk pe rakhta (replay possible) | consume ke baad delete (usually) |
| Ordering | per-partition | per-queue |
| Use | event streaming, log, analytics | task queues, RPC, routing |

### 9.4 — Queue vs Pub-Sub
- **Queue (point-to-point):** ek message ek consumer ko (work distribution). SQS.
- **Pub-Sub (fan-out):** ek message sab subscribers ko (broadcast). Kafka topics, SNS.
```
Queue:    Producer -> [Q] -> Consumer A (ya B, ya C — koi ek)
Pub-Sub:  Publisher -> [Topic] -> Subscriber A + B + C (sab ko)
```

### 9.5 — Delivery guarantees
- **At-most-once:** ho sakta miss ho (fire-forget). Fastest.
- **At-least-once:** guarantee delivery, par duplicate ho sakta → consumer **idempotent** ho.
- **Exactly-once:** ek hi baar (mushkil, expensive — Kafka transactions).
> Practical: **at-least-once + idempotent consumer** (most common).

### 9.6 — Event-driven architecture
Components events emit/consume karte (tight coupling nahi). "Order placed" event → inventory,
notification, analytics sab react karte independently.
- **Fayda:** loose coupling, scalable, extensible (naya consumer add without touching producer).
- **Cost:** eventual consistency, debugging mushkil (distributed flow), event ordering.

### 9.7 — Dead Letter Queue (DLQ)
Message repeatedly fail ho to DLQ me bhejo (poison message main queue block na kare). Baad me
inspect/retry.

---

<a name="h10"></a>
## 10. Networking — DNS, HTTP, WebSockets, REST/gRPC/GraphQL

### 10.1 — DNS (Domain Name System)
Domain name (`google.com`) → IP address. Distributed hierarchical:
```
Browser cache → OS cache → Resolver (ISP) → Root → TLD (.com) → Authoritative → IP
```
- **DNS caching** (TTL) — har baar full lookup nahi.
- **DNS load balancing** — ek domain ke multiple IPs (round-robin, geo-routing).

### 10.2 — HTTP/HTTPS
- **HTTP** — request/response, stateless. Methods: GET (read), POST (create), PUT (update/replace),
  PATCH (partial), DELETE.
- **Status codes:** 2xx success, 3xx redirect, 4xx client error (400 bad, 401 unauth, 403
  forbidden, 404 not found, 429 rate limit), 5xx server error (500, 502 bad gateway, 503 unavailable).
- **HTTPS** — HTTP + TLS (encryption). SSL termination usually LB pe.
- **HTTP/2** — multiplexing (ek connection pe multiple requests), header compression.
- **HTTP/3** — QUIC (UDP-based), faster.

### 10.3 — TCP vs UDP
| | TCP | UDP |
|---|---|---|
| Reliability | guaranteed, ordered | best-effort (drop possible) |
| Speed | slow (handshake, acks) | fast (no overhead) |
| Use | web, DB, file transfer | video/voice streaming, gaming, DNS |

### 10.4 — WebSockets (real-time)
HTTP request/response ke bajaye **persistent bidirectional** connection. Server client ko push
kar sakta (polling nahi).
- **Kab:** chat, live notifications, live scores, collaborative editing, trading.
- **Alternatives:** long polling (client baar-baar poochhe), SSE (server-sent events, one-way push).

### 10.5 — REST vs gRPC vs GraphQL ⭐
| | REST | gRPC | GraphQL |
|---|---|---|---|
| Protocol | HTTP/JSON | HTTP/2 + Protobuf (binary) | HTTP/JSON |
| Speed | moderate | fast (binary, HTTP/2) | moderate |
| Contract | loose (OpenAPI) | strict (.proto) | schema |
| Over/under-fetching | common problem | — | client exact data maangta |
| Use | public APIs, simple | internal microservices (fast) | flexible clients (mobile) |
| Streaming | limited | bidirectional streaming | subscriptions |

**REST best practices:** resources noun-based (`/users/123`), HTTP methods sahi, stateless,
versioning (`/v1/`), pagination (`?page=2&limit=20`), proper status codes.

### 10.6 — API idempotency + rate limiting
- **Idempotency:** POST retry pe duplicate na ho — idempotency key (client generate karta).
- **Rate limiting:** API abuse rokna — token bucket / sliding window (LLD me detail). 429 return.

---

<a name="h11"></a>
## 11. Microservices + API Gateway + Resilience

### 11.1 — Monolith vs Microservices ⭐
| | Monolith | Microservices |
|---|---|---|
| Structure | ek codebase, ek deploy | chhoti independent services |
| Deploy | poora ek saath | har service alag |
| Scaling | poora scale | per-service scale (jo busy wahi) |
| Tech | ek stack | per-service tech (polyglot) |
| Complexity | simple start | distributed complexity (network, monitoring) |
| Team | ek team | per-service team (independent) |
| Failure | ek bug poora down | isolated (ek service down, baaki chale) |
> **Start monolith, split into microservices jab zaroorat** (team/scale grow). Premature
> microservices = distributed monolith (worst of both).

### 11.2 — API Gateway
Client aur microservices ke beech single entry point:
- **Routing** — request sahi service ko.
- **Cross-cutting:** authentication, rate limiting, logging, SSL termination, caching — ek jagah.
- **Aggregation** — multiple services ka data combine (ek client request, multiple backend calls).
- Examples: Kong, AWS API Gateway, Nginx.

### 11.3 — Service Discovery
Microservices dynamic (scale up/down, IPs badalte) — kaunsi service kaha hai?
- **Client-side:** client registry (Consul/Eureka) se pooch ke direct call.
- **Server-side:** LB registry se pooch ke route.
- **Registry:** services register hoti (heartbeat), unhealthy remove.

### 11.4 — Resilience patterns ⭐
- **Circuit Breaker:** service baar-baar fail → "circuit open" (fail fast, retry na karo), thodi
  der baad "half-open" (test), theek to "closed". Cascading failure rokta.
- **Retry with backoff:** fail pe retry, par exponential backoff (1s, 2s, 4s) + jitter.
- **Timeout:** har call pe timeout (hang na ho).
- **Bulkhead:** resources isolate (ek service ka failure poora system na kha jaye).
- **Rate limiting:** overload se bachao.
- **Graceful degradation:** ek feature down → baaki chale (recommendations down → feed still works).

### 11.5 — Distributed transactions — Saga pattern
Microservices me ek DB transaction possible nahi (alag DBs). **Saga** = sequence of local
transactions, har step ka **compensating action** (undo):
```
Order flow: createOrder -> reserveInventory -> chargePayment -> ship
Agar chargePayment fail -> compensate: releaseInventory + cancelOrder
```
- **Choreography:** har service event emit/consume (no central coordinator).
- **Orchestration:** central orchestrator steps manage karta.
> Repo `Ecommerce_Cart_Checkout_LLD` me reservation saga (reserve → commit/release) — LLD level pe.

### 11.6 — Database per service
Har microservice ka apna DB (loose coupling). ⚠ Cross-service query mushkil → API calls ya
event-driven data replication. Shared DB = tight coupling (anti-pattern).

---

<a name="h12"></a>
## 12. Storage Systems

### 12.1 — Storage types
| Type | Kya | Kab | Example |
|---|---|---|---|
| **Block storage** | raw disk blocks | DB, low-level | AWS EBS |
| **File storage** | hierarchical files/folders | shared FS | NFS, AWS EFS |
| **Object storage** | flat key → object + metadata | media, backups, big files | AWS S3, GCS |
> **Object storage (S3)** system design me sabse common — cheap, scalable, HTTP access, unlimited.
> Images/videos/files S3 me, metadata DB me (S3 URL store karo).

### 12.2 — Blob storage pattern
Large files (images, videos) DB me mat rakho (DB bloat). S3 me daalo, DB me sirf URL/metadata.
```
Upload: client -> pre-signed S3 URL -> direct upload to S3 -> DB me metadata (url, size, owner)
Serve:  DB se URL -> CDN -> S3 (origin)
```

### 12.3 — Data warehouse vs Data lake
- **Data warehouse:** structured, processed data for analytics (Redshift, BigQuery). Schema-on-write.
- **Data lake:** raw data (any format) for future processing (S3 + Spark). Schema-on-read.

---

<a name="h13"></a>
## 13. Security + Authentication

### 13.1 — Authentication vs Authorization
- **Authentication (AuthN):** "tum kaun ho?" (login, verify identity).
- **Authorization (AuthZ):** "tum kya kar sakte ho?" (permissions, roles).

### 13.2 — Session vs Token (JWT) ⭐
| | Session-based | Token-based (JWT) |
|---|---|---|
| State | server session store (Redis) | stateless (token me sab) |
| Scaling | session store shared chahiye | stateless (easy scale) |
| Revoke | easy (delete session) | mushkil (token valid till expiry) |
| Use | traditional web | APIs, microservices, mobile |

**JWT structure:** `header.payload.signature` — payload me user claims (id, role, expiry),
signature se tamper-proof. Server signature verify karta (no DB lookup).

### 13.3 — OAuth 2.0 (third-party auth)
"Login with Google/Facebook" — user apna password app ko nahi deta, Google token deta:
```
User -> App -> Google (login) -> Google returns auth code -> 
App exchanges code for access token -> App uses token to access user data
```
- **OAuth** = authorization. **OpenID Connect (OIDC)** = OAuth + authentication (identity).

### 13.4 — Common security measures
- **HTTPS/TLS** — encryption in transit (always).
- **Encryption at rest** — DB/disk encrypted.
- **Hashing (passwords)** — bcrypt/Argon2 + salt (never plain text). (Repo GPay/Truecaller me note.)
- **Rate limiting** — brute-force, DDoS se bachao.
- **Input validation** — SQL injection, XSS se bachao.
- **Principle of least privilege** — sabko minimal access.
- **API keys / tokens** — service-to-service auth.

### 13.5 — DDoS protection
- Rate limiting, WAF (web application firewall), CDN (absorb traffic), IP blacklisting,
  auto-scaling (absorb spike).

---

<a name="h14"></a>
## 14. Observability (Monitoring)

Production system me "kya ho raha hai" dekhna zaroori. **3 pillars:**

### 14.1 — Logging
Events ka record (errors, requests). Centralized (ELK stack: Elasticsearch + Logstash + Kibana).
Structured logs (JSON) — searchable. Log levels (DEBUG/INFO/WARN/ERROR). (Repo: `Logger_LLD`)

### 14.2 — Metrics
Numeric measurements over time (QPS, latency p50/p95/p99, error rate, CPU, memory).
- **Prometheus + Grafana** — collect + visualize.
- **RED method:** Rate, Errors, Duration. **USE method:** Utilization, Saturation, Errors.
- **p99 latency** > average (tail latency matters — 1% users ka experience).

### 14.3 — Tracing (distributed)
Ek request ka poora journey across microservices (trace id har hop pe). Jaeger, Zipkin.
Bottleneck kahan hai pata chalta (kaunsi service slow).

### 14.4 — Alerting
Threshold cross (error rate > 5%, p99 > 1s) → alert (PagerDuty/Slack). On-call engineer.

### 14.5 — Health checks
Har service `/health` endpoint — LB/orchestrator check karta. Unhealthy → restart/remove.

---

<a name="h15"></a>
## 15. Classic HLD Problems (detailed walkthroughs)

> Format: **Requirements → Estimation → API → Data model → Architecture → Deep dive → Scaling**.
> Interviewer in problems se 90% cover ho jaata.

### 15.1 — 🔗 TinyURL (URL Shortener) ✅ (repo LLD: `URL_Shortner_LLD`)
**Requirements:** long URL → short URL, short → redirect. Custom alias, expiry (optional). Analytics.

**Estimation:** 100M writes/day, 100:1 read:write → 116K reads/sec (READ-HEAVY → caching critical).
Storage ~91 TB over 5 years.

**API:**
```
POST /shorten { longUrl, customAlias?, expiry? } -> { shortUrl }
GET  /{shortCode} -> 301 redirect to longUrl
```

**Data model:**
```
url_mappings: shortCode (PK) | longUrl | createdAt | expiry | userId
```

**⭐ Short code generation (key decision):**
- **Counter + Base62:** global counter → base62 encode (a-z,A-Z,0-9). 7 chars = 62^7 = 3.5
  trillion URLs. Unique, short. ⚠ counter distributed karna (range allocation per server, ya
  ticket server / ZooKeeper).
- **Hash (MD5/SHA):** hash(longUrl) → first 7 chars. ⚠ collision handling (retry with salt).
- **Random:** random 7 chars, check DB collision.

**Architecture:**
```
Client -> LB -> App servers -> Cache (Redis, hot URLs) -> DB (NoSQL, key-value)
                                                        -> Analytics (async via queue)
```

**Deep dive — redirect flow (read-heavy):**
```
GET /abc123 -> cache.get("abc123")
   hit  -> 301 redirect (fast)
   miss -> DB lookup -> cache.set -> 301 redirect
```

**Scaling:** cache (80% traffic hot), DB sharding (by shortCode hash), CDN, read replicas.
**Trade-offs:** 301 (permanent, cached by browser — kam analytics) vs 302 (temporary — har hit
server pe — better analytics).

---

### 15.2 — 🐦 Twitter / News Feed ⭐⭐ (repo LLD: `Linkedin_LLD` feed)
**Requirements:** post tweet, follow users, home timeline (followed users ke tweets), like/retweet.

**Estimation:** 200M DAU, 100M tweets/day. Read-heavy (timeline views >> posts). Celebrity
(millions followers) = hot key problem.

**⭐⭐ CORE CHALLENGE — timeline generation (fan-out):**
| Approach | Kaise | Trade-off |
|---|---|---|
| **Fan-out on write (push)** | tweet post → sab followers ki timeline cache me push | read fast, par celebrity (1M followers) = 1M writes! |
| **Fan-out on read (pull)** | timeline maango → followed users ke tweets fetch + merge | write cheap, par read slow (merge on the fly) |
| **Hybrid (best)** | normal users push, celebrities pull | celebrity tweets read pe merge, baaki pre-computed |

**Hybrid flow:**
```
Post tweet:
   normal user -> fan-out to followers' timeline cache (push)
   celebrity -> DON'T fan-out (too many)
Read timeline:
   pre-computed timeline (cache) + celebrity tweets (pull, merge) -> sorted feed
```

**Data model:**
```
tweets:    tweetId | userId | content | createdAt
follows:   followerId | followeeId
timeline:  userId -> [tweetIds] (Redis cache, pre-computed)
```

**Architecture:**
```
Client -> LB -> API -> Tweet service -> DB + Queue (fan-out workers)
                    -> Timeline service -> Redis (timelines) 
                    -> Media -> S3 + CDN
```

**Scaling:** timeline in Redis, tweets sharded by userId, media on CDN, fan-out via queue (async).

---

### 15.3 — 📸 Instagram (photo sharing)
**Requirements:** upload photo, feed, follow, like, comment, stories.

**Estimation:** heavy media storage. 500M photos/day × ~2MB = 1 PB/day. Read-heavy feed.

**Key components:**
- **Media upload:** client → pre-signed S3 URL → direct upload. Metadata DB me.
- **Media serving:** CDN (edge cache) → S3 origin. Multiple resolutions (thumbnail/full).
- **Feed:** fan-out (Twitter jaisa hybrid).
- **Image processing:** upload → queue → workers (resize, compress, multiple resolutions).

**Data model:**
```
photos:  photoId | userId | s3Url | caption | createdAt
DB: metadata (SQL/NoSQL). Media: S3 + CDN.
```
**Trade-off:** photos immutable (append-only) → aggressive caching. Feed eventual consistency ok.

---

### 15.4 — 💬 WhatsApp / Chat ⭐ (repo LLD idea: chat)
**Requirements:** 1-1 chat, group chat, online status, delivery/read receipts, media.

**⭐ CORE — real-time delivery (WebSockets):**
```
User online -> persistent WebSocket connection to a chat server
Send message: A -> chat server -> (B online? push via B's WebSocket : store + push notification)
Delivery states: SENT -> DELIVERED (B received) -> READ (B opened)
```

**Architecture:**
```
Client <-WebSocket-> Chat servers (stateful — connection state)
                     -> Message store (Cassandra — write-heavy)
                     -> Presence service (Redis — online status)
                     -> Notification service (offline users -> push)
```

**Deep dive:**
- **Connection management:** har user ka WebSocket kis server pe — registry (Redis: userId → server).
- **Message ordering:** per-conversation sequence number / timestamp.
- **Offline:** message store me rakho, user online → deliver + push notification.
- **Group chat:** message → sab members ko fan-out (WebSocket ya push).

**Scaling:** chat servers stateful (sticky by user), Cassandra (write-heavy messages), Redis (presence).

---

### 15.5 — 🚕 Uber / Ride-sharing ⭐ (repo LLD idea: cab booking)
**Requirements:** rider request, driver matching, real-time location, pricing (surge), trip.

**⭐⭐ CORE — geospatial matching:**
- **Problem:** rider ke paas nearby available drivers dhundo (fast).
- **Solution — Geohash / QuadTree:** world ko grid cells me baato. Driver location → cell.
  Rider → apni cell + adjacent cells ke drivers query. O(1)-ish nearby lookup.
```
Geohash: lat/long -> string ("tdr1y"). Same prefix = nearby.
QuadTree: recursive grid subdivision, dense areas me zyada cells.
```

**Architecture:**
```
Rider app -> LB -> Trip service -> Matching service -> Location service (Redis geo / QuadTree)
Driver app -> Location updates (every few sec) -> Location service
           -> Pricing service (surge = demand/supply ratio)
```

**Data model:**
```
trips: tripId | riderId | driverId | status | pickup | drop | fare
driver_locations: driverId | lat | long | updatedAt (Redis, ephemeral)
```

**Deep dive:**
- **Location updates:** drivers har 4 sec location bhejte → Redis (high write). Old data TTL.
- **Matching:** nearby available drivers → nearest/ETA → offer → accept.
- **Surge pricing:** area me demand > supply → multiplier (1x-3x).
- **Trip state machine:** REQUESTED → MATCHED → STARTED → COMPLETED.

**Scaling:** location service sharded by geo region, Redis geo commands, matching per-region.

---

### 15.6 — 🎥 YouTube / Netflix (video streaming)
**Requirements:** upload video, stream, search, recommendations.

**⭐ CORE — video processing + streaming:**
- **Upload:** video → S3 → queue → transcoding workers (multiple resolutions/formats: 360p,
  720p, 1080p, 4K + HLS/DASH chunks).
- **Streaming:** adaptive bitrate (network ke hisaab se quality) — video chunks (segments).
- **CDN:** video chunks CDN pe (edge, user ke paas) — origin load minimal.

**Architecture:**
```
Upload -> S3 -> Queue -> Transcoding workers -> processed chunks -> S3 -> CDN
Watch  -> CDN (chunks) -> adaptive bitrate player
Metadata -> DB. Search -> Elasticsearch. Recommendations -> ML pipeline.
```
**Estimation:** massive storage (PB), massive bandwidth (CDN critical), read-heavy.
**Trade-off:** pre-transcode all resolutions (storage cost) vs on-demand (compute cost).

---

### 15.7 — 📁 Dropbox / Google Drive (file storage + sync)
**Requirements:** upload/download files, sync across devices, share, versioning.

**⭐ CORE — file sync + dedup:**
- **Chunking:** file ko chunks me todo (4MB). Sirf changed chunks upload (delta sync).
- **Deduplication:** chunk ka hash → agar hash pehle se hai to reference (storage bachao).
- **Sync:** client watches changes → upload changed chunks → notify other devices → download.

**Architecture:**
```
Client -> Metadata service (file tree, chunk list, versions) -> DB
       -> Block service -> S3 (chunks, deduped by hash)
       -> Notification service (sync trigger to other devices)
```
**Data model:** `files: fileId | userId | name | version | [chunkHashes]`. `chunks: hash -> S3 location`.

---

### 15.8 — ⏱️ API Rate Limiter ✅ (repo LLD: algorithms)
**Requirements:** per-user/IP limited requests per window. Distributed (multiple servers).

**⭐ Distributed rate limiter:**
```
Redis (centralized counter) — atomic INCR + TTL
   key = "rate:{userId}:{window}"
   count = INCR key; if count == 1: EXPIRE key window
   if count > limit: reject (429)
```
Algorithms (LLD me detail): Token Bucket, Sliding Window (section in LLD_Interview.md).
**Trade-off:** centralized Redis (accurate, network hop) vs local (fast, approximate).

---

### 15.9 — 🔍 Search Autocomplete / Typeahead
**Requirements:** user type kare, top suggestions instantly (< 100ms).

**⭐ CORE — Trie + ranking:**
- **Trie (prefix tree):** prefix se suggestions O(prefix length). Har node pe top-k cached.
- **Ranking:** frequency/popularity se sort. Pre-compute top-k per prefix.
```
Trie node "ca" -> pre-computed top 5: [cat, car, cake, camera, cash]
```
**Architecture:** Trie in memory (cache), periodic rebuild from search logs (offline). CDN for
static, WebSocket/debounced requests.
**Scaling:** Trie sharded by prefix, aggressive caching, eventual updates (new terms).

---

### 15.10 — 🔔 Notification System ✅ (repo LLD idea)
**Requirements:** multi-channel (email/SMS/push), templates, preferences, at scale.

**Architecture:**
```
Event -> Notification service -> Queue -> Channel workers (email/SMS/push)
      -> User preference check -> template render -> send (via provider) -> track delivery
```
**Deep dive:** rate limiting (spam na ho), dedup, priority queue, retry (DLQ), delivery tracking.
Providers: SendGrid (email), Twilio (SMS), FCM/APNS (push).

---

### 15.11 — Common problems (quick pointers)
| Problem | Key challenge |
|---|---|
| **Pastebin** | TinyURL jaisa + large text storage (S3) |
| **Ticketmaster/BookMyShow** | seat locking, concurrency, high contention (repo IRCTC LLD) |
| **Payment system** | idempotency, consistency (CP), double-spend, ledger (repo GPay LLD) |
| **Distributed cache** | consistent hashing, replication, eviction (repo LRU/LFU LLD) |
| **Google Docs** | real-time collaboration (OT/CRDT), WebSockets |
| **Web crawler** | BFS, politeness (rate limit per domain), dedup (Bloom filter), distributed |
| **Yelp / Nearby** | geospatial (QuadTree/Geohash), like Uber |
| **Leaderboard** | Redis sorted sets (ZADD/ZRANK), real-time ranking |
| **Distributed ID generator** | Snowflake (timestamp + machine + sequence), unique + sortable |

---

<a name="h16"></a>
## 16. Rapid-fire Q&A (100+)

### 🏛️ Fundamentals

**Q1. HLD vs LLD?**
HLD = poore system ka architecture (components, DB, cache). LLD = ek component ke andar ka code
(classes, patterns).

**Q2. System design me pehla step?**
Requirements clarify (functional + non-functional). Kabhi turant architecture nahi.

**Q3. Non-functional requirements kya-kya?**
Scale (DAU/QPS), latency, availability, consistency, read:write ratio, durability.

**Q4. Read-heavy vs write-heavy — design pe asar?**
Read-heavy → caching + read replicas. Write-heavy → sharding + write-optimized DB (Cassandra) + queue.

**Q5. Vertical vs horizontal scaling?**
Vertical = ek machine bada (limit + SPOF). Horizontal = zyada machines (unlimited, fault tolerant,
par distributed complexity).

**Q6. Stateless kyun important?**
Koi bhi server koi bhi request handle kare → easy horizontal scaling, simple LB, no sticky session.

**Q7. SPOF kya hai?**
Single Point of Failure — ek component mara to poora system down. Fix: redundancy har layer pe.

**Q8. Latency vs throughput?**
Latency = ek request ka time. Throughput = per second kitne requests. Dono optimize karo (kabhi
trade-off).

**Q9. Availability kaise badhaayein?**
Redundancy, replication, failover, load balancing, no SPOF, geo-distribution.

**Q10. 99.9% vs 99.99% availability?**
99.9% = 8.76 hrs/year downtime. 99.99% = 52 min/year. Har extra nine 10x mehnga.

### ⚖️ Load Balancing

**Q11. Load balancer kya karta?**
Traffic ko multiple servers pe distribute — no overload, high availability, failover.

**Q12. L4 vs L7 LB?**
L4 = IP/port (fast, dumb). L7 = HTTP content (smart routing, SSL, slower).

**Q13. LB algorithms?**
Round robin, weighted RR, least connections, least response time, IP hash, consistent hash.

**Q14. Health check kya?**
LB periodically server ping (`/health`) — unhealthy ko rotation se hata, healthy ko traffic.

**Q15. LB khud SPOF na bane?**
Active-passive (standby + failover) ya active-active (multiple + DNS RR).

**Q16. Sticky session ka problem?**
Client ko same server bind karta → scaling mushkil. Better: stateless + external session store.

### 🗄️ Caching

**Q17. Cache kyun?**
Frequently accessed data memory me — latency kam, DB load kam. Memory >> disk >> network.

**Q18. Cache-aside kya?**
App cache check → miss → DB → cache me daalo. Write pe cache invalidate. Most common.

**Q19. Write-through vs write-back?**
Write-through = cache + DB saath (consistent, slow). Write-back = cache pehle, DB async (fast, data loss risk).

**Q20. Cache eviction policies?**
LRU (recently used), LFU (frequently used), FIFO, TTL.

**Q21. Cache invalidation kaise?**
TTL (expiry), write-through invalidation, event-based. Trade-off: freshness vs DB load.

**Q22. Thundering herd?**
Popular key expire → ek saath hazaar requests DB pe. Fix: lock (ek laaye), early refresh.

**Q23. Cache penetration?**
Non-existent keys baar-baar query. Fix: null cache (short TTL), Bloom filter.

**Q24. Hot key problem?**
Ek key pe bahut load (celebrity). Fix: replicate across nodes, local cache.

**Q25. Redis vs Memcached?**
Redis = rich types + persistence + replication. Memcached = simple pure cache.

**Q26. CDN kya?**
Static content (images/video) edge servers pe cache — user ke paas, latency kam, origin load kam.

**Q27. Cache hit ratio kya?**
Hits / (hits + misses). Zyada = better. 80%+ good. Cache size + eviction se tune.

### 🗃️ Databases

**Q28. SQL vs NoSQL?**
SQL = fixed schema, ACID, joins, vertical. NoSQL = flexible, horizontal, eventual, huge scale.

**Q29. ACID expand?**
Atomicity, Consistency, Isolation, Durability. SQL transactions.

**Q30. BASE kya?**
Basically Available, Soft state, Eventual consistency. NoSQL.

**Q31. NoSQL types?**
Key-value (Redis), Document (MongoDB), Column (Cassandra), Graph (Neo4j).

**Q32. Index kya, cost?**
Fast lookup (B-tree, O(log n)). Cost: slow writes (index update), extra storage.

**Q33. Composite index — order matters?**
Haan. `(a, b)` index `a` aur `a,b` queries pe kaam karta, sirf `b` pe nahi (leftmost prefix rule).

**Q34. Normalization vs denormalization?**
Normalize = no duplication (consistent, joins mehnge). Denormalize = duplicate (reads fast, updates mushkil).

**Q35. Replication kya?**
Data copies multiple servers pe. Master-slave (read scaling), master-master (write availability).

**Q36. Replication lag?**
Async replication me slave master se peeche → stale reads. Fix: read from master for
read-your-writes, ya sync replication (slow).

**Q37. Sharding kya?**
Data multiple DBs me split (horizontal). Write scaling. Strategies: range, hash, consistent hash, geo.

**Q38. Sharding ke problems?**
Cross-shard joins mushkil, resharding, hotspots, distributed transactions.

**Q39. Shard key kaise choose?**
High cardinality, even distribution, common query pattern. Bad key = hotspots.

**Q40. Partitioning vs sharding?**
Partitioning = ek DB me split. Sharding = partitions alag machines pe.

**Q41. Consistent hashing kyun?**
Server add/remove pe sirf 1/N keys move (simple `%N` me saari remap). Cache/sharding me.

**Q42. Virtual nodes kya?**
Har physical server ko ring pe kai points — better load distribution, smooth rebalancing.

**Q43. OLTP vs OLAP?**
OLTP = transactions (normalized, fast writes). OLAP = analytics (denormalized, big reads, warehouse).

**Q44. Connection pooling kyun?**
DB connections mehnge — pool reuse karta, bar-bar create/destroy nahi.

### 🌐 CAP + Consistency

**Q45. CAP theorem?**
Distributed system Consistency, Availability, Partition tolerance — sirf 2/3. P zaroori → C ya A.

**Q46. CP vs AP example?**
CP = banking (consistency > availability). AP = social feed (availability > consistency).

**Q47. Strong vs eventual consistency?**
Strong = har read latest (slow, coordination). Eventual = thodi der me consistent (fast).

**Q48. Quorum kya?**
W + R > N → strong consistency. Tune W/R for read vs write performance.

**Q49. PACELC?**
CAP + normal operation me Latency vs Consistency trade-off (partition na bhi ho).

**Q50. Consensus algorithms?**
Paxos (complex), Raft (simpler, leader-based), 2PC (distributed txn, blocking).

**Q51. Eventual consistency kaha ok?**
Likes count, view count, feeds, DNS — thodi stale chalega. Banking me nahi.

### 📨 Message Queues

**Q52. Message queue kyun?**
Decoupling, async processing, buffering (spike absorb), reliability (consumer down → safe).

**Q53. Kafka vs RabbitMQ?**
Kafka = distributed log, high throughput, retention/replay. RabbitMQ = broker, routing, task queues.

**Q54. Queue vs pub-sub?**
Queue = ek message ek consumer (work split). Pub-sub = ek message sab subscribers (broadcast).

**Q55. Delivery guarantees?**
At-most-once (miss possible), at-least-once (dup possible → idempotent), exactly-once (hard).

**Q56. Idempotent consumer kyun?**
At-least-once me duplicate messages → idempotent processing (same message 2x = same result).

**Q57. Dead letter queue?**
Repeatedly fail messages DLQ me (poison message main queue block na kare). Baad me inspect.

**Q58. Event-driven architecture?**
Components events emit/consume (loose coupling). Naya consumer add without touching producer.

**Q59. Back-pressure kya?**
Consumer slow → producer ko slow karo (queue full signal) — overload se bachao.

### 🔌 Networking

**Q60. DNS kaise kaam karta?**
Domain → IP. Hierarchical (root → TLD → authoritative). Cached (TTL).

**Q61. HTTP methods?**
GET (read), POST (create), PUT (replace), PATCH (partial), DELETE. Idempotency: GET/PUT/DELETE
idempotent, POST nahi.

**Q62. Important status codes?**
200 ok, 201 created, 301 moved, 400 bad request, 401 unauth, 403 forbidden, 404 not found,
429 rate limit, 500 server error, 503 unavailable.

**Q63. TCP vs UDP?**
TCP = reliable, ordered (web, DB). UDP = fast, best-effort (streaming, gaming, DNS).

**Q64. WebSocket kab?**
Real-time bidirectional (chat, live scores, notifications). Persistent connection, server push.

**Q65. Long polling vs WebSocket vs SSE?**
Long polling = client repeat request. WebSocket = bidirectional persistent. SSE = server one-way push.

**Q66. REST vs gRPC?**
REST = HTTP/JSON (public, simple). gRPC = HTTP2/Protobuf (fast, internal microservices, streaming).

**Q67. GraphQL kab?**
Client exact data maangta (no over/under-fetching). Flexible clients (mobile). Complex to cache.

**Q68. API versioning kaise?**
URL (`/v1/`), header, ya query param. Backward compatibility rakho.

**Q69. Idempotency key kya?**
Client-generated unique key — POST retry pe duplicate na ho (payment double na ho).

### 🧩 Microservices

**Q70. Monolith vs microservices?**
Monolith = ek deploy (simple start). Microservices = independent services (per-service scale/deploy,
distributed complexity).

**Q71. Kab microservices?**
Team/scale grow, independent deploy chahiye. Start monolith, split jab zaroorat.

**Q72. API Gateway kya?**
Single entry — routing, auth, rate limiting, SSL, aggregation. Cross-cutting ek jagah.

**Q73. Service discovery?**
Dynamic services ki location (registry: Consul/Eureka). Client-side ya server-side.

**Q74. Circuit breaker?**
Service fail hote rahein → circuit open (fail fast) → half-open (test) → closed. Cascading failure rokta.

**Q75. Saga pattern?**
Distributed transaction — local transactions + compensating actions (undo). Choreography ya orchestration.

**Q76. Database per service?**
Har service apna DB (loose coupling). Cross-service = API/events (shared DB anti-pattern).

**Q77. Retry with backoff?**
Fail pe retry, exponential backoff (1s,2s,4s) + jitter (thundering herd se bachao).

**Q78. Graceful degradation?**
Ek feature down → baaki chale (recommendations down → feed still works).

**Q79. Bulkhead pattern?**
Resources isolate — ek service failure poora system na kha jaye (ship ke compartments jaisa).

### 💾 Storage + Security

**Q80. Object storage (S3) kab?**
Media/files/backups — cheap, scalable, HTTP. DB me sirf URL/metadata.

**Q81. Large files kaise handle?**
S3 me daalo (DB me nahi), metadata DB me. Serve via CDN. Pre-signed URL for direct upload.

**Q82. AuthN vs AuthZ?**
Authentication = kaun ho (identity). Authorization = kya kar sakte (permissions).

**Q83. JWT vs session?**
JWT = stateless (token me claims, easy scale). Session = server store (easy revoke).

**Q84. OAuth kya?**
Third-party authorization (login with Google) — password share nahi, token milta.

**Q85. Passwords kaise store?**
Hash + salt (bcrypt/Argon2). Never plain text. One-way (reverse nahi ho sakta).

**Q86. HTTPS kyun?**
Encryption in transit (TLS). Man-in-middle se bachao. Always use.

**Q87. Rate limiting kyun?**
API abuse, brute-force, DDoS se bachao. Token bucket / sliding window. 429 return.

**Q88. DDoS se kaise bachein?**
Rate limiting, WAF, CDN (absorb), IP blacklist, auto-scaling.

### 📊 Observability + Estimation

**Q89. Monitoring ke 3 pillars?**
Logging (events), Metrics (numbers over time), Tracing (request journey).

**Q90. p99 latency kya, kyun?**
99th percentile — 1% slowest requests. Average se important (tail latency = worst user experience).

**Q91. QPS kaise estimate?**
DAU × actions/user/day / 86400. Peak = 2-3× average.

**Q92. Storage kaise estimate?**
records/day × size × retention × replication factor.

**Q93. Bandwidth estimate?**
QPS × payload size.

**Q94. Latency numbers yaad?**
RAM ~100ns, SSD ~16μs, datacenter RT ~500μs, disk seek ~10ms, cross-continent ~150ms.

### 🔥 Advanced / Senior

**Q95. Idempotency HLD me kaise ensure?**
Idempotency key (client) + dedup store (Redis) — retry safe.

**Q96. Distributed lock kaise?**
Redis (SETNX + TTL, Redlock) ya ZooKeeper. ⚠ TTL expiry vs work time, fencing tokens.

**Q97. Unique ID generation (distributed)?**
Snowflake = timestamp + machineId + sequence. Unique, sortable, no central bottleneck. UUID (random).

**Q98. Hot partition kaise fix?**
Better shard key, split hot partition, cache hot data, add randomness to key.

**Q99. Data migration (zero downtime)?**
Dual writes (old + new), backfill, verify, switch reads, cleanup. Feature flags.

**Q100. Bloom filter kya?**
Probabilistic set membership — "definitely not in" ya "maybe in". Memory efficient. Cache
penetration, dedup me.

**Q101. Write-ahead log (WAL)?**
Change pehle log me likho (durable), phir apply. Crash recovery, replication ka base.

**Q102. CQRS kya?**
Command Query Responsibility Segregation — read/write models alag. Read-optimized views.

**Q103. Backpressure vs rate limiting?**
Rate limiting = incoming reject. Backpressure = downstream slow signal upstream ko.

**Q104. Geo-distribution kyun?**
Users ke paas data (latency), disaster recovery, compliance (data residency).

**Q105. Blue-green vs canary deployment?**
Blue-green = 2 environments, switch. Canary = thode users ko naya (gradual rollout, risk kam).

---

<a name="h17"></a>
## 17. Building blocks toolbox (har design me ye components)

System design "lego blocks" ka game hai. Ye components yaad rakho — jab jo problem ho tab pick karo.

### 🧱 Component → kab use
| Component | Problem jo solve karta | Kab add karo |
|---|---|---|
| **Load Balancer** | traffic distribute, HA | multiple servers hote hi |
| **Cache (Redis)** | read latency, DB load | read-heavy, hot data |
| **CDN** | static content latency | images/video/JS/CSS |
| **Message Queue** | async, decouple, buffer | slow tasks, spike absorb, decoupling |
| **Read Replicas** | read scaling | read-heavy DB |
| **Sharding** | write scaling, storage | single DB overwhelmed |
| **Object Storage (S3)** | large files cheap | media, backups |
| **Search Engine (ES)** | full-text search | search feature |
| **API Gateway** | routing, auth, rate limit | microservices |
| **Blob + metadata split** | large objects | files with searchable metadata |
| **Consistent Hashing** | even distribution, smooth resharding | distributed cache/DB |
| **Bloom Filter** | fast "not exists" check | cache penetration, dedup |
| **WebSocket** | real-time push | chat, live updates |

### 🔄 Standard data flow (bahut systems me common)
```
Client
  │
  ▼
DNS ──> CDN (static) ──> Load Balancer
                            │
                            ▼
                        API servers (stateless)
                       /        │         \
                  Cache      Message      Search
                  (Redis)     Queue       (ES)
                     │          │
                     ▼          ▼
                  Database    Workers ──> DB / S3 / notifications
                (replicas +
                 shards)
```

### 📐 Design decisions — quick guide
```
Read-heavy?          -> Cache + read replicas + CDN
Write-heavy?         -> Sharding + write-optimized DB (Cassandra) + queue
Strong consistency?  -> SQL / CP system, sync replication, quorum
High availability?   -> AP system, async replication, multi-region
Large files?         -> S3 + CDN, metadata in DB
Real-time?           -> WebSockets, pub-sub
Search?              -> Elasticsearch
Async work?          -> Message queue + workers
Global users?        -> CDN + geo-distribution + multi-region DB
Unpredictable spike? -> Auto-scaling + queue (buffer)
```

---

<a name="h18"></a>
## 18. Common trade-offs (senior-level thinking)

Interview me trade-offs bolna maturity dikhata. Har decision ka cost hota:

| Trade-off | Option A | Option B |
|---|---|---|
| **Consistency vs Availability** | strong (CP) — accurate, slower | eventual (AP) — fast, stale possible |
| **Latency vs Consistency** | async replication (fast, lag) | sync (consistent, slow) |
| **SQL vs NoSQL** | SQL (joins, ACID) | NoSQL (scale, flexible) |
| **Normalization vs Denormalization** | normalize (consistent, joins) | denormalize (fast reads, dup) |
| **Fan-out on write vs read** | write (fast read, celebrity issue) | read (cheap write, slow read) |
| **Push vs Pull** | push (real-time, resource) | pull (efficient, delay) |
| **Cache freshness vs load** | short TTL (fresh, DB load) | long TTL (stale, less load) |
| **Monolith vs Microservices** | monolith (simple, coupled) | micro (scalable, complex) |
| **Sync vs Async** | sync (simple, coupled, slow) | async (decoupled, complex, eventual) |
| **Vertical vs Horizontal** | vertical (simple, limited) | horizontal (scalable, distributed) |
| **Batch vs Stream** | batch (efficient, delayed) | stream (real-time, complex) |
| **Cost vs Performance** | cheap (slower, fewer resources) | expensive (fast, redundant) |

> ⭐ **Interview line:** "Main [option] choose karunga kyunki [requirement]. Iska cost [downside]
> hai, jise [mitigation] se handle karunga." — ye maturity dikhata hai.

---

<a name="h19"></a>
## 19. Numbers cheat sheet (interview se 1 ghanta pehle)

### Latency ladder (memorize)
```
L1 cache          ~1 ns
Branch mispredict ~3 ns
L2 cache          ~4 ns
Mutex lock/unlock ~17 ns
Main memory (RAM) ~100 ns
Compress 1KB      ~2 μs
SSD random read   ~16 μs
Datacenter RT     ~500 μs
Disk seek (HDD)   ~10 ms
India -> US RT    ~150 ms
```
**Takeaway:** RAM 1000x faster than SSD, SSD 1000x faster than network cross-continent. **Cache!**

### Data size shortcuts
```
1 char       = 1 byte (ASCII), 2-4 (UTF-8/16)
1 int        = 4 bytes
1 timestamp  = 8 bytes (long)
UUID         = 16 bytes
1 KB ≈ 10^3, 1 MB ≈ 10^6, 1 GB ≈ 10^9, 1 TB ≈ 10^12, 1 PB ≈ 10^15
```

### Time shortcuts
```
1 day     = 86,400 sec ≈ 10^5 (round for quick math)
1 month   ≈ 2.5M sec
1 year    ≈ 31.5M sec ≈ 3×10^7
```

### Common estimation formulas
```
QPS (avg)   = DAU × actions_per_day / 86400
QPS (peak)  = 2-3 × avg QPS
Storage     = records/day × size × retention_days × replication_factor
Bandwidth   = QPS × avg_payload_size
Cache size  = hot_data_fraction × total_data (usually 20% = 80% traffic)
Servers     = peak_QPS / QPS_per_server
```

### Availability nines
```
99%      -> 3.65 days/year   down
99.9%    -> 8.76 hours/year
99.99%   -> 52.6 min/year
99.999%  -> 5.26 min/year
```

### Single server rough capacity (order of magnitude)
```
~1000s QPS (simple requests)
~10-100 GB RAM
~TBs disk
-> beyond this, scale horizontally
```

---

<a name="h20"></a>
## 20. Glossary (jaldi reference)

| Term | Matlab |
|---|---|
| **QPS/RPS** | queries/requests per second |
| **DAU/MAU** | daily/monthly active users |
| **Latency** | ek request ka time |
| **Throughput** | per second processed |
| **p50/p95/p99** | percentile latency (p99 = 1% slowest) |
| **SPOF** | single point of failure |
| **CAP** | Consistency, Availability, Partition tolerance |
| **ACID** | Atomicity, Consistency, Isolation, Durability (SQL) |
| **BASE** | Basically Available, Soft state, Eventual (NoSQL) |
| **Sharding** | data across multiple DBs (horizontal) |
| **Replication** | data copies (read scaling, HA) |
| **Consistent hashing** | hash ring, smooth resharding |
| **Fan-out** | ek write → multiple destinations |
| **Cache-aside** | app manages cache (lazy load) |
| **TTL** | time-to-live (expiry) |
| **CDN** | edge content cache |
| **WAL** | write-ahead log (durability) |
| **2PC** | two-phase commit (distributed txn) |
| **Saga** | distributed txn with compensations |
| **Quorum** | W+R>N for consistency |
| **Idempotency** | same op kai baar = same result |
| **Backpressure** | downstream slow signal upstream |
| **Circuit breaker** | fail-fast on repeated failures |
| **Thundering herd** | mass simultaneous requests (cache miss) |
| **Bloom filter** | probabilistic set membership |
| **Snowflake ID** | distributed unique sortable ID |
| **Geohash/QuadTree** | geospatial indexing |
| **Eventual consistency** | thodi der me consistent |
| **Read replica** | read-only DB copy |
| **Hot key/partition** | disproportionate load on one |
| **WebSocket** | persistent bidirectional connection |
| **Pub-Sub** | publisher → many subscribers |
| **DLQ** | dead letter queue (failed messages) |
| **Blue-green** | 2 environments, switch deploy |
| **Canary** | gradual rollout to subset |

---

<a name="h18"></a>
## 21. Do's & Don'ts

### ✅ DO
- **Requirements + scope pehle** (functional + non-functional) — kabhi turant architecture nahi
- **Estimate karo** (QPS, storage, bandwidth) — scale decisions ground pe rakho
- **Structured raho** (RESHADED framework follow karo)
- **Draw karo** — components + data flow (interviewer ko dikhe)
- **Deep dive** ek-do components me (breadth + depth dono)
- **Trade-offs khud bolo** ("ye kiya kyunki..., cost ye hai")
- **Bottlenecks identify karo** (SPOF, hot key, scaling limits)
- **Collaborative raho** — interviewer se confirm karte raho
- **Start simple, then scale** (single server → add components as needed)
- **Numbers justify karo** (kyun ye DB, kyun cache)

### ❌ DON'T
- Turant architecture (requirements skip) 🚫
- Ek hi component pe atak jaana (breadth bhi chahiye) 🚫
- Buzzwords bina samajh ("Kafka daal do" bina kyun) 🚫
- Over-engineering (100 users ke liye 50 microservices) 🚫
- Trade-offs ignore ("ye best hai" — kuch best nahi, sab trade-off) 🚫
- Estimation skip ("bahut zyada scale" — number do) 🚫
- SPOF chhodna (har box ka redundancy socho) 🚫
- Chup rehna (think out loud) 🚫

### 🗣️ Interviewer ke common deep-dive questions
1. "Ye 10x scale kaise karega?" → cache, shard, replica, CDN, queue
2. "Is component ka failure to?" → redundancy, failover, graceful degradation
3. "Consistency ya availability?" → use case ke hisaab se justify (CAP)
4. "Hot key/celebrity kaise handle?" → replicate, local cache, hybrid fan-out
5. "Data kaise store karoge?" → SQL/NoSQL justify, schema, sharding
6. "Real-time kaise?" → WebSocket, pub-sub
7. "Cost kaise kam karoge?" → caching, cold storage, right-sizing

---

<a name="h22"></a>
## 22. Distributed systems deep concepts (senior)

### 22.1 — Idempotency (distributed)
Same request kai baar aaye (retry, network) to side-effect ek hi baar. Kaise:
```
1. Client idempotency key generate kare (UUID per operation)
2. Server: key ko dedup store (Redis) me check
   - key exists -> pehle wala result return (no re-process)
   - key naya -> process + store key + result
```
> Repo `Ecommerce`/`GPay` LLD me clientRequestId isi ka LLD-level roop.

### 22.2 — Exactly-once processing (myth + reality)
"Exactly-once delivery" networks me impossible (FLP). Reality: **at-least-once delivery +
idempotent processing = effectively exactly-once**. Kafka transactions isse achieve karte
(idempotent producer + transactional consumer).

### 22.3 — Distributed locks
Multiple nodes ek resource pe exclusive access:
- **Redis (SETNX + TTL):** `SET key value NX PX 30000`. ⚠ TTL work se pehle expire ho sakta.
- **Redlock:** multiple Redis instances pe majority lock (safer).
- **ZooKeeper:** ephemeral nodes (session mari to lock release).
- **Fencing token:** lock ke saath monotonic token — stale lock holder detect.
> ⚠ Distributed lock avoid karo agar possible — optimistic concurrency (version) better.

### 22.4 — Clocks + ordering
- **Physical clocks unreliable** (drift, NTP sync). Events ka order galat ho sakta.
- **Lamport timestamps:** logical counter (causality — "happened before").
- **Vector clocks:** per-node counters (concurrent vs causal detect). Dynamo use karta.
- **TrueTime (Google Spanner):** GPS + atomic clocks, bounded uncertainty.

### 22.5 — Heartbeats + failure detection
Nodes periodically "alive" signal bhejte. Miss → suspected dead → failover. ⚠ network partition
me "dead" galat ho sakta (split-brain).

### 22.6 — Gossip protocol
Nodes randomly ek doosre ko state batate (epidemic spread). Cassandra membership. Scalable,
eventually consistent cluster view.

### 22.7 — Leader election
Ek node leader (coordination). Raft/Paxos se elect. Leader mara → re-elect. Split-brain se
bachne ke liye quorum.

### 22.8 — Split-brain problem
Network partition → dono halves apna leader elect kar lein → conflicting writes. Fix: quorum
(majority side hi active), fencing.

### 22.9 — Write-ahead log (WAL) + event sourcing
- **WAL:** change pehle durable log me, phir apply. Crash recovery + replication base.
- **Event sourcing:** state ko events ki sequence se derive (current snapshot ke bajaye). Full
  audit trail, replay possible, time-travel.

### 22.10 — Batch vs Stream processing
- **Batch:** bade data chunks periodically (Hadoop/Spark). Efficient, high latency (hours).
- **Stream:** real-time event-by-event (Kafka Streams, Flink). Low latency, complex.
- **Lambda architecture:** batch (accurate) + stream (fast) dono, merge.

---

<a name="h23"></a>
## 23. Deep-dive worked example — Design WhatsApp (full)

Ye section 15 se **zyada detail** me — poora RESHADED with math, schema, flows.

### R — Requirements
```
Functional: 1-1 chat, group chat (up to 256), online/last-seen, delivery+read receipts,
            media sharing, message history
Non-functional: real-time (<100ms delivery), 99.99% available, ordering per conversation,
                2B users, message durability
```

### E — Estimation
```
Users: 2B total, 500M DAU
Messages: 50B messages/day
  Writes/sec = 50B / 86400 ≈ 580K messages/sec (peak ~1.5M/sec)
Storage: 50B/day × 100 bytes × 365 × several years... but messages often deleted after delivery
  Persistent: assume 30 days retention -> 50B × 100B × 30 = 150 TB
Connections: 500M concurrent WebSocket connections (huge — connection servers)
```

### A — API
```
WebSocket: persistent connection per online user
  sendMessage(toUser, content) -> ack
  receiveMessage (server push)
  updateStatus(delivered/read)
REST: getHistory(conversationId, before, limit), uploadMedia -> S3 url
```

### S — Data model
```
messages (Cassandra — write-heavy, per-conversation partition):
  conversationId (partition key) | messageId (clustering, time-sorted) | senderId | content | timestamp | status
conversations: conversationId | participants[] | type(1-1/group) | lastMessageAt
user_connections (Redis): userId -> connectionServerId (kaunse server pe connected)
presence (Redis): userId -> online/lastSeen (TTL)
```

### H — Architecture
```
Client <--WebSocket--> Connection Servers (stateful, hold connections)
                            │
                            ▼
                       Message Service ──> Cassandra (messages)
                            │           ──> Redis (presence, connection registry)
                            ▼
                       Notification Service (offline users -> FCM/APNS push)
                       Media Service -> S3 + CDN
```

### D — Deep dive: message delivery flow
```
A sends message to B:
1. A's WebSocket -> A's connection server -> Message Service
2. Message Service: persist to Cassandra (durability first!)
3. Lookup B in connection registry (Redis)
   - B online -> route to B's connection server -> push via B's WebSocket -> DELIVERED ack
   - B offline -> store + send push notification (FCM/APNS)
4. B opens chat -> READ receipt -> back to A (via A's connection)
```

**Message states:** SENT (server got) → DELIVERED (B's device got) → READ (B opened).
**Group chat:** message → sab members ka fan-out (online: WebSocket, offline: push). Group me
har member ka delivery/read status track (large groups me expensive — batch/summary).

### E — Evaluate + scale
- **Connection servers:** 500M connections — har server ~65K connections → ~10K servers.
  Connection registry (Redis) se routing.
- **Message ordering:** per-conversation sequence number ya timestamp (clustering key).
- **Cassandra:** write-heavy perfect (LSM tree). Partition by conversationId.
- **Media:** S3 + CDN, WebSocket se sirf URL (file nahi).

### D — Trade-offs
- **Durability first vs latency:** persist before deliver (message loss na ho) — thoda latency.
- **Delivery guarantee:** at-least-once + client dedup (messageId).
- **E2E encryption:** messages device pe encrypt (server plaintext nahi dekhta — Signal protocol).

---

<a name="h24"></a>
## 24. Deep-dive worked example — Design Uber (full)

### R — Requirements
```
Functional: rider request ride, driver-rider matching, real-time location, pricing (surge),
            trip lifecycle, ratings
Non-functional: low latency matching (<seconds), real-time location, high availability,
                accurate geospatial, 100M users
```

### E — Estimation
```
Users: 100M, active drivers: 5M
Location updates: 5M drivers × every 4 sec = 1.25M updates/sec (huge write load!)
Ride requests: assume 10M rides/day = ~115/sec avg, peak ~500/sec
Storage: trips = 10M/day × 1KB × 365 × years... few TB
```

### S — Data model
```
trips: tripId | riderId | driverId | status | pickup(lat,lng) | drop | fare | timestamps
driver_locations (Redis geo — ephemeral, high write):
  GEOADD drivers <lng> <lat> <driverId>   (Redis geospatial)
drivers: driverId | status(available/busy/offline) | rating | vehicle
```

### H — Architecture
```
Rider app -> LB -> Trip Service -> Matching Service -> Location Service (Redis Geo / QuadTree)
Driver app -> Location updates (every 4s) -> Location Service (high write, sharded by region)
                                          -> Pricing Service (surge = demand/supply per area)
Notification (WebSocket/push for real-time trip updates)
```

### D — Deep dive: matching flow
```
1. Rider requests ride (pickup location)
2. Matching Service -> Location Service: nearby available drivers
   - Geohash/QuadTree: pickup cell + adjacent cells ke drivers (O(1)-ish)
   - Redis: GEORADIUS drivers <lng> <lat> 2 km
3. Rank candidates (distance/ETA/rating) -> offer to nearest
4. Driver accepts -> trip created (REQUESTED -> MATCHED)
5. Both track each other real-time (WebSocket location stream)
6. STARTED -> COMPLETED -> fare charged -> ratings
```

### ⭐ Geospatial indexing (core)
- **Geohash:** lat/lng → string. Same prefix = nearby. Grid cells. Simple, uneven in dense areas.
- **QuadTree:** recursive grid subdivision — dense areas (cities) me zyada cells, sparse me kam.
  Balanced. Nearby query = traverse to cell + neighbors.
- **Redis GEO:** built-in geospatial (GEOADD, GEORADIUS) — practical choice.

### ⭐ Surge pricing
```
For each area/cell:
  demand = active ride requests
  supply = available drivers
  surge_multiplier = f(demand / supply)   // 1.0x to 3.0x
  fare = (base + perKm × dist + perMin × time) × surge_multiplier
```

### E — Scale + trade-offs
- **Location writes (1.25M/sec):** shard Location Service by geo region. Redis per region.
  Old locations TTL (ephemeral — no durability needed).
- **Matching per region:** locality (rider aur drivers same region).
- **Trip state:** state machine, persisted (durability). Trip DB sharded by tripId/region.
- **Trade-off:** location accuracy (frequent updates = more load) vs freshness. 4 sec balance.

---

<a name="h25"></a>
## 25. Full mock interview transcript — Design a URL Shortener

> Realistic 45-min flow. **Dhyan: candidate structured hai (RESHADED), estimate karta, trade-offs bolta.**

> **Interviewer:** "Design a URL shortener like TinyURL."

> **Candidate:** "Pehle requirements clarify karta hu.
> **Functional:** long URL → short URL, short → redirect. Custom alias chahiye? Expiry? Analytics?
> **Non-functional:** scale kitna? Latency? Availability?"

> **Interviewer:** "Custom alias optional, expiry optional. Analytics basic (click count). Scale
> maano 100M new URLs/day. Fast redirects zaroori."

> **Candidate:** "Great. To ye **read-heavy** system hoga — redirects >> creates. Estimate karta hu:
> - Writes: 100M/day = ~1,160/sec
> - Reads (100:1 ratio): ~116K/sec — **caching critical**
> - Storage: 100M × 365 × 5 years × 500 bytes ≈ 91 TB
> - Ye scale pe single DB nahi chalega — sharding + cache chahiye."

> **Interviewer:** "Short code kaise generate karoge?"

> **Candidate:** "Kuch options hain:
> 1. **Counter + Base62** — global counter, base62 encode. 7 chars = 3.5 trillion URLs. Unique
>    guaranteed, chhota. Problem: distributed counter — main range allocation karunga (har server
>    ko counter range de do, ya ZooKeeper/ticket server).
> 2. **Hash (MD5)** — hash(url) first 7 chars. Problem: collision — retry with salt.
> 3. **Random** — random 7 chars, DB collision check.
> Main **counter + Base62** lunga — no collision, guaranteed short. Custom alias ke liye alag
> path (user-provided, uniqueness check)."

> **Interviewer:** "Data model?"

> **Candidate:**
> ```
> url_mappings: shortCode (PK) | longUrl | userId | createdAt | expiry | clickCount
> ```
> "Key-value access pattern hai (shortCode se lookup) — **NoSQL** (DynamoDB/Cassandra) perfect,
> horizontal scale. shortCode pe shard."

> **Interviewer:** "Ab architecture aur redirect flow."

> **Candidate:**
> ```
> Client -> DNS -> LB -> App servers (stateless) -> Cache (Redis) -> DB (NoSQL, sharded)
>                                                 -> Analytics (async via queue)
> ```
> "**Redirect flow (hot path):**
> ```
> GET /abc123 -> Redis.get(abc123)
>    hit  -> 301 redirect (fast, most cases)
>    miss -> DB lookup -> cache.set(TTL) -> 301 redirect
> ```
> Cache me hot URLs (80% traffic = 20% URLs — Pareto). Isse DB load bahut kam."

> **Interviewer:** "301 ya 302 redirect?"

> **Candidate:** "Trade-off hai:
> - **301 (permanent):** browser cache karta — subsequent hits server tak aate hi nahi. Fast,
>   par analytics miss (click count kam).
> - **302 (temporary):** har hit server pe aata — accurate analytics, par zyada load.
> Analytics chahiye to 302, pure speed chahiye to 301. Main **302** lunga kyunki click count
> requirement hai, aur caching se load handle kar lunga."

> **Interviewer:** "Scale + failure handling?"

> **Candidate:**
> - "**Cache:** Redis cluster, hot URLs. Cache down → DB fallback (degraded, not down).
> - **DB:** sharded by shortCode hash, read replicas.
> - **Analytics:** async (queue + workers) — redirect block na ho. Eventual click count ok.
> - **CDN:** redirect response bhi edge pe cache ho sakta (301 case).
> - **Availability:** ye AP system — redirect available rahe (thodi stale analytics ok). CAP me
>   availability > consistency yahan."

> **Interviewer:** "Custom alias collision?"

> **Candidate:** "Custom alias insert pe uniqueness check (DB unique constraint / conditional
> write). Collision → user ko error, doosra alias maango. Auto-generated codes counter se
> unique, no collision."

> **⭐ Ye transcript kya sikhata:**
> 1. RESHADED structure (requirements → estimation → data → architecture → deep dive → trade-offs)
> 2. Estimation se decisions justify (read-heavy → cache)
> 3. Options diye + ek pick + kyun (counter vs hash)
> 4. Trade-offs khud raise (301 vs 302, CAP choice)
> 5. Failure handling proactively (cache down fallback)

---

## 🎯 Final strategy (interview room me)

```
┌──────────────────────────────────────────────────────────────────┐
│  R - Requirements  (functional + non-functional + scope)          │
│  E - Estimation    (QPS, storage, bandwidth — read:write ratio)   │
│  S - Storage/Schema (SQL vs NoSQL, data model)                    │
│  H - High-level    (draw components + data flow)                  │
│  A - API design    (key endpoints)                                │
│  D - Deep dive     (1-2 components in detail)                     │
│  E - Evaluate      (bottlenecks, SPOF, scaling)                  │
│  D - Discuss       (trade-offs — CAP, cost, consistency)          │
└──────────────────────────────────────────────────────────────────┘
```

**Yaad rakho:**
- HLD me **koi single right answer nahi** — trade-offs matter karte hain
- **Start simple, then scale** (single server → add components as bottlenecks aayein)
- **Estimate** karo (numbers se decisions justify)
- **Draw** karo (visual > words)
- **Think out loud + collaborate** (interviewer se confirm karte raho)
- Har component pe poocho: "ye fail hua to? scale kaise? consistency kya?"

**Combine with `LLD_Interview.md`** — HLD (ye file) poore system ka, LLD ek component ke andar ka.
Dono padho, dono FAANG interviews me aate hain. All the best! 🚀

---

<a name="h26"></a>
## 26. More deep-dive worked examples

### 26.1 — Design Twitter / News Feed (full)

**R — Requirements**
```
Functional: post tweet, follow, home timeline, like/retweet, search (basic)
Non-functional: 200M DAU, read-heavy (timeline views >> tweets), low latency feed (<200ms),
                eventual consistency ok (feed thodi stale chalega)
```

**E — Estimation**
```
DAU: 200M. Tweets: 100M/day = ~1,160/sec (peak ~3K/sec)
Timeline reads: har user ~10 timeline loads/day = 2B reads/day = ~23K/sec (peak ~70K/sec)
  READ-HEAVY -> pre-compute timelines + cache
Storage: 100M tweets/day × 300 bytes × 365 × 5 = ~55 TB (text). Media separate (S3).
```

**S — Data model**
```
tweets:  tweetId | userId | content | mediaUrl | createdAt        (Cassandra)
follows: followerId | followeeId                                   (who follows whom)
timeline (Redis): userId -> [tweetId, ...]  (pre-computed, capped ~800)
users:   userId | name | handle | followerCount
```

**H — Architecture**
```
Client -> LB -> API -> Tweet Service -> Cassandra + Fan-out Queue
                    -> Timeline Service -> Redis (timelines)
                    -> Media -> S3 + CDN
                    -> Search -> Elasticsearch
Fan-out Workers: tweet -> followers ki timeline cache me push
```

**D — Deep dive: fan-out (core challenge)**
```
Fan-out on write (push): tweet post -> sab followers ki timeline me add (Redis)
  + read fast (pre-computed)
  - celebrity (50M followers) = 50M writes per tweet! (hot problem)

Fan-out on read (pull): timeline maango -> followed users ke recent tweets fetch + merge
  + write cheap
  - read slow (merge on the fly)

HYBRID (best): 
  - normal users -> push (fan-out on write)
  - celebrities (>threshold followers) -> DON'T push
  - read time: pre-computed timeline (Redis) + celebrity tweets (pull) -> merge -> sort
```

**E — Scale + trade-offs**
- Timeline in Redis (fast reads), capped (~800 tweets — older paginate from DB)
- Tweets in Cassandra (write-heavy), sharded by tweetId
- Media on S3 + CDN
- Search async indexed to Elasticsearch
- **Trade-off:** eventual consistency (feed thodi stale ok), celebrity hybrid (complexity vs scale)

---

### 26.2 — Design Instagram (full)

**R — Requirements:** upload photo, feed, follow, like, comment, stories, explore.

**E — Estimation**
```
DAU: 500M. Photo uploads: 100M/day. Feed views: massive (read-heavy).
Media storage: 100M × 2MB avg = 200 TB/day (!!) -> S3 + CDN critical, storage tiering
```

**S — Data model**
```
photos: photoId | userId | s3Url | caption | createdAt | likeCount    (metadata in DB)
Media: S3 (multiple resolutions) + CDN
feed (Redis): userId -> [photoId, ...]  (fan-out, hybrid like Twitter)
```

**H — Architecture**
```
Upload: client -> pre-signed S3 URL -> DIRECT upload to S3 (app server bypass!)
                -> metadata to DB -> image processing queue (resize/compress)
Serve:  feed -> DB metadata -> CDN (images)
```

**D — Deep dive**
- **Upload:** pre-signed URL se client seedha S3 pe upload (app server load bachao). Metadata
  DB me, processing async (thumbnails, multiple resolutions, filters).
- **Feed:** hybrid fan-out (Twitter jaisa). Photos immutable → aggressive caching.
- **Stories:** 24-hour TTL content — Redis with expiry, separate from permanent posts.
- **Explore:** ML-based recommendations (offline pipeline).

**Trade-offs:** photos immutable (cache-friendly), feed eventual consistency, storage tiering
(hot on SSD/CDN, cold on cheap storage).

---

### 26.3 — Design a Distributed Cache (like Redis Cluster)

**R — Requirements:** get/set/delete, TTL, eviction, distributed (multiple nodes), fault tolerant.

**⭐ Core concepts:**
- **Sharding (consistent hashing):** keys ko nodes pe distribute. Node add/remove → 1/N move.
- **Replication:** har shard ki replica (master-replica) — node fail → replica promote.
- **Eviction:** memory full → LRU/LFU (repo LLD me algorithms).
- **TTL:** expiry (lazy — access pe check, ya active — background sweep).

**Architecture**
```
Client -> (consistent hash) -> Cache Node (shard) -> master + replicas
Coordination: gossip protocol (membership), or config service (ZooKeeper)
```
**Deep dive:**
- **Consistent hashing ring** + virtual nodes (even distribution)
- **Replication:** async (fast, may lose recent) vs sync (consistent, slow)
- **Failure:** node down → replica promote, ring rebalance (only affected keys)
- **Hot key:** replicate hot key across nodes, client-side local cache

**Trade-offs:** consistency (async replication = possible loss) vs performance, memory vs hit-ratio.

---

### 26.4 — Design YouTube / Netflix (full)

**R — Requirements:** upload video, stream (adaptive), search, recommendations, comments.

**E — Estimation**
```
Uploads: 500 hours video/min (YouTube scale). Massive storage (PB) + bandwidth.
Reads >> writes (views). CDN bandwidth = dominant cost.
```

**⭐ Core: video processing pipeline**
```
Upload -> S3 (raw) -> Transcoding Queue -> Workers:
   - multiple resolutions (144p..4K)
   - multiple formats (HLS, DASH)
   - split into chunks (segments, ~10 sec each)
   -> processed chunks -> S3 -> CDN
```

**⭐ Streaming: adaptive bitrate**
```
Player -> manifest file (available qualities) -> requests chunks based on network speed
   good network -> 1080p chunks; poor -> 480p (adaptive, seamless)
Chunks served from CDN (edge, user ke paas) -> origin (S3) load minimal
```

**Architecture**
```
Upload -> S3 -> Queue -> Transcoding workers -> S3 -> CDN
Watch  -> Metadata service (DB) -> CDN (video chunks)
Search -> Elasticsearch. Recommendations -> ML pipeline (offline + online).
Analytics (views, watch time) -> stream processing (Kafka + Flink)
```

**Deep dive + trade-offs:**
- **Pre-transcode all resolutions** (storage cost, instant playback) vs **on-demand** (compute cost)
- **CDN** = critical (bandwidth cost + latency). Popular videos aggressively cached.
- **Thumbnails, preview** — pre-generated.
- **View count** — eventual (approximate, batched — exact real-time too expensive).

---

### 26.5 — Design a Notification System (full)

**R — Requirements:** multi-channel (email/SMS/push/in-app), templates, user preferences,
priority, at scale, delivery tracking.

**E — Estimation:** billions of notifications/day, spike-prone (marketing blasts).

**Architecture**
```
Event source -> Notification Service -> Message Queue -> Channel Workers:
   Email worker  -> SendGrid/SES
   SMS worker    -> Twilio
   Push worker   -> FCM (Android) / APNS (iOS)
   In-app worker -> DB + WebSocket
Preference check + template render + rate limit + dedup -> send -> track delivery
```

**Deep dive:**
- **Queue + workers:** async (event source block na ho), spike absorb, per-channel scaling.
- **User preferences:** opt-out, quiet hours, channel choice — check before send.
- **Rate limiting:** spam se bachao (per-user notification cap).
- **Dedup:** same notification 2x na jaye (idempotency key).
- **Priority:** OTP (high) vs marketing (low) — separate queues.
- **Retry + DLQ:** delivery fail → retry with backoff → DLQ.
- **Delivery tracking:** sent/delivered/opened → analytics + retry decisions.

**Trade-offs:** at-least-once (dup risk, dedup needed) vs complexity, priority queues (fairness).

---

### 26.6 — Design a Payment System (full) — repo LLD: `GPay`, `Razorpay`

**R — Requirements:** process payments, multiple methods, idempotency, consistency (no
double-charge, no money loss), ledger, refunds.

**⭐ Core: consistency (this is CP, not AP!)**
- Money me **strong consistency** zaroori — double-charge ya money-loss unacceptable.
- **Idempotency:** payment retry pe double-charge na ho (idempotency key).
- **Double-entry ledger:** har transaction 2 entries (debit + credit) — audit + reconciliation.

**Architecture**
```
Client -> Payment Service (idempotency check) -> Payment Gateway (Stripe/Razorpay)
       -> Ledger Service (double-entry, strong consistency, SQL)
       -> Reconciliation (async, gateway vs ledger match)
       -> Notification (receipt)
```

**Deep dive:**
- **Idempotency:** client key → dedup store → same key = same result (no re-charge).
- **State machine:** INITIATED → PENDING → SUCCESS/FAILED (crash-safe intermediate state).
- **Compensating transaction:** debit success, credit fail → reversal (repo GPay LLD me bug fix).
- **2PC / Saga:** distributed (debit one account, credit another) — atomicity.
- **Reconciliation:** async job — gateway records vs internal ledger match (mismatches flag).

**Trade-offs:** consistency > availability (CP), sync processing (slower but safe), audit
(storage) vs compliance.

---

### 26.7 — Design Search Autocomplete / Typeahead (full)

**R — Requirements:** type ke saath top-5 suggestions, <100ms, ranked by popularity, at scale.

**⭐ Core: Trie + pre-computed top-k**
```
Trie (prefix tree): each node = character. Path = prefix.
At each node: pre-compute + cache top-k completions (by frequency)
Query "ca" -> traverse to "ca" node -> return cached top-5
```

**Architecture**
```
Query -> API -> Trie service (in-memory, cached) -> top-k suggestions
Offline: search logs -> aggregate frequencies -> rebuild Trie (periodic) -> deploy
```

**Deep dive:**
- **Trie in memory** (fast traversal). Top-k cached at each node (no runtime sort).
- **Ranking:** frequency/popularity. Update periodically (not real-time — expensive).
- **Sharding:** Trie by first letter/prefix (a-m one server, n-z another).
- **Client:** debounce (har keystroke pe request nahi — 200ms wait), cache recent.

**Trade-offs:** freshness (periodic rebuild = new terms delayed) vs performance, memory (Trie
size) vs speed, personalization (per-user vs global).

---

### 26.8 — Design a Rate Limiter (full) — repo LLD: algorithms

**R — Requirements:** limit requests per user/IP per window, distributed, low latency, accurate.

**Algorithms (LLD me detail — yahan distributed angle):**
```
Token Bucket: bucket me tokens, refill rate se. Request = 1 token. Burst allows.
Sliding Window Counter: current + previous window weighted. Practical accuracy.
```

**⭐ Distributed rate limiter (Redis)**
```
key = "rate:{userId}:{window}"
Lua script (atomic): 
   count = INCR key
   if count == 1: EXPIRE key <window>
   return count <= limit
```
- **Centralized (Redis):** accurate across servers, but network hop + Redis SPOF.
- **Local (per-server):** fast, but approximate (each server own count → total > limit).
- **Hybrid:** local + periodic sync.

**Architecture:** API Gateway / middleware level. 429 + Retry-After header on limit.
**Trade-offs:** accuracy (centralized) vs latency (local), Redis dependency, sliding vs fixed window.

---

### 26.9 — Design Dropbox / Google Drive (full)

**R — Requirements:** upload/download files, sync across devices, share, versioning, large files.

**E — Estimation:** billions of files, PB storage, sync events high. Read:write mixed.

**⭐ Core: chunking + dedup + sync**
```
Chunking: file -> fixed chunks (4MB). Sirf CHANGED chunks upload (delta sync — bandwidth bachao)
Dedup: chunk ka hash (SHA-256) -> agar hash pehle se exists to REFERENCE (storage bachao)
Sync: client watches local changes -> upload changed chunks -> notify other devices -> download
```

**Architecture**
```
Client (watcher) -> Metadata Service (file tree, chunk list, versions) -> Metadata DB
                 -> Block Service -> S3 (chunks, content-addressed by hash)
                 -> Notification Service (sync trigger to user's other devices)
```

**Data model**
```
files: fileId | userId | name | path | version | [chunkHashes] | updatedAt
chunks: hash (PK) -> S3 location, refCount (dedup)
devices: userId -> [deviceIds] (for sync notification)
```

**Deep dive:**
- **Delta sync:** only changed chunks transfer (client compares hashes).
- **Dedup:** same content (hash) stored once, referenced. Massive storage saving.
- **Conflict:** same file edited on 2 devices → conflict resolution (keep both, versioned).
- **Metadata vs data:** metadata DB (small, queryable), chunks S3 (large, cheap).

**Trade-offs:** chunk size (small = better dedup + more metadata; large = less overhead),
consistency (last-write-wins vs conflict copies).

---

### 26.10 — Design Google Docs (collaborative editing)

**R — Requirements:** multiple users edit same doc real-time, see each other's changes,
conflict-free, cursor presence.

**⭐ Core: real-time collaboration (hardest part)**
- **Operational Transformation (OT):** operations (insert/delete) transform against concurrent
  ops to maintain consistency. Google Docs uses this.
- **CRDT (Conflict-free Replicated Data Type):** data structure that merges concurrently without
  conflict (each char unique ID). Simpler reasoning, more memory.

**Architecture**
```
Client <--WebSocket--> Collaboration servers (per-doc session)
                       -> apply OT/CRDT -> broadcast to other editors
                       -> persist ops (WAL) -> Document store
Presence: who's editing, cursors (Redis, ephemeral)
```

**Deep dive:**
- **Real-time sync:** WebSocket, ops broadcast to all editors of doc.
- **Consistency:** OT/CRDT ensures all editors converge to same state despite concurrent edits.
- **Persistence:** operations log (event sourcing) → replay = current doc. Snapshots periodically.
- **Presence:** cursors + who's online (Redis, TTL).

**Trade-offs:** OT (efficient, complex server logic) vs CRDT (simpler, more memory/metadata),
consistency (convergence guarantee) vs latency.

---

### 26.11 — Design a Web Crawler

**R — Requirements:** crawl billions of web pages, extract links, store content, respect
politeness (robots.txt), avoid duplicates, freshness.

**⭐ Core: BFS + politeness + dedup**
```
Seed URLs -> URL Frontier (priority queue) -> Fetcher workers -> parse -> extract links
   -> new links back to frontier (BFS)
   -> content stored (S3)
Dedup: URL seen? (Bloom filter). Content dup? (content hash / SimHash)
Politeness: per-domain rate limit (server overload na ho), robots.txt respect
```

**Architecture**
```
URL Frontier (distributed queue, priority + politeness) -> Fetchers (distributed)
   -> Parser (extract links + content) -> Dedup (Bloom filter) 
   -> Content store (S3) -> Link graph (for ranking)
```

**Deep dive:**
- **URL frontier:** priority (important pages first) + politeness (per-domain queue, rate limit).
- **Dedup:** Bloom filter (URL seen — memory efficient, probabilistic). Content dedup (hash).
- **Distributed:** partition by domain/hash, coordinate via queue.
- **Freshness:** re-crawl based on change frequency (news daily, static rarely).
- **Traps:** infinite loops (calendar pages), deep URLs — depth limit, patterns detect.

**Trade-offs:** freshness (re-crawl cost) vs coverage, politeness (slow) vs speed, Bloom filter
(false positives = miss some pages) vs memory.

---

### 26.12 — Design a Leaderboard (real-time ranking)

**R — Requirements:** top-N players by score, player rank, real-time updates, millions of players.

**⭐ Core: Redis Sorted Sets**
```
ZADD leaderboard <score> <playerId>    -- add/update score O(log n)
ZREVRANGE leaderboard 0 9              -- top 10 O(log n + k)
ZREVRANK leaderboard <playerId>         -- player's rank O(log n)
ZINCRBY leaderboard <delta> <playerId>  -- increment score
```
Redis sorted set = perfect (score-ordered, O(log n) ops).

**Architecture**
```
Game -> Score Service -> Redis Sorted Set (leaderboard) -> real-time ranks
     -> persistent DB (backup, history)
For scale: shard by region/game, aggregate for global. Time-windowed (daily/weekly) = separate sets.
```

**Deep dive:**
- **Redis sorted set** primary (fast). DB backup for durability.
- **Scale:** millions of players — shard leaderboards (per region/game), global = merge top-N.
- **Time windows:** daily/weekly/all-time = separate sorted sets (TTL for daily).
- **Ties:** same score → secondary sort (timestamp — earlier first).

**Trade-offs:** Redis memory (all players in memory) vs approximate (only top-K exact), real-time
(every update) vs batched.

---

### 26.13 — Design Distributed Unique ID Generator

**R — Requirements:** unique IDs across distributed system, high throughput, roughly sortable
(time-ordered), no central bottleneck.

**Options:**
| Approach | Pros | Cons |
|---|---|---|
| **UUID** | no coordination, simple | 128-bit (big), not sortable |
| **DB auto-increment** | simple, sortable | central bottleneck, SPOF |
| **DB ticket server (range)** | batched (less contention) | still central |
| **Snowflake** ⭐ | sortable, distributed, 64-bit | clock dependency |

**⭐ Snowflake (Twitter) — 64 bits:**
```
[1 bit unused][41 bits timestamp][10 bits machineId][12 bits sequence]
  timestamp -> roughly time-sortable
  machineId -> per-machine (no coordination)
  sequence  -> per-ms counter (4096 IDs/ms/machine)
```
- **Time-sortable** (timestamp high bits), **distributed** (machineId), **high throughput**.
- ⚠ Clock skew/backward → duplicate risk (handle: wait ya sequence bit).

**Trade-offs:** Snowflake (sortable, clock-dependent) vs UUID (no deps, not sortable),
coordination (machineId assignment) vs simplicity.

---

### 26.14 — Design Ticketmaster / BookMyShow (high contention) — repo LLD: `IRCTC`

**R — Requirements:** browse events, book seats, **prevent double-booking** (high contention on
popular events), payment, waitlist.

**⭐⭐ Core: seat reservation (concurrency — THE challenge)**
```
1. User selects seats -> LOCK seats temporarily (5-10 min hold, with timeout)
2. Locked seats "reserved" -> koi aur nahi le sakta
3. Payment SUCCESS -> seats CONFIRMED (permanent)
4. Payment FAIL / TIMEOUT -> seats RELEASED (back to available)
```

**Locking approaches:**
- **Pessimistic (DB row lock / distributed lock):** lock seat during booking. Popular events me
  contention high — most systems use this.
- **Optimistic (version):** book without lock, at commit check version. Better if contention low.
- **Status field + atomic update:** `UPDATE seats SET status='held' WHERE id=X AND status='available'`
  (atomic compare-and-set).

**Architecture**
```
Client -> LB -> Booking Service -> DB (seat status, atomic updates)
                                -> Redis (temporary locks + TTL)
                                -> Payment Service -> confirm/release
Timeout job: expired holds -> release seats
```

**Deep dive:**
- **High contention:** popular event, 100K users for 10K seats — most fail fast (seat taken).
- **Per-event locking** (fine-grained) — different events parallel.
- **Hold timeout:** locked seat auto-release if payment not done (background job / TTL).
- Repo `IRCTC_LLD`: segment-based seats + per-run mutex — exactly this LLD pattern.

**Trade-offs:** hold duration (long = seats blocked, short = user rushed), pessimistic
(safe, contention) vs optimistic (fast, retry).

---

### 26.15 — Design Yelp / Nearby Places (geospatial)

**R — Requirements:** find businesses/places near a location, search by category, reviews, ratings.

**⭐ Core: geospatial indexing (like Uber)**
- **Geohash:** lat/lng → string. Query = same prefix + adjacent cells.
- **QuadTree:** recursive grid (dense areas more cells). Nearby = traverse to cell + neighbors.
- **Redis GEO:** `GEOADD places`, `GEORADIUS` — practical.

**Architecture**
```
Client -> API -> Search Service -> Geo index (QuadTree/Redis GEO) -> nearby placeIds
       -> Business DB (details, reviews) -> filter by category/rating
       -> Reviews (separate, write-heavy) -> aggregate rating (cached)
```

**Deep dive:**
- **Static data:** businesses rarely change → aggressive caching, pre-computed geo index.
- **Reviews:** write-heavy, separate store. Rating aggregate cached (not real-time recompute).
- **Search:** geo filter + category filter + rank (distance/rating/relevance).

**Trade-offs:** geohash (simple, uneven) vs QuadTree (balanced, complex), rating freshness
(cached, eventual) vs real-time.

---

<a name="h27"></a>
## 27. Database internals + storage engines (senior深)

### 27.1 — B-Tree vs LSM-Tree ⭐
| | B-Tree | LSM-Tree |
|---|---|---|
| Used by | MySQL, PostgreSQL (RDBMS) | Cassandra, RocksDB, LevelDB (NoSQL) |
| Writes | in-place update (random I/O) | append-only (sequential — fast) |
| Reads | fast (direct lookup) | slower (multiple SSTables + bloom filter) |
| Best for | read-heavy, range queries | write-heavy |
| Structure | balanced tree, pages | memtable + SSTables + compaction |

**LSM flow:** writes → memtable (RAM) → full → flush to SSTable (disk, immutable) →
background compaction (merge SSTables). Reads check memtable + SSTables (Bloom filter se skip).
> **Interview insight:** write-heavy system? LSM (Cassandra). Read-heavy + transactions? B-tree (SQL).

### 27.2 — MVCC (Multi-Version Concurrency Control)
Readers writers ko block na karein — har row ki multiple versions. Reader ek snapshot dekhta
(consistent), writer nayi version banata. PostgreSQL, MySQL InnoDB use karte.
> Isliye SQL me reads writes ko block nahi karte (snapshot isolation).

### 27.3 — Isolation levels (SQL)
| Level | Prevents | Allows |
|---|---|---|
| Read Uncommitted | — | dirty reads |
| Read Committed | dirty reads | non-repeatable reads |
| Repeatable Read | non-repeatable | phantom reads |
| Serializable | sab | (slowest, full isolation) |
> Higher isolation = more consistency, less concurrency. Trade-off.

### 27.4 — Indexing internals
- **Clustered index:** data physically sorted by key (primary key). Ek hi per table.
- **Non-clustered (secondary):** separate structure → points to row. Multiple allowed.
- **Covering index:** query ke saare columns index me → table access hi nahi (fast).

### 27.5 — Write-ahead log (WAL) revisited
Change pehle WAL (append, durable) → phir apply to data pages. Crash → WAL replay = recovery.
Base for durability + replication (ship WAL to replicas).

### 27.6 — Denormalization patterns
- **Materialized views:** pre-computed query result (refresh periodically). Read-heavy analytics.
- **Precomputed aggregates:** counts/sums stored (not computed each read). Eventual update.
- **Embedded documents (NoSQL):** related data ek document me (no join).

---

<a name="h28"></a>
## 28. Scenario-based / behavioral system design Q&A

**Q1. "System slow ho gaya, kaise debug karoge?"**
Metrics dekho (latency p99, error rate, CPU/memory). Tracing se bottleneck component. DB slow
query log. Cache hit ratio. Recent deploy? Traffic spike? Layer by layer isolate.

**Q2. "DB pe load bahut hai, kya karoge?"**
Cache add (read load kam), read replicas (read scaling), query optimize + index, connection
pooling, sharding (write scaling), archive old data.

**Q3. "Ek service down ho gayi, cascade kaise roke?"**
Circuit breaker (fail fast), timeout, retry with backoff, bulkhead (isolate), graceful
degradation (feature off, system up).

**Q4. "Traffic 10x ho gaya achanak, kaise handle?"**
Auto-scaling (horizontal), queue (buffer spike), CDN (offload static), rate limiting (protect),
cache (DB protect), load shedding (non-critical drop).

**Q5. "Data consistency issue — users purana data dekh rahe."**
Cache invalidation dekho (stale cache?), replication lag (read from replica?), eventual
consistency window. Fix: cache TTL, read-your-writes (master read), or accept + inform.

**Q6. "Zero-downtime deployment kaise?"**
Blue-green (2 env, switch), canary (gradual), rolling update, feature flags, backward-compatible
DB migrations (add column, dual-write, backfill, switch).

**Q7. "Storage cost bahut zyada, kaise kam?"**
Data tiering (hot SSD, cold cheap storage/Glacier), compression, dedup, TTL/archival, right-size,
delete unused.

**Q8. "Global users, latency high — kya karoge?"**
CDN (static edge), multi-region deployment, geo-DNS routing, regional DB replicas, edge compute.

**Q9. "Analytics real-time chahiye, kaise?"**
Stream processing (Kafka + Flink/Spark Streaming), pre-aggregation, approximate algorithms
(HyperLogLog for unique counts), time-windowed.

**Q10. "Idempotency kaise ensure karoge payment me?"**
Client idempotency key → server dedup store (Redis) → same key = cached result (no re-charge).

**Q11. "Hot partition/celebrity problem?"**
Cache hot data (local + distributed), replicate hot key across nodes, hybrid fan-out
(celebrity pull), consistent hashing rebalance, dedicated resources.

**Q12. "How to handle a thundering herd on cache miss?"**
Lock (one request rebuilds, others wait), stale-while-revalidate (serve stale + async refresh),
probabilistic early expiration (jitter TTL), request coalescing.

**Q13. "Message ordering kaise guarantee?"**
Per-partition ordering (Kafka), sequence numbers, single consumer per key, ya accept unordered
+ client sort by timestamp.

**Q14. "Duplicate processing kaise avoid (at-least-once queue)?"**
Idempotent consumer (dedup by message ID), processed-messages store, idempotent operations.

**Q15. "How to migrate DB with zero downtime?"**
Dual writes (old + new), backfill historical, verify consistency, gradually switch reads,
monitor, cleanup old. Feature flags for rollback.

---

<a name="h29"></a>
## 29. Two more deep-dive examples

### 29.1 — Design Ad Click Aggregator (real-time analytics)

**R — Requirements:** billions of ad clicks/day, real-time aggregation (clicks per ad per
minute), query aggregated data, accuracy + low latency.

**⭐ Core: stream aggregation**
```
Click events -> Kafka (ingest, high throughput) -> Stream processor (Flink/Spark Streaming)
   -> windowed aggregation (per ad, per minute) -> OLAP DB (fast reads)
   -> raw events -> data lake (S3, batch reprocess / accuracy)
```

**Architecture (Lambda-ish):**
```
Real-time path: Kafka -> Flink (approximate, fast) -> serving DB
Batch path: S3 raw -> Spark (accurate, periodic) -> reconcile
```

**Deep dive:**
- **Ingest:** Kafka absorbs spike (partition by adId).
- **Aggregation:** tumbling windows (per minute), stateful stream processing.
- **Dedup:** click fraud / duplicates — dedup by clickId (window).
- **Storage:** aggregated in OLAP (fast query), raw in data lake (reprocess if logic changes).
- **Accuracy vs latency:** real-time (approximate) + batch (accurate) reconcile.

**Trade-offs:** exactly-once (complex) vs at-least-once + dedup, real-time approximation vs
batch accuracy, hot ads (partition skew).

---

### 29.2 — Design Zoom / Video Calling

**R — Requirements:** 1-1 + group video calls, low latency (<150ms), screen share, recording,
scale (100s in a call).

**⭐ Core: media transport architecture**
| Architecture | Kaise | Kab |
|---|---|---|
| **P2P (mesh)** | har participant har doosre ko direct | 1-1, chhote (2-4) |
| **SFU (Selective Forwarding Unit)** | server forwards streams (no mixing) | medium groups (efficient) |
| **MCU (Multipoint Control Unit)** | server mixes streams into one | large, low client bandwidth |

**Architecture**
```
Clients <-WebRTC-> SFU (media server, forwards streams)
Signaling server (WebSocket): connection setup, ICE candidates, SDP exchange
STUN/TURN servers: NAT traversal (firewall/NAT ke peeche clients connect)
Recording: server-side stream capture -> S3
```

**Deep dive:**
- **WebRTC:** browser real-time media (UDP-based, low latency).
- **Signaling:** WebSocket for call setup (who's in call, codecs, network info).
- **NAT traversal:** STUN (discover public IP), TURN (relay if direct fails).
- **SFU:** scalable for groups (forward, don't mix — CPU efficient). Simulcast (multiple
  qualities, receiver picks).
- **UDP over TCP:** video latency > reliability (dropped frame ok, delay not).

**Trade-offs:** P2P (no server cost, doesn't scale) vs SFU (scalable, server cost) vs MCU
(low client bandwidth, high server CPU), quality vs bandwidth (adaptive/simulcast).

---

<a name="h30"></a>
## 30. Technology decision matrices (kaunsa kab)

### 30.1 — Kaunsa database?
| Requirement | Choose |
|---|---|
| Complex queries, joins, transactions | **PostgreSQL / MySQL** (RDBMS) |
| Flexible schema, documents | **MongoDB** |
| Write-heavy, time-series, huge scale | **Cassandra** |
| Key-value, ultra-fast, cache | **Redis / DynamoDB** |
| Full-text search | **Elasticsearch** |
| Graph/relationships (social) | **Neo4j** |
| Analytics / warehouse | **Redshift / BigQuery / Snowflake** |
| Geospatial | **PostGIS / Redis GEO** |
| Wide-column, Google-scale | **Bigtable / HBase** |

### 30.2 — Kaunsa cache?
| Requirement | Choose |
|---|---|
| Rich types, persistence, pub-sub | **Redis** |
| Simple pure cache, multi-threaded | **Memcached** |
| Static content, global | **CDN (Cloudflare/CloudFront)** |
| Application-local | **in-memory (Caffeine/Guava)** |

### 30.3 — Kaunsa message system?
| Requirement | Choose |
|---|---|
| Event streaming, replay, high throughput | **Kafka** |
| Task queues, routing, RPC | **RabbitMQ** |
| Managed, simple queue | **AWS SQS** |
| Pub-sub fan-out (managed) | **AWS SNS / Google Pub-Sub** |

### 30.4 — Kaunsa communication protocol?
| Requirement | Choose |
|---|---|
| Public API, simple | **REST** |
| Internal microservices, fast | **gRPC** |
| Flexible client data needs | **GraphQL** |
| Real-time bidirectional | **WebSocket** |
| Server → client one-way push | **SSE** |

### 30.5 — Consistency choice
| Use case | Choose |
|---|---|
| Banking, payments, inventory, booking | **Strong (CP)** |
| Social feed, likes, view counts, analytics | **Eventual (AP)** |
| User's own writes must show immediately | **Read-your-writes** |

---

<a name="h31"></a>
## 31. Kafka + Redis deep (commonly asked technologies)

### 31.1 — Kafka essentials
- **Topic:** category of messages. **Partition:** topic ka shard (parallelism + ordering unit).
- **Producer:** writes to topic (partition by key). **Consumer:** reads. **Consumer group:**
  consumers share partitions (each partition → one consumer in group).
- **Offset:** consumer's position (can replay from any offset — messages retained on disk).
- **Ordering:** guaranteed **per-partition** only (not across topic).
- **Replication:** partitions replicated (leader + followers) — fault tolerant.
- **Why fast:** sequential disk writes (append-only log), zero-copy, batching.
> **Interview:** "ordering chahiye + parallelism" → partition by key (same key → same partition
> → ordered). Throughput → more partitions (but ordering only per-partition).

### 31.2 — Redis essentials
- **Data types:** String, List, Set, Sorted Set (ZSET), Hash, Bitmap, HyperLogLog, Streams.
- **Use cases:** cache, session store, leaderboard (ZSET), rate limiter (INCR+TTL), pub-sub,
  distributed lock (SETNX), queue (List).
- **Persistence:** RDB (snapshots) + AOF (append-only log). Or pure cache (no persist).
- **Single-threaded:** commands atomic (no race within a command). Very fast (in-memory).
- **Redis Cluster:** sharding (16384 hash slots) + replication. Scale + HA.
- **Eviction:** maxmemory + policy (LRU/LFU/TTL).
> **Interview:** "atomic counter" → INCR. "top-N" → ZSET. "unique count at scale" → HyperLogLog
> (approximate, tiny memory). "rate limit" → INCR + EXPIRE (Lua for atomicity).

### 31.3 — Elasticsearch essentials
- **Inverted index:** word → documents (full-text search fast).
- **Use:** search, log analytics (ELK), autocomplete.
- **Not a primary DB:** eventual consistency, denormalized. Source of truth alag rakho.

---

## 32. 30 must-know one-liners (last-minute revision)

1. **Requirements pehle** — functional + non-functional + scope
2. **Estimate** — QPS = DAU × actions / 86400; peak = 2-3× avg
3. **Read-heavy** → cache + read replicas + CDN
4. **Write-heavy** → sharding + Cassandra + queue
5. **Stateless services** → easy horizontal scaling
6. **No SPOF** → redundancy every layer
7. **Cache-aside** → most common caching pattern
8. **Cache invalidation** → hardest problem (TTL / write-through / event)
9. **Consistent hashing** → smooth resharding (1/N move)
10. **CAP** → P zaroori, choose C or A
11. **CP** → banking; **AP** → social feed
12. **ACID** → SQL; **BASE** → NoSQL
13. **Sharding** → write scaling; **Replication** → read scaling + HA
14. **Index** → fast reads, slow writes
15. **Message queue** → decouple + async + buffer
16. **At-least-once + idempotent** → practical exactly-once
17. **Fan-out on write** → fast read (celebrity issue) → hybrid
18. **WebSocket** → real-time bidirectional
19. **CDN** → static content, global latency
20. **Circuit breaker** → cascading failure roke
21. **Saga** → distributed transaction (compensations)
22. **Idempotency key** → retry-safe payments
23. **Rate limiter** → token bucket / sliding window
24. **Geospatial** → Geohash / QuadTree / Redis GEO
25. **Snowflake** → distributed sortable unique ID
26. **Bloom filter** → fast "not exists" (cache penetration)
27. **p99 latency** > average (tail matters)
28. **LSM-tree** → write-heavy; **B-tree** → read-heavy
29. **Start simple, scale as bottlenecks appear**
30. **Trade-offs > perfect answer** — justify every decision

---

## 33. Distributed system design patterns (architectural)

Ye microservices/distributed systems ke architectural patterns hain (LLD ke GoF patterns se alag):

### 33.1 — Sidecar pattern
Main service ke saath ek helper container (logging, monitoring, proxy). Service Mesh (Istio)
sidecar proxy use karta — networking/observability app se alag.

### 33.2 — Ambassador pattern
Client aur remote service ke beech ek proxy (retries, circuit breaking, monitoring). Client
simple, cross-cutting ambassador me.

### 33.3 — API Gateway (revisited)
Single entry — routing, auth, rate limit, aggregation. Client ↔ microservices ke beech.

### 33.4 — Backend for Frontend (BFF)
Har client type (mobile/web) ka apna backend layer — tailored responses (mobile kam data,
web zyada). One-size-fits-all API ke problems solve karta.

### 33.5 — Strangler Fig pattern
Monolith → microservices migration: naye features microservices me, purane gradually migrate,
monolith "strangle" hota jaata (ek saath rewrite nahi). Low risk.

### 33.6 — CQRS (Command Query Responsibility Segregation)
Read model aur write model alag. Writes → normalized (consistency). Reads → denormalized views
(fast). Event se sync. Read-heavy + complex queries me.

### 33.7 — Event Sourcing
State = events ki sequence (current snapshot ke bajaye). Full audit, replay, time-travel.
CQRS ke saath aksar. Ledger, banking, audit-heavy systems.

### 33.8 — Saga (revisited)
Distributed transaction across services — local transactions + compensating actions.
Choreography (events) ya Orchestration (central).

### 33.9 — Outbox pattern
DB write + message publish atomic nahi (2 systems). Solution: message ko same DB transaction me
"outbox" table me likho → separate process outbox se publish. Guaranteed delivery + consistency.

### 33.10 — Leader-Follower (Primary-Replica)
Ek leader (writes) + followers (reads/standby). Leader mara → follower promote. HA + read scaling.

### 33.11 — Scatter-Gather
Request ko multiple services/shards pe bhejo (scatter), results merge (gather). Search across
shards, aggregation.

### 33.12 — Cache-aside + Write-through (revisited)
Caching patterns — section 6 me detail.

### 📋 Pattern → problem
| Pattern | Solves |
|---|---|
| Sidecar / Ambassador | cross-cutting (logging, retry) app se alag |
| BFF | per-client tailored API |
| Strangler Fig | safe monolith → microservices migration |
| CQRS | read/write different needs |
| Event Sourcing | audit, replay, time-travel |
| Saga | distributed transaction |
| Outbox | DB write + event publish consistency |
| Scatter-Gather | query across shards |
| Circuit Breaker | cascading failure |
| Leader-Follower | HA + read scaling |

---

## 🎓 Final words

Ye do files (`LLD_Interview.md` + `HLD_Interview.md`) tumhare interview prep ke liye complete
reference hain:
- **LLD** — ek component ke andar ka design (OOP, patterns, SOLID, C++, concurrency, classic
  problems with code)
- **HLD** — poore system ka architecture (scalability, DB, cache, distributed systems)

**Aur is repo ke folders ka asli code padho** — har LLD folder me detailed Hinglish comments +
`design_patterns_used.md` + `design_diagram.md` (UML/mermaid). Theory yahan, practice wahan.

**Interview me yaad rakho:**
1. Requirements clarify karo (dono LLD + HLD me)
2. Structured raho (LLD: entities→patterns→code; HLD: RESHADED)
3. Trade-offs khud bolo (no perfect answer)
4. Think out loud + collaborate
5. Start simple, scale/extend as needed

All the best bhai! 🚀 Ja ke crack kar de. 💪

---

## 34. Estimation practice (worked problems)

Estimation interview me karna padta — ye examples practice karo:

### 34.1 — Design Twitter — estimation
```
Given: 300M MAU, 50% DAU = 150M DAU
Tweets: avg user 2 tweets/day -> 300M tweets/day
   Write QPS = 300M / 86400 ≈ 3,500 tweets/sec (peak ~10K/sec)
Reads: avg user 100 timeline views/day -> 15B reads/day
   Read QPS = 15B / 86400 ≈ 173K/sec (peak ~500K/sec)
   Read:Write ≈ 50:1 -> READ-HEAVY -> cache + pre-computed timelines

Storage (tweets):
   300M tweets/day × 300 bytes = 90 GB/day
   × 365 × 5 years = ~164 TB (text only; media separate on S3)

Cache (timelines):
   150M DAU × 800 tweets × 8 bytes (tweetId) = ~1 TB (timeline cache in Redis)

Bandwidth:
   Read: 173K/sec × 300 bytes ≈ 52 MB/sec incoming timeline data
```

### 34.2 — Design YouTube — estimation
```
Given: 2B users, 500 hours uploaded/min
Storage (uploads):
   500 hrs/min × 60 = 30,000 hrs/day
   1 hr video ≈ 1 GB (compressed, 1 resolution)
   × 5 resolutions ≈ 5 GB/hr
   30,000 × 5 = 150 TB/day of processed video (!!)
   × 365 = ~55 PB/year -> storage tiering + CDN critical

Bandwidth (watch):
   Reads >> writes. Say 5B video views/day, avg 5 min, 720p (~1.5 GB/hr = 125 MB per 5 min)
   5B × 125 MB = massive -> CDN handles 95%+ (origin protected)
```

### 34.3 — Design Chat (WhatsApp) — estimation
```
Given: 500M DAU, 40 messages/user/day
Messages: 500M × 40 = 20B messages/day
   Write QPS = 20B / 86400 ≈ 230K/sec (peak ~700K/sec)
Storage: 20B × 100 bytes = 2 TB/day (if retained)
Connections: 500M concurrent WebSocket
   Per server ~65K connections -> ~8,000 connection servers
```

### 34.4 — Estimation tips (interview me)
- **Round aggressively:** 86,400 ≈ 100K (10^5), 365 ≈ 400. Speed > precision.
- **Read:write ratio pehle poocho** — decides caching importance.
- **State assumptions** — "maano avg tweet 300 bytes" (interviewer correct karega).
- **Order of magnitude matters** — GB vs TB vs PB (not exact number).
- **Storage = count × size × retention × replication.**
- **Peak = 2-3× average** (traffic uneven).

---

## 35. Common HLD interview mistakes (avoid karo)

| Mistake | Kyun galat | Fix |
|---|---|---|
| Requirements skip | galat system banaoge | 5 min clarify |
| No estimation | scale decisions ungrounded | QPS/storage estimate |
| Ek component obsess | breadth miss | high-level pehle, phir deep dive |
| Buzzword bombing | samajh nahi dikhti | har tech ka "kyun" bolo |
| Over-engineering | 100 users ke liye 50 services | start simple |
| No trade-offs | naive lagta | har decision ka cost bolo |
| SPOF ignore | system fragile | redundancy har layer |
| Ignoring interviewer hints | wo guide kar rahe | listen + adapt |
| Silent thinking | wo tumhari soch nahi dekh paate | think out loud |
| No bottleneck analysis | scaling adhoora | "yahan bottleneck, ye fix" |

---

## 36. HLD interview checklist (har design me ye cover karo)

Interview ke dauran mentally ye checklist run karo — kuch chhoot na jaye:

```
□ REQUIREMENTS
  □ Functional (core features listed + confirmed)
  □ Non-functional (scale, latency, availability, consistency)
  □ Scope defined (kya IN, kya OUT)
  □ Read vs write ratio poocha

□ ESTIMATION
  □ QPS (average + peak)
  □ Storage (with retention + replication)
  □ Bandwidth
  □ Cache size (hot data)

□ API DESIGN
  □ Key endpoints (REST/gRPC)
  □ Request/response shape
  □ Idempotency (writes)

□ DATA MODEL
  □ SQL vs NoSQL (justified)
  □ Schema / entities
  □ Sharding key (if sharded)
  □ Indexes

□ HIGH-LEVEL ARCHITECTURE
  □ Client → LB → services → cache → DB drawn
  □ Data flow shown
  □ Async paths (queue) identified

□ SCALABILITY
  □ Caching (where, strategy)
  □ Load balancing
  □ DB scaling (replicas + sharding)
  □ CDN (static content)
  □ Stateless services

□ RELIABILITY
  □ No SPOF (redundancy)
  □ Replication + failover
  □ Circuit breaker / retry / timeout
  □ Graceful degradation

□ DEEP DIVE
  □ 1-2 components in detail
  □ Core algorithm (fan-out / geo / matching / etc.)

□ TRADE-OFFS
  □ Consistency vs availability (CAP)
  □ Latency vs consistency
  □ Cost vs performance
  □ Every major decision justified

□ BOTTLENECKS
  □ Hot keys / hot partitions
  □ Scaling limits identified
  □ Monitoring/observability mentioned
```

### One-page mental model (har system)
```
        [ Client ]
            │
        [  CDN  ]  ← static (images, video, JS)
            │
    [ Load Balancer ]  ← distribute, health check, no SPOF
            │
   [ Stateless App Servers ]  ← horizontal scale
       /    │    \
  [Cache] [Queue] [Search]   ← Redis / Kafka / Elasticsearch
     │       │
  [ Database ]  ← replicas (read) + shards (write)
     │
  [ Object Storage (S3) ] + [ Data Lake / Warehouse ]
```

> **Ye diagram 80% systems ka base hai.** Requirements ke hisaab se components add/remove karo,
> aur har box pe poocho: "ye fail hua to? scale kaise? consistency kya?"

---

# ⚡ PART II — ADVANCED / DEEP TOPICS

> Ye section senior/SDE-2+ level depth ke liye. Sharding, replication, consensus, distributed
> transactions, probabilistic data structures, geo, streaming — sab detail me.

---

<a name="a1"></a>
## A1. Sharding — deep dive ⭐⭐

Basics section 7 me the. Ab **poora depth**.

### A1.1 — Sharding kyun (aur kab NAHI)
- **Kyun:** ek DB ki write throughput / storage / connections limit cross ho jaaye.
- **Kab NAHI (pehle ye try karo):** read replicas (read scaling), caching, vertical scaling,
  query optimization, archiving. **Sharding last resort** — bahut complexity laata (cross-shard
  queries, transactions, rebalancing, operational overhead).
> Interview line: "Sharding se pehle main caching + read replicas + vertical scaling try karunga.
> Jab writes ek node handle na kar paaye, tab sharding."

### A1.2 — Shard key selection (SABSE critical decision)
Shard key galat = poora system suffer. Achha shard key:
1. **High cardinality** — bahut unique values (userId ✅, gender ❌ sirf 2-3 values)
2. **Even distribution** — koi ek shard overloaded na ho (uniform)
3. **Query isolation** — common queries ek shard se satisfy ho (cross-shard avoid)
4. **Monotonic NA ho** — auto-increment/timestamp key = saara naya data ek shard pe (hotspot)

**Examples:**
```
Twitter tweets: shard by tweetId (even) ya userId (user ki tweets ek jagah — timeline aasan)
E-commerce orders: shard by customerId (customer ki orders ek shard — order history fast)
Chat messages: shard by conversationId (conversation ek shard — history query fast)
```
⚠ **Anti-pattern:** shard by timestamp → aaj ka saara traffic ek shard (hot), purane shards idle.

### A1.3 — Sharding strategies (deep)
| Strategy | Kaise | Pros | Cons |
|---|---|---|---|
| **Range-based** | key ranges (A-M, N-Z) | range queries efficient, simple | hotspots (uneven), manual rebalance |
| **Hash-based** | `hash(key) % N` | even distribution | range query = all shards, resharding painful (% N changes) |
| **Consistent hashing** | hash ring | resharding = 1/N move | complex, still range-query issue |
| **Directory-based** | lookup table (key→shard) | flexible, easy rebalance | lookup overhead, directory = SPOF/bottleneck |
| **Geo-based** | location se | latency (local data) | uneven (populous regions), compliance-friendly |
| **Composite** | multiple fields (tenantId + hash) | multi-tenant isolation | complex |

### A1.4 — Resharding / rebalancing ⭐
Data grow → shards add karna. Sabse mushkil operational task.
- **Problem with `hash % N`:** N badalne se **saari keys remap** → massive data movement + downtime.
- **Consistent hashing:** sirf 1/N keys move (adjacent shard). Better.
- **Pre-splitting / virtual shards:** logical shards (jaise 1024) physical nodes pe map. Node add
  karo → kuch virtual shards move (poora rehash nahi). MongoDB/many systems ye karte.
```
1024 virtual shards -> 4 physical nodes (256 each)
Add 5th node -> move ~205 virtual shards (rebalance), no full rehash
```
- **Live migration:** dual-write (old + new shard), backfill, verify, switch reads, cleanup.
  Zero downtime.

### A1.5 — Cross-shard problems + solutions
- **Cross-shard queries:** ek query multiple shards se. Solution: **scatter-gather** (query all
  shards parallel, merge results). Slow — avoid via good shard key.
- **Cross-shard joins:** avoid. Denormalize (embed related data), ya application-level join,
  ya keep related data on same shard (co-location).
- **Cross-shard transactions:** distributed transaction (2PC / Saga — section A3). Expensive.
  Avoid — design so transactions stay within a shard.
- **Global secondary index:** ek field pe query jo shard key nahi. Solution: separate index
  table (sharded by that field), ya search engine (Elasticsearch).
- **Aggregations (COUNT/SUM across shards):** scatter-gather + merge, ya pre-computed
  aggregates, ya OLAP/analytics DB.

### A1.6 — Hot shard / celebrity problem
Ek shard pe disproportionate load (celebrity user, viral content).
- **Detect:** monitor per-shard QPS/latency.
- **Mitigate:** cache hot data (celebrity separate), split hot shard, add randomness to key
  (`userId#bucket`), read replicas for hot shard, dedicated resources.

### A1.7 — Sharding + replication together
Real systems dono use karte:
```
Shard 1: Primary + Replica1 + Replica2   (writes to primary, reads from replicas)
Shard 2: Primary + Replica1 + Replica2
...
Har shard independently replicated (HA + read scaling within shard)
```

---

<a name="a2"></a>
## A2. Replication — deep dive ⭐⭐

### A2.1 — Replication topologies
| Topology | Kaise | Pros | Cons |
|---|---|---|---|
| **Single-leader (master-slave)** | ek leader writes, replicas reads | simple, no write conflict | leader = write bottleneck + SPOF |
| **Multi-leader (master-master)** | multiple leaders writes | write availability, geo (each region leader) | **write conflicts** (resolution needed) |
| **Leaderless (Dynamo-style)** | koi bhi node write, quorum | high availability, no SPOF | complex (read-repair, conflicts) |

### A2.2 — Sync vs Async vs Semi-sync replication
- **Synchronous:** leader waits for replica ack before confirming write. **Consistent** (no data
  loss on failover), par **slow** + replica down → writes block.
- **Asynchronous:** leader confirms immediately, replicates in background. **Fast**, par
  **replication lag** (stale reads) + failover pe data loss (unreplicated writes gone).
- **Semi-synchronous:** at least ONE replica sync, baaki async. Balance (no total loss, decent speed).
> Most systems: async (speed) with monitoring, ya semi-sync for critical data.

### A2.3 — Replication lag problems + solutions ⭐
Async replication me replica leader se peeche → stale reads. Fixes:
- **Read-your-own-writes:** user apni likhi cheez turant dekhe → uske reads leader se (ya
  recently-written data leader se).
- **Monotonic reads:** ek user hamesha same replica se padhe (ya newer) — time backward na jaaye.
- **Consistent prefix reads:** causally related writes order me dikhein.
- **Bounded staleness:** replica X seconds se zyada peeche na ho (warna traffic hatao).

### A2.4 — Multi-leader conflict resolution ⭐
Do leaders same row alag update karein → conflict. Resolution:
- **Last-Write-Wins (LWW):** timestamp se latest jeetta. Simple par **data loss** (ek update gaya).
- **Version vectors / vector clocks:** concurrent vs causal detect. Concurrent → app resolve ya
  keep both.
- **CRDTs (Conflict-free Replicated Data Types):** data structures jo automatically merge
  (counters, sets) bina conflict. Collaborative editing, distributed counters.
- **Application-level:** business logic decide kare (merge shopping carts — union).

### A2.5 — Leaderless replication (Dynamo/Cassandra)
Koi bhi node write/read accept karta. Consistency **quorum** se tune:
```
N = replicas, W = write acks needed, R = read acks needed
W + R > N  -> strong consistency (overlap guaranteed)
N=3, W=2, R=2 -> strong. W=1,R=1 -> fast but eventual.
```
- **Read-repair:** read pe stale replica detect → update (during read).
- **Anti-entropy:** background process replicas sync karta (Merkle trees se diff).
- **Hinted handoff:** node down → dusra node uska write temporarily rakhta ("hint"), node wapas
  → handoff. Availability badhata.

### A2.6 — Failover (leader mara to?)
```
1. Detect failure (heartbeat timeout)
2. Choose new leader (most up-to-date replica — consensus/election)
3. Reconfigure (clients + replicas new leader pe point)
Problems: split-brain (2 leaders!), lost writes (async unreplicated), 
          timeout tuning (too short = false positive)
```
> **Split-brain fix:** fencing (old leader ko reject), quorum (majority side hi leader).

---

<a name="a3"></a>
## A3. Distributed transactions — deep dive ⭐

Multiple nodes/services pe atomic operation. "All-or-nothing" across systems.

### A3.1 — Two-Phase Commit (2PC)
Coordinator + participants. Do phases:
```
Phase 1 (PREPARE): coordinator -> "ready?" -> participants vote (yes/no), lock resources
Phase 2 (COMMIT/ABORT): sab yes -> "commit"; koi no -> "abort" -> participants act
```
- **Pros:** strong consistency (atomic).
- **Cons:** **blocking** (coordinator down → participants locked, wait), coordinator = SPOF,
  slow (2 round trips + locks held). Latency + availability hit.

### A3.2 — Three-Phase Commit (3PC)
2PC + extra "pre-commit" phase → non-blocking (timeout se decide). Par network partition me
still issues. Practically rare (complexity).

### A3.3 — Saga pattern ⭐ (microservices ka favourite)
Long-running transaction ko sequence of **local transactions** me todo, har ek ka
**compensating action** (undo).
```
Order Saga: createOrder -> reserveInventory -> chargePayment -> arrangeShipping
Agar chargePayment FAIL:
  compensate (reverse order): releaseInventory + cancelOrder
```
**Do types:**
- **Choreography:** har service event emit/consume, no central coordinator. Loose coupling par
  flow trace karna mushkil (distributed logic).
```
OrderService -> "OrderCreated" event -> InventoryService -> "InventoryReserved" -> PaymentService...
Fail pe -> compensating events reverse direction
```
- **Orchestration:** central orchestrator steps manage karta. Clear flow, par orchestrator
  = single point of logic.
```
Orchestrator: call inventory -> call payment -> call shipping (sequence control)
Fail pe -> orchestrator compensations trigger karta
```
> **2PC vs Saga:** 2PC = strong consistency, blocking (avoid at scale). Saga = eventual
> consistency, non-blocking, resilient (microservices standard).
> Repo `Ecommerce_Cart_Checkout_LLD` me reservation saga (reserve → commit/release) LLD level pe.

### A3.4 — TCC (Try-Confirm-Cancel)
Saga ka structured version:
```
Try:     resources reserve (tentative — hold)
Confirm: reservation commit (permanent)
Cancel:  reservation release (rollback)
```
Har service teeno operations implement karti. Booking systems (seat hold → confirm/release).

### A3.5 — Outbox pattern (dual-write problem solve)
DB write + event publish **atomic nahi** (2 systems — ek succeed ek fail ho sakta).
```
Solution: event ko SAME DB transaction me "outbox" table me likho
          -> separate poller/CDC outbox se read karke message broker pe publish
          -> guaranteed: DB change hui to event bhi jaayega (eventual)
```
> Ye "at-least-once event delivery + DB consistency" ensure karta. Kafka Connect + Debezium (CDC).

### A3.6 — Idempotency in distributed transactions
Retry safe banane ke liye har step idempotent ho — same operation kai baar = ek hi effect.
Idempotency key + dedup store.

---

<a name="a4"></a>
## A4. Consensus + Consistency — deep dive ⭐⭐

### A4.1 — Consensus problem
Multiple nodes ek value pe agree karein (leader election, commit order, config) — network
unreliable, nodes fail. **FLP impossibility:** async network me guaranteed consensus impossible
(par practically timeouts se solve).

### A4.2 — Raft (deep — most asked)
Leader-based, understandable consensus. 3 sub-problems:
```
1. LEADER ELECTION:
   - nodes: Follower / Candidate / Leader
   - leader heartbeat bhejta. Timeout (no heartbeat) -> follower Candidate banta
   - Candidate votes maangta. Majority vote -> Leader
   - randomized election timeout (split vote avoid)

2. LOG REPLICATION:
   - client -> leader -> log entry append -> replicate to followers
   - majority ack -> entry "committed" -> apply to state machine -> respond client

3. SAFETY:
   - only up-to-date node leader ban sakta (log completeness)
   - committed entries kabhi lost nahi (majority persisted)
```
- **Used by:** etcd, Consul, TiKV, CockroachDB.
- **Quorum:** majority (N/2 + 1) chahiye — isliye odd number of nodes (3, 5) preferred.

### A4.3 — Paxos / Multi-Paxos / ZAB
- **Paxos:** classic consensus, correct par samajhna mushkil. Google Chubby.
- **Multi-Paxos:** Paxos for sequence of values (log). 
- **ZAB (ZooKeeper Atomic Broadcast):** ZooKeeper ka protocol (Raft jaisa, leader-based).

### A4.4 — Consistency models spectrum (deep)
Strong se weak tak:
```
Linearizability (strongest)  -> har operation ek global order me, real-time
  |  = "single copy jaisa lagta" (jaise ek hi machine)
Sequential consistency       -> ek global order, par real-time nahi
Causal consistency           -> causally-related ops order preserve (concurrent koi bhi order)
Eventual consistency (weakest) -> thodi der me sab consistent
```

### A4.5 — Linearizability vs Serializability ⭐ (confuse karte log)
- **Linearizability:** SINGLE object pe real-time ordering guarantee (recency — latest write dikhe).
  Distributed systems consistency.
- **Serializability:** MULTIPLE objects pe transactions ka isolation (jaise serially chale).
  Database ACID isolation.
- **Strict serializability** = serializability + linearizability (dono, strongest).
> Ek object ke liye linearizability, transactions ke liye serializability.

### A4.6 — Session guarantees (client-centric)
- **Read-your-writes:** apni likhi turant dikhe.
- **Monotonic reads:** ek baar naya dekha to purana nahi.
- **Monotonic writes:** ek client ki writes order me apply.
- **Writes-follow-reads:** jo padha uske baad ki write us data ke baad.

### A4.7 — CAP + PACELC (revisited, deep)
```
CAP: Partition hui to Consistency ya Availability
PACELC: Partition (P) -> A ya C
        Else (E, normal) -> Latency (L) ya Consistency (C)

Examples:
  MongoDB     -> CP / EC (consistency focus)
  Cassandra   -> AP / EL (availability + low latency)
  DynamoDB    -> AP / EL (tunable)
  Spanner     -> CP / EC (TrueTime se strong consistency globally)
```

---

<a name="a5"></a>
## A5. Probabilistic data structures ⭐

Bade scale pe exact data structures memory me nahi aate — approximate se kaam chalao (tiny memory).

### A5.1 — Bloom Filter
"Ye element set me hai?" — **"definitely NOT" ya "maybe YES"** (false positives ok, no false negatives).
```
Bit array + k hash functions.
Add(x):    k hashes -> set k bits to 1
Check(x):  k hashes -> agar koi bit 0 -> DEFINITELY NOT present
                    -> saare 1 -> MAYBE present (false positive possible)
```
- **Use:** cache penetration ("key exist karti hai kya" — DB hit se pehle), dedup (URL crawled?),
  DB (SSTable me key hai kya — LSM read optimize).
- **Trade-off:** more bits/hashes = less false positives, more memory. Can't delete (Counting
  Bloom filter delete allow karta).

### A5.2 — Count-Min Sketch
"Element ka approximate count" (frequency) — tiny memory, over-estimate possible.
- **Use:** heavy hitters (top-K frequent items), rate limiting, trending topics, traffic analysis.

### A5.3 — HyperLogLog
"Approximate UNIQUE count" (cardinality) — billions of unique items in KBs of memory (~2% error).
```
Redis: PFADD, PFCOUNT
```
- **Use:** unique visitors count, unique search queries — jaha exact count expensive/unnecessary.

### A5.4 — Merkle Tree
Hash tree — bade datasets efficiently compare/verify. Leaf = data hash, parent = children hash.
- **Use:** anti-entropy (Cassandra replicas diff), Git, blockchain, data verification.

### A5.5 — Skip List
Probabilistic balanced structure (linked list + express lanes). O(log n) search/insert.
- **Use:** Redis sorted sets (ZSET) internally, LevelDB memtable.

---

<a name="a6"></a>
## A6. Consistent Hashing — deep dive ⭐

Basics section 4.5 me. Ab deep.

### A6.1 — Problem recap
`hash(key) % N` — N badalne pe **saari keys remap** (massive movement). Distributed cache/DB me
node add/remove common → disaster.

### A6.2 — Consistent hashing mechanism
```
Servers + keys dono ek hash ring (0 to 2^32-1) pe map.
Key clockwise nearest server ko assign.
Server add -> sirf next server ki kuch keys move (1/N average).
Server remove -> uski keys next server ko.
```

### A6.3 — Virtual nodes (vnodes) — zaroori
Problem: kam servers → uneven distribution (ek server ko zyada keys). Solution: har physical
server ko ring pe **kai points** (virtual nodes) pe rakho.
```
Server A -> A#1, A#2, A#3, ... (100+ points on ring)
-> smoother distribution, aur server remove pe load evenly redistribute
```
> Cassandra, DynamoDB, Riak — sab vnodes use karte.

### A6.4 — Variants
- **Consistent hashing with bounded loads:** koi server capacity limit cross na kare (overflow
  next server ko). Google use karta.
- **Jump consistent hash:** no storage, fast, even — par sirf "add at end" (arbitrary remove nahi).
- **Rendezvous (HRW) hashing:** har key ke liye har server ka score (hash(key, server)), highest
  jeetta. Simple, good distribution, no ring.

### A6.5 — Kahan use
Distributed cache (Memcached client), DB sharding (Cassandra, DynamoDB), load balancer (session
affinity), CDN (content → edge), distributed hash tables (P2P).

---

<a name="a7"></a>
## A7. Geospatial indexing — deep dive ⭐

"Nearby" queries (Uber, Yelp, food delivery, nearby friends). Lat/long pe efficient search.

### A7.1 — Problem
Naive: har point se distance calculate (O(N)) — millions of points pe slow. Chahiye spatial index.

### A7.2 — Geohash
Lat/long → base32 string. **Same prefix = geographically nearby** (spatial locality).
```
"tdr1y" -> longer prefix = more precise (smaller area)
Nearby query: same prefix + 8 neighboring cells (edge cases pe adjacent cells check)
```
- **Pros:** simple, string prefix = range query (DB index friendly), sortable.
- **Cons:** boundary issues (do close points alag prefix ho sakte edge pe), uneven cell sizes.

### A7.3 — QuadTree
Recursive grid subdivision — dense areas (city) me zyada cells, sparse (ocean) me kam (adaptive).
```
Root = whole map. Cell me > threshold points -> 4 sub-cells me split (recurse).
Nearby query: traverse to point's leaf cell + neighbors.
```
- **Pros:** adaptive (dense areas fine-grained), balanced.
- **Cons:** in-memory (rebuild on updates), tree maintenance.

### A7.4 — Google S2 / Uber H3
- **S2 (Google):** sphere ko cells me (Hilbert curve — locality). Precise, handles earth curvature.
- **H3 (Uber):** hexagonal grid. Hexagons = uniform neighbor distance (better than squares for
  "nearby"). Uber production use karta.

### A7.5 — R-Tree
Rectangles ka tree (bounding boxes). Range/nearest queries. PostGIS use karta.

### A7.6 — Practical: Redis GEO
```
GEOADD drivers <lng> <lat> <driverId>
GEORADIUS drivers <lng> <lat> 2 km   -> nearby drivers
```
Internally geohash + sorted set. Production ke liye simple + fast.

---

<a name="a8"></a>
## A8. Search systems — deep dive

Full-text search (Elasticsearch, product search, autocomplete).

### A8.1 — Inverted index (core)
```
Documents -> tokenize -> normalize (lowercase, stem) -> index
Inverted index: word -> [doc IDs containing it]
   "phone" -> [doc1, doc5, doc9]
   "case"  -> [doc1, doc3]
Query "phone case" -> intersect lists -> [doc1]
```
Forward index = doc → words. **Inverted** = word → docs (search fast).

### A8.2 — Text processing pipeline
```
1. Tokenization: sentence -> words
2. Normalization: lowercase, remove punctuation
3. Stop words: remove "the", "a", "is" (noise)
4. Stemming/Lemmatization: "running" -> "run" (root)
5. Synonyms: "phone" ~ "mobile"
```

### A8.3 — Ranking (relevance)
- **TF-IDF:** Term Frequency (word doc me kitni baar) × Inverse Document Frequency (rare words
  zyada important). Common words down-weighted.
- **BM25:** TF-IDF ka improved version (Elasticsearch default). Saturation + length normalization.
- **Beyond text:** popularity, recency, personalization, click-through rate — ML ranking.

### A8.4 — Autocomplete (Trie — section 26.7)
Trie + pre-computed top-k per prefix. Debounce client requests.

### A8.5 — Architecture
```
Write: document -> index service -> tokenize -> inverted index (sharded) -> replicated
Read:  query -> parse -> shards (scatter) -> rank -> merge (gather) -> top-N
Elasticsearch: distributed inverted index, near-real-time, sharded + replicated.
```
> ⚠ Search engine = **NOT primary DB** (eventual consistency, denormalized). Source of truth alag.

---

<a name="a9"></a>
## A9. Stream processing — deep dive

Real-time event processing (analytics, monitoring, fraud detection).

### A9.1 — Batch vs Stream
- **Batch:** bounded data, periodic (Spark, Hadoop MapReduce). High latency (min-hours), efficient.
- **Stream:** unbounded data, continuous (Kafka Streams, Flink, Spark Streaming). Low latency (ms-sec).

### A9.2 — Windowing (stream me time grouping)
```
Tumbling: fixed non-overlapping (0-5min, 5-10min) — "per minute count"
Sliding:  overlapping (0-5, 1-6, 2-7) — "last 5 min every 1 min"
Session:  activity-based (gap se define) — "user session"
```

### A9.3 — Event time vs Processing time + Watermarks
- **Event time:** jab event actually hua (device timestamp).
- **Processing time:** jab system ne process kiya.
- **Problem:** events late/out-of-order aate (network delay).
- **Watermark:** "is time tak ke saare events aa gaye (probably)" — late events ka threshold.
  Window close karne ka signal.

### A9.4 — Exactly-once in streaming
- **Checkpointing:** state periodically save (Flink). Failure → last checkpoint se resume.
- **Idempotent writes / transactions:** duplicate processing → same result.
- Kafka + Flink exactly-once (transactional).

### A9.5 — Lambda vs Kappa architecture
- **Lambda:** batch (accurate, slow) + stream (fast, approximate) dono, merge. Complex (2 codebases).
- **Kappa:** sirf stream (reprocess by replaying log). Simpler, Kafka-centric.

---

<a name="a10"></a>
## A10. Change Data Capture (CDC)

DB me changes ko real-time capture karke downstream systems ko bhejo (cache, search, analytics, replicas).

### A10.1 — Problem
DB update hui → cache stale, search index purana, analytics peeche. Manually har jagah update
karna error-prone (dual-write problem).

### A10.2 — CDC approaches
- **Query-based (polling):** periodically "updated_at > last" query. Simple par delay + load +
  deletes miss.
- **Log-based (best):** DB ka transaction log (WAL/binlog) read karo → changes stream. Real-time,
  low overhead, deletes bhi capture. **Debezium** (Kafka Connect) popular.

### A10.3 — Use cases
- Cache invalidation (DB change → cache update)
- Search index sync (DB → Elasticsearch)
- Data warehouse ETL (OLTP → OLAP)
- Microservices data replication (outbox pattern)
- Audit log

```
MySQL/Postgres (binlog/WAL) -> Debezium -> Kafka -> consumers (cache, ES, analytics)
```

---

<a name="a11"></a>
## A11. Multi-region / Geo-distribution — deep

Global users → data unke paas (latency), disaster recovery, compliance (data residency).

### A11.1 — Deployment models
| Model | Kaise | Pros | Cons |
|---|---|---|---|
| **Single region** | ek datacenter | simple | far users slow, region down = total down |
| **Active-Passive** | primary region + standby | DR (failover) | standby idle (cost), failover delay |
| **Active-Active** | multiple regions serve traffic | low latency, HA | data sync/conflict complexity |

### A11.2 — Data challenges (active-active)
- **Replication across regions:** async (lag, but fast) — cross-continent sync too slow.
- **Conflict resolution:** same data 2 regions me update → LWW / CRDT / app logic (section A2.4).
- **Consistency:** usually eventual across regions (strong = high latency).

### A11.3 — Routing
- **GeoDNS:** user ko nearest region (DNS level).
- **Anycast:** same IP multiple locations, network nearest ko route.
- **Global load balancer:** health + latency based routing.

### A11.4 — Data residency / compliance
GDPR (EU), data localization (India) — user data specific region me rehna chahiye. Shard by
region (geo-sharding). Sensitive data cross-border na jaaye.

### A11.5 — Google Spanner (special)
Globally distributed + strong consistency (usually impossible at scale). **TrueTime** (GPS +
atomic clocks, bounded uncertainty) se globally consistent timestamps. CP system, low latency.

---

<a name="a12"></a>
## A12. Advanced caching

### A12.1 — Multi-level caching
```
L1: Browser cache (client)
L2: CDN (edge)
L3: Application local cache (in-process — Caffeine/Guava, fastest, per-server)
L4: Distributed cache (Redis — shared across servers)
L5: Database buffer pool (DB's own cache)
```
Har level miss → next. Local cache (L3) ultra-fast but per-server (consistency issue).

### A12.2 — Local vs distributed cache trade-off
- **Local (in-process):** fastest (no network), par per-server (stale across servers, memory duplicate).
- **Distributed (Redis):** shared (consistent), par network hop.
- **Near cache (hybrid):** local L1 + distributed L2. Best of both, invalidation complex.

### A12.3 — Cache invalidation at scale
- **TTL:** simple, eventual freshness.
- **Write-through/behind:** write pe update.
- **Event-based (CDC):** DB change → invalidate (accurate).
- **Versioned keys:** `user:123:v2` — old version naturally expire (no explicit delete).

### A12.4 — Cache warming
Cold cache (restart/deploy) → sab miss → DB overwhelmed. Pre-load hot data (warm up) before
serving traffic.

### A12.5 — Negative caching
"Not found" bhi cache karo (short TTL) — repeated queries for non-existent data DB tak na jaayein
(cache penetration defense).

---

<a name="a13"></a>
## A13. Networking — deep dive

### A13.1 — TLS handshake (HTTPS)
```
1. ClientHello (supported ciphers, TLS version)
2. ServerHello + certificate (public key)
3. Client verifies cert (CA chain), generates session key, encrypts with server public key
4. Both derive symmetric session key -> encrypted communication
```
- **Asymmetric** (public/private key) for handshake, **symmetric** (fast) for data.
- **TLS termination:** usually LB pe (CPU-heavy decryption offload).
- **mTLS (mutual TLS):** client bhi certificate deta (service-to-service auth, zero-trust).

### A13.2 — HTTP/1.1 vs HTTP/2 vs HTTP/3
| | HTTP/1.1 | HTTP/2 | HTTP/3 |
|---|---|---|---|
| Connections | one request/response at a time (head-of-line block) | multiplexing (many on one conn) | multiplexing over QUIC |
| Transport | TCP | TCP | **UDP (QUIC)** |
| Header | plain text | compressed (HPACK) | compressed |
| HoL blocking | yes | TCP-level still | no (QUIC solves) |
> HTTP/3 (QUIC) — faster connection setup, no TCP HoL blocking, better on lossy networks (mobile).

### A13.3 — Connection management
- **Keep-alive:** TCP connection reuse (handshake overhead avoid).
- **Connection pooling:** pre-established connections (DB, HTTP clients) — reuse.
- **TCP handshake:** 3-way (SYN, SYN-ACK, ACK) — 1 RTT before data. TLS adds more.

### A13.4 — Load balancing at network level
- **DNS load balancing:** multiple A records (round-robin). Coarse (caching, no health awareness).
- **Anycast:** same IP, multiple locations (BGP routing to nearest). CDN, DNS.

---

<a name="a14"></a>
## A14. Security — deep dive

### A14.1 — Encryption
- **In transit:** TLS/HTTPS (always). mTLS for internal.
- **At rest:** DB/disk encryption (AES). Key management (AWS KMS, HashiCorp Vault).
- **Symmetric (AES):** same key encrypt/decrypt (fast, bulk data).
- **Asymmetric (RSA):** public/private (key exchange, signatures).
- **End-to-end (E2E):** only sender/receiver decrypt (server plaintext nahi — WhatsApp/Signal).

### A14.2 — Authentication deep
- **OAuth 2.0:** delegated authorization (login with Google — token, no password share).
- **OIDC:** OAuth + identity layer (authentication).
- **JWT:** stateless token (header.payload.signature) — self-contained, scale-friendly.
- **API keys:** service-to-service.
- **mTLS:** certificate-based mutual auth (zero-trust internal).

### A14.3 — Common attacks + defense
| Attack | Defense |
|---|---|
| SQL injection | parameterized queries, ORM, input validation |
| XSS (cross-site scripting) | output encoding, CSP headers |
| CSRF | CSRF tokens, SameSite cookies |
| DDoS | rate limiting, WAF, CDN, auto-scale |
| Man-in-middle | TLS/HTTPS |
| Brute force | rate limiting, account lockout, CAPTCHA |
| Data breach | encryption at rest, least privilege, audit |

### A14.4 — Secrets management
Passwords/keys/tokens code me NAHI (git leak). Vault (HashiCorp), AWS Secrets Manager, env vars,
rotation. Principle of least privilege.

### A14.5 — Zero-trust architecture
"Never trust, always verify" — internal traffic bhi authenticate (mTLS), no implicit trust based
on network location.

---

<a name="a15"></a>
## A15. Resilience — autoscaling, load shedding, backpressure

### A15.1 — Autoscaling
- **Horizontal Pod/instance Autoscaler:** metrics (CPU/memory/QPS) threshold → add/remove instances.
- **Reactive:** load badha → scale up (delay — new instances warm up).
- **Predictive:** ML/schedule based (known peak — pre-scale, e.g. sale time).
- **Scale-in carefully:** graceful (drain connections before terminate).

### A15.2 — Load shedding
Overload me non-critical requests **drop** karo (system bacha lo). 503 return. Priority-based
(paid users first, critical APIs first). "Graceful degradation over total collapse."

### A15.3 — Backpressure
Downstream slow → upstream ko signal (slow down / stop). Queue bounded (unbounded = OOM crash).
Reactive streams, bounded buffers, TCP flow control.

### A15.4 — Rate limiting (system protection)
Section 26.8. API abuse + overload defense. Token bucket / sliding window. 429 + Retry-After.

### A15.5 — Bulkhead
Resources isolate (thread pools per service/tenant) — ek ka failure/overload poora system na le
doobe (ship compartments jaisa).

### A15.6 — Timeouts + retries + circuit breaker (recap)
- Timeout: har call bounded (hang na ho).
- Retry: exponential backoff + jitter (thundering herd avoid).
- Circuit breaker: repeated fail → open (fail fast) → half-open (test) → closed.
- **Idempotency zaroori** (retry safe).

---

<a name="a16"></a>
## A16. NoSQL data modeling (access-pattern driven)

SQL: normalize first, query later. **NoSQL: queries first, model accordingly** (denormalize).

### A16.1 — Principles
- **Access patterns pehle define karo** — kaunsi queries chahiye?
- **Denormalize** — joins nahi, related data embed/duplicate.
- **Model for reads** — write time pe extra kaam (duplicate), read time fast.
- **Single-table design (DynamoDB):** ek table, multiple entity types (partition + sort key clever use).

### A16.2 — Partition key + Sort key (DynamoDB)
```
Partition key: data distribution (even, high cardinality) — determines shard
Sort key: within partition ordering + range queries
Example (chat): PK = conversationId, SK = timestamp -> "conversation ke messages, time-ordered"
```

### A16.3 — Common patterns
- **Composite keys:** `USER#123 | PROFILE`, `USER#123 | ORDER#456` (one table, hierarchical).
- **Denormalized aggregates:** likeCount stored (not COUNT query).
- **Materialized views / GSI (Global Secondary Index):** alternate access pattern.
- **Write sharding:** hot partition avoid (`key#randomBucket`).

### A16.4 — Trade-offs
Denormalization = fast reads but **update anomaly** (data duplicate — update multiple jagah,
consistency risk). Eventual consistency accept karna padta.

---

<a name="a17"></a>
## A17. Specialized databases

### A17.1 — Time-series DB (InfluxDB, TimescaleDB, Prometheus)
Metrics, IoT, monitoring — time-stamped data, append-heavy, time-range queries.
- **Optimizations:** time-based partitioning, downsampling (old data aggregate), retention
  policies (auto-delete old), compression.

### A17.2 — OLAP / Columnar (Redshift, BigQuery, ClickHouse, Snowflake)
Analytics — aggregations over huge data. **Columnar storage** (column-wise, not row-wise):
```
Row store:    [id,name,age][id,name,age]...  -> good for "get full row"
Column store: [id,id,id][name,name][age,age] -> good for "SUM(age)" (read one column)
```
- **Fayda:** aggregation queries fast (only needed columns read), better compression.
- **OLTP vs OLAP:** OLTP = transactions (row store, normalized). OLAP = analytics (column store, denormalized).

### A17.3 — Graph DB (Neo4j)
Relationships-heavy (social network, recommendations, fraud). "Friends of friends" — graph
traversal (SQL joins me expensive).

### A17.4 — Vector DB (Pinecone, Milvus) — AI era
Embeddings (vectors) similarity search (nearest neighbor). RAG, recommendation, semantic search.
ANN (Approximate Nearest Neighbor) algorithms (HNSW).

---

<a name="a18"></a>
## A18. Infrastructure — containers, orchestration, service mesh

### A18.1 — Containers (Docker)
App + dependencies ek portable unit. Consistent across environments ("works on my machine" solve).
Lightweight (VM se — shared OS kernel).

### A18.2 — Kubernetes (K8s) basics
Container orchestration — deploy, scale, heal, network containers at scale.
- **Pod:** smallest unit (1+ containers).
- **Deployment:** desired state (replicas), rolling updates.
- **Service:** stable endpoint + load balancing for pods.
- **HPA:** horizontal pod autoscaler (metrics based).
- **Self-healing:** pod crash → restart, node down → reschedule.

### A18.3 — Service Mesh (Istio, Linkerd)
Service-to-service communication ki dedicated layer (sidecar proxy per service):
- **Handles:** mTLS, retries, circuit breaking, load balancing, observability — **app code ke bahar**.
- **Sidecar pattern** — proxy container har service ke saath.
- App business logic pe focus, networking mesh handle karta.

### A18.4 — Deployment strategies
- **Rolling:** gradually replace old with new (zero downtime, slow).
- **Blue-green:** 2 environments, instant switch (fast rollback, 2x resources).
- **Canary:** small % to new (gradual, risk minimize, monitor).
- **Feature flags:** code deploy but feature off, gradually enable (decouple deploy from release).

---

<a name="a19"></a>
## A19. Capacity planning + cost optimization

### A19.1 — Capacity planning
- **Estimate load** (QPS, storage, growth).
- **Headroom** (peak + buffer, ~2-3x average).
- **Bottleneck identify** (CPU/memory/IO/network — kaunsa pehle saturate).
- **Load testing** (simulate peak, find breaking point).

### A19.2 — Cost optimization ⭐ (senior signal)
- **Right-sizing:** over-provisioned resources kam karo.
- **Auto-scaling:** demand ke hisaab se (idle resources = waste).
- **Storage tiering:** hot (SSD/expensive) vs cold (Glacier/cheap) — access frequency se.
- **Reserved/spot instances:** predictable load → reserved (cheaper), fault-tolerant → spot.
- **CDN:** origin bandwidth cost bachao (edge serves).
- **Data lifecycle:** archive/delete old data (retention policy).
- **Caching:** DB/compute cost kam (cache hit = no expensive query).

---

<a name="a20"></a>
## A20. Reliability engineering (SRE)

### A20.1 — SLA vs SLO vs SLI
- **SLI (Indicator):** actual measured metric (99.95% requests < 200ms).
- **SLO (Objective):** internal target (99.9% availability).
- **SLA (Agreement):** customer contract + penalty (99.9% or refund). SLA < SLO (buffer).

### A20.2 — Error budget
`100% - SLO` = allowed failure. 99.9% SLO → 0.1% error budget (~43 min/month). Budget bacha →
ship features fast. Budget khatam → freeze, reliability pe focus. Balances velocity vs stability.

### A20.3 — Chaos engineering
Production me jaan-boojh ke failures inject (Netflix Chaos Monkey — random instances kill) →
resilience verify. "Break it before it breaks you." Hypothesis → experiment → learn.

### A20.4 — Disaster recovery
- **RTO (Recovery Time Objective):** kitni der me recover (downtime tolerance).
- **RPO (Recovery Point Objective):** kitna data loss acceptable (backup frequency).
- **Backups:** regular, tested (untested backup = no backup), geo-redundant.
- **Failover:** automated (fast) vs manual (safe).

### A20.5 — Observability (recap + deep)
- **Logs:** events (structured, centralized — ELK).
- **Metrics:** numbers over time (Prometheus + Grafana). RED (Rate/Errors/Duration).
- **Traces:** request journey across services (Jaeger). Bottleneck locate.
- **Alerting:** SLO breach → page on-call. Actionable alerts only (alert fatigue avoid).

---

## 📖 Advanced topics — summary index

| Topic | Section | Key idea |
|---|---|---|
| Sharding deep | A1 | shard key, resharding, cross-shard, hot shard |
| Replication deep | A2 | topologies, sync/async, conflict resolution, quorum |
| Distributed transactions | A3 | 2PC, Saga (choreography/orchestration), TCC, Outbox |
| Consensus + consistency | A4 | Raft, linearizability vs serializability, PACELC |
| Probabilistic structures | A5 | Bloom, Count-Min, HyperLogLog, Merkle |
| Consistent hashing deep | A6 | virtual nodes, bounded loads, rendezvous |
| Geospatial deep | A7 | geohash, quadtree, S2, H3, R-tree |
| Search systems | A8 | inverted index, TF-IDF/BM25, ranking |
| Stream processing | A9 | windowing, watermarks, exactly-once, Lambda/Kappa |
| Change Data Capture | A10 | log-based CDC, Debezium |
| Multi-region | A11 | active-active, conflict, GeoDNS, data residency |
| Advanced caching | A12 | multi-level, near cache, warming, negative |
| Networking deep | A13 | TLS, HTTP/2/3, QUIC, connection pooling |
| Security deep | A14 | encryption, OAuth/JWT/mTLS, attacks, secrets |
| Resilience | A15 | autoscaling, load shedding, backpressure, bulkhead |
| NoSQL modeling | A16 | access-pattern driven, single-table, PK/SK |
| Specialized DBs | A17 | time-series, OLAP/columnar, graph, vector |
| Infra/K8s | A18 | containers, Kubernetes, service mesh, deploy strategies |
| Capacity + cost | A19 | planning, right-sizing, tiering, spot/reserved |
| Reliability (SRE) | A20 | SLA/SLO/SLI, error budget, chaos, DR (RTO/RPO) |

---

## 📚 Sources (web research)
- [InterviewBit — System Design Interview Questions](https://www.interviewbit.com/system-design-interview-questions/)
- [DesignGurus — 25 System Design Fundamentals](https://www.designgurus.io/blog/system-design-interview-fundamentals)
- [DesignGurus — Top System Design Questions (FAANG)](https://www.designgurus.io/blog/system-design-interview-questions-to-crack-your-next-faang-interview)
- [GeeksforGeeks — System Design Tutorial](https://www.geeksforgeeks.org/system-design/system-design-tutorial/)
- [Educative — Grokking the System Design Interview](https://www.educative.io/courses/grokking-the-system-design-interview)
- [DEV — 64 System Design Questions Ranked](https://dev.to/arslan_ah/64-system-design-interview-questions-ranked-from-easiest-to-hardest-260m)
- [System Design Handbook — Top 40 Questions](https://www.systemdesignhandbook.com/guides/system-design-interview-questions/)
- [Educative — System Design Interview Questions 2025](https://www.educative.io/blog/system-design-interview-questions)








# 📕 High-Level Design (HLD) — System Design Master Folder

<p align="center">
  <img src="https://img.shields.io/badge/Focus-System%20Design-e63946?style=for-the-badge" alt="System Design" />
  <img src="https://img.shields.io/badge/Deep%20Dives-32%20topics-2F81F7?style=for-the-badge" alt="topics" />
  <img src="https://img.shields.io/badge/Interview%20Guide-3800%2B%20lines-16a34a?style=for-the-badge" alt="Guide" />
  <img src="https://img.shields.io/badge/Diagrams-Mermaid-FB8500?style=for-the-badge" alt="Mermaid" />
</p>

> **High-Level Design (HLD)** = poore system ka architecture — scalability, databases, caching,
> distributed systems, security. Yahan **32 detailed topic deep-dives** (diagrams + tables +
> advantages/disadvantages + interview Q&A) + ek complete **HLD interview guide** hai. Har file
> Hinglish me, interview-focused. LLD (code-level) ke liye [`../LLD/`](../LLD/) dekho.

---

## 🧭 Quick Navigation

| Chahiye | Jao |
|---|---|
| **Complete interview guide** (framework + 20+ system designs) | [`HLD_Interview.md`](./HLD_Interview.md) |
| **21 core topics** (structured course) | [Section neeche](#-part-1--21-core-topics) |
| **11 deep-dive topics** (advanced) | [Section neeche](#-part-2--advanced-deep-dives) |
| **Theory notes** | [`System_Design_theory.md`](./System_Design_theory.md) |
| LLD (code, patterns, C++) | [`../LLD/`](../LLD/README.md) |

---

## 📘 Complete HLD Interview Guide

[`HLD_Interview.md`](./HLD_Interview.md) — **3800+ lines** ka comprehensive guide:
- **RESHADED framework** (Requirements → Estimation → Schema → High-level → API → Deep-dive → Evaluate → Discuss)
- Scalability, load balancing, caching, databases, CAP, message queues, networking, microservices
- **20+ full system designs** — TinyURL, Twitter, WhatsApp, Uber, YouTube, Dropbox, Payment, Rate Limiter...
- **PART II Advanced** — sharding, replication, consensus (Raft), distributed transactions, probabilistic
  structures, geo-indexing, streaming, CDC, multi-region, security, SRE, NoSQL modeling

> Topic files (neeche) = focused deep-dives per concept. Interview guide = end-to-end framework +
> full system designs. **Dono padho.**

---

## 📚 PART 1 — 21 Core Topics

Structured system-design course (sequential padho). Har file: concept + diagrams + tables + Q&A.

### 🏗️ Foundations & Networking (1–5)
| # | Topic | Kya cover |
|---|---|---|
| 01 | [Monolithic vs Microservices](./01_Monolithic_and_Microservices.md) ⭐ | architectures + **microservices design patterns by phase** (Strangler Fig, **Saga deep**, CQRS, Event Sourcing, Outbox, Canary, Circuit Breaker) |
| 02 | [API Gateway & Load Balancer](./02_API_Gateway_and_Load_Balancer.md) | gateway vs LB, cross-cutting, aggregation, resiliency |
| 03 | [Load Balancer — Types & Algorithms](./03_Load_Balancer_Types_and_Algorithms.md) | L4 vs L7, RR/least-conn/hash, health checks, NAT/DSR |
| 04 | [Proxy & Reverse Proxy](./04_Proxy_and_Reverse_Proxy.md) | forward vs reverse, use cases, SSL/cache/security mechanics |
| 05 | [Network Protocols](./05_Network_Protocols.md) | client-server vs P2P, TCP/UDP, HTTP/1-2-3, WebSocket, REST/gRPC/GraphQL |

### 📈 Scaling & Caching (6–8)
| # | Topic | Kya cover |
|---|---|---|
| 06 | [Scaling — Vertical & Horizontal](./06_Scaling_Vertical_and_Horizontal.md) | scale up vs out, stateless, DB scaling, autoscaling |
| 07 | [Scale App 0 → Million](./07_Scale_Application_0_to_Million.md) | full evolution: 1 server → sharding → microservices |
| 08 | [Caching & Distributed Caching](./08_Caching_and_Distributed_Caching.md) | strategies, eviction, invalidation, thundering herd, Redis |

### 🌐 Distributed Systems (9–11)
| # | Topic | Kya cover |
|---|---|---|
| 09 | [Introduction to Distributed Systems](./09_Introduction_to_Distributed_Systems.md) | challenges, fallacies, consensus, replication, failure handling |
| 10 | [Content Delivery Network (CDN)](./10_Content_Delivery_Network_CDN.md) | **kaise kaam karta**, push vs pull, invalidation, anycast |
| 11 | [CAP Theorem](./11_CAP_Theorem.md) | C/A/P, CP vs AP, PACELC, consistency models |

### 🛡️ Rate Limiting & Security (12–15)
| # | Topic | Kya cover |
|---|---|---|
| 12 | [Rate Limiting & Algorithms](./12_Rate_Limiting_and_Algorithms.md) | token/leaky bucket, fixed/sliding window |
| 13 | [Rate Limiting Part-2 (Distributed)](./13_Rate_Limiting_Part_2.md) | Redis, race conditions, atomicity (Lua), fail open/closed |
| 14 | [SSL Certificate](./14_SSL_Certificate.md) | TLS handshake, certificates, CA chain, mTLS |
| 15 | [Rate Limiting Strategies](./15_Rate_Limiting_Strategies.md) | placement, dimensions, tiers, 429 headers, graceful |

### 🗄️ Data & Reliability (16–21)
| # | Topic | Kya cover |
|---|---|---|
| 16 | [Database Design Tips](./16_Database_Design_Tips.md) | SQL vs NoSQL, indexing, normalization, transactions, pooling |
| 17 | [Avoid Single Point of Failure](./17_Avoid_Single_Point_of_Failure.md) | redundancy, failover, multi-region, availability math |
| 18 | [Message Queues (Kafka/RabbitMQ)](./18_Message_Queues_Kafka_RabbitMQ.md) | queue vs pub-sub, Kafka/RabbitMQ, delivery guarantees |
| 19 | [Consistent Hashing](./19_Consistent_Hashing.md) | hash ring, virtual nodes, resharding, replication |
| 20 | [Back-of-the-Envelope Calculations](./20_Back_of_the_Envelope_Calculations.md) | numbers, formulas, worked examples (TinyURL/Twitter/Chat) |
| 21 | [Database Sharding](./21_Database_Sharding.md) | strategies, shard key, resharding, cross-shard, hotspots |

---

## 🔬 PART 2 — Advanced Deep-Dives

Extra detailed standalone topics (advantages/disadvantages + diagrams + interview Q&A). Ye 21 core
topics ko complement karte hain.

### 🗃️ Databases & Data
| Topic | Kya cover |
|---|---|
| [**SQL vs NoSQL**](./SQL_vs_NoSQL.md) ⭐ (1000+ lines) | ACID/BASE, all NoSQL types, data modeling, scaling, transactions, CAP, popular DBs, polyglot, real-world case studies |
| [**Database Replication**](./Database_Replication.md) | master-slave/multi-master/leaderless, sync/async, replication lag, conflict resolution, quorum, failover, read replicas |
| [**Database Sharding**](./21_Database_Sharding.md) | (core topic 21 — sharding ka jodidaar replication ke saath) |

### 🔒 Consistency & Transactions
| Topic | Kya cover |
|---|---|
| [**Idempotency**](./Idempotency.md) | idempotency key, HTTP methods, dedup store, race conditions, exactly-once, payments |
| [**Concurrency Control**](./Concurrency_Control.md) | optimistic vs pessimistic locking, distributed locks (Redis/Zookeeper), deadlocks, isolation levels |
| [**Distributed Transactions**](./Distributed_Transactions.md) | 2PC, 3PC, Saga, TCC, Outbox — mechanisms + pros/cons |

### 🌐 Architecture & Communication
| Topic | Kya cover |
|---|---|
| [**API Design**](./API_Design.md) | REST best practices, HTTP methods/status, versioning, pagination, errors, REST vs gRPC vs GraphQL |
| [**Event-Driven Architecture**](./Event_Driven_Architecture.md) | events, pub-sub, event streaming, event sourcing, CQRS, pros/cons |
| [**Stateful vs Stateless Architecture**](./Stateful_and_Stateless_Architecture.md) | state, scaling implications, sticky sessions, externalize state, JWT vs session |
| [**WebSockets & Real-Time**](./WebSockets_and_Realtime.md) | polling/long-polling/WebSocket/SSE, WebSocket deep, scaling millions of connections |

### 🛡️ Security & Data Structures
| Topic | Kya cover |
|---|---|
| [**Security in System Design**](./Security_in_System_Design.md) | AuthN vs AuthZ, OAuth/JWT, encryption, attacks (SQLi/XSS/CSRF/DDoS) + defenses, secrets, zero-trust |
| [**Bloom Filters & Probabilistic DS**](./Bloom_Filters_and_Probabilistic_Data_Structures.md) | Bloom filter, Count-Min Sketch, HyperLogLog, Merkle tree, Skip list |

---

## 🎯 System Design Interview — RESHADED Framework

```mermaid
flowchart LR
    R[R: Requirements] --> E[E: Estimation]
    E --> S[S: Schema/Data]
    S --> H[H: High-level design]
    H --> A[A: API]
    A --> D1[D: Deep dive]
    D1 --> E2[E: Evaluate/bottlenecks]
    E2 --> D2[D: Discuss trade-offs]
```

1. **Requirements** — functional + non-functional + scope (read:write ratio poocho)
2. **Estimation** — QPS, storage, bandwidth ([topic 20](./20_Back_of_the_Envelope_Calculations.md))
3. **Schema/Data** — SQL vs NoSQL, data model ([SQL vs NoSQL](./SQL_vs_NoSQL.md), [topic 16](./16_Database_Design_Tips.md))
4. **High-level design** — components + data flow (draw karo)
5. **API** — key endpoints ([API Design](./API_Design.md))
6. **Deep dive** — 1-2 components in detail
7. **Evaluate** — bottlenecks, SPOF ([topic 17](./17_Avoid_Single_Point_of_Failure.md)), scaling
8. **Discuss** — trade-offs (CAP, cost, consistency)

---

## 🗺️ Topic map by theme (kaunsa concept kahan)

```mermaid
flowchart TB
    A[System Design] --> B[Scaling: 06, 07, Stateful/Stateless]
    A --> C[Databases: 16, 21, SQL-vs-NoSQL, Replication]
    A --> D[Distributed: 09, 11, 19, Distributed-Txns, Concurrency]
    A --> E[Communication: 02-05, API-Design, Event-Driven, WebSockets]
    A --> F[Caching + CDN: 08, 10]
    A --> G[Reliability: 17, Idempotency, Message-Queues-18]
    A --> H[Security: 14, Security-in-SD, Rate-Limiting 12-13-15]
    A --> I[Data structures: 20, Bloom-Filters]
```

---

## 📖 Recommended study path
1. **Topics 01–11** — core (foundations → distributed systems).
2. **Topics 12–21** — rate limiting, security, data, reliability.
3. **Deep-dives** — SQL vs NoSQL, Replication, Idempotency, Concurrency, API Design, Security,
   Event-Driven, WebSockets, Distributed Transactions, Bloom Filters, Stateful/Stateless.
4. **[`HLD_Interview.md`](./HLD_Interview.md)** — RESHADED + full system designs.
5. **Practice** — pick a system (Twitter/Uber/WhatsApp), apply RESHADED, use topic files as reference.

---

## 📂 All files in this folder (32 topic deep-dives + guide)

```
HLD/
├── README.md                    ← you are here (index)
├── HLD_Interview.md             ← complete guide (3800+ lines)
├── System_Design_theory.md      ← theory notes
├── 01..21_*.md                  ← 21 core topics
├── SQL_vs_NoSQL.md              ← deep (1000+)
├── Database_Replication.md
├── Idempotency.md
├── Concurrency_Control.md
├── API_Design.md
├── Security_in_System_Design.md
├── Event_Driven_Architecture.md
├── Bloom_Filters_and_Probabilistic_Data_Structures.md
├── WebSockets_and_Realtime.md
├── Distributed_Transactions.md
└── Stateful_and_Stateless_Architecture.md
```

---

> 💡 **Combine with LLD** — HLD (ye folder) = poore system ka architecture; LLD ([`../LLD/`](../LLD/))
> = ek component ke andar ka code (OOP, patterns, C++). Dono FAANG/product interviews me aate hain.
>
> **Ja ke crack kar de bhai! 🚀**

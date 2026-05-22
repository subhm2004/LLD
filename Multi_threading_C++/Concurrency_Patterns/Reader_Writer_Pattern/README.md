# Reader-Writer Pattern — Detailed Guide (C++17)

## 1. Pattern kya hai?

**Reader-Writer Pattern** = shared data par do tarah ke access:

| Access | Kitne threads | Lock type |
|--------|---------------|-----------|
| **Read** | Multiple readers **ek saath** | Shared / read lock |
| **Write** | Sirf **ek** writer | Exclusive / write lock |

**Rule:** Jab writer kaam kar raha ho → **koi reader nahi**. Jab readers padh rahe hon → writer wait.

**Analogy:** Library book — bahut log **ek saath padh** sakte hain (read), lekin **edit** karte waqt koi aur na pade na likhe (write exclusive).

---

## 2. Kyun chahiye? (vs normal `mutex`)

| Sirf `mutex` | Reader-Writer lock |
|--------------|-------------------|
| Har read bhi exclusive | Reads **parallel** |
| Read-heavy apps slow | Throughput zyada |
| Simple | Thoda complex |

**Kab use karo:**

- Config / cache — reads 1000×, writes kam
- In-memory database index
- Shared document / stats counter read-mostly
- Connection metadata lookup

**Kab mat use karo:**

- Writes zyada ya read/write barabar — simple `mutex` kaafi
- Data chhota + critical section 1 line — RW lock overhead waste
- Strong fairness chahiye — custom policy design karni padegi

---

## 3. Core components

| Piece | Role |
|-------|------|
| Shared data | `int`, `map`, string buffer, etc. |
| **Read lock** | `shared_lock<shared_mutex>` ya `lock_shared()` |
| **Write lock** | `unique_lock<shared_mutex>` ya `lock()` |
| Reader count / writer flag | Custom impl mein (see `ReadWriteLock.h`) |
| `condition_variable` | Custom lock mein wait/notify |

### C++17 standard way

```cpp
shared_mutex mtx;

// reader
shared_lock<shared_mutex> lock(mtx);

// writer
unique_lock<shared_mutex> lock(mtx);
```

---

## 4. Folder structure

```
Reader_Writer_Pattern/
├── ReadWriteLock.h           ← custom + writer-preference variant
├── 01_std_shared_mutex_basics.cpp
├── 02_custom_read_write_lock.cpp
├── 03_many_readers_parallel.cpp
├── 04_writer_exclusive_access.cpp
├── 05_thread_safe_cache.cpp
├── 06_reader_vs_writer_preference.cpp
├── docs/
├── compile.sh / run_all.sh
└── bin/
```

---

## 5. Build & run

```bash
cd Reader_Writer_Pattern
./compile.sh
./bin/01_std_shared_mutex_basics
./run_all.sh
```

---

## 6. Har demo — kya ho raha hai

### 01 — `01_std_shared_mutex_basics.cpp`

**Kya:** C++17 `std::shared_mutex` — 2 readers + 1 writer.

- `shared_lock` → parallel reads of `shared_value`
- `unique_lock` → writer `++shared_value` exclusive

**Fayda:** Production mein yahi use karo jab standard library kaafi ho.

**Detail:** [`docs/01_std_shared_mutex_basics.md`](./docs/01_std_shared_mutex_basics.md)

---

### 02 — `02_custom_read_write_lock.cpp`

**Kya:** `ReadWriteLock.h` — interview / learning ke liye manual logic.

- `lock_shared` / `unlock_shared` for readers
- `lock` / `unlock` for writer

**Fayda:** Whiteboard par explain kar sakte ho "andar kaise kaam karta hai".

**Detail:** [`docs/02_custom_read_write_lock.md`](./docs/02_custom_read_write_lock.md)

---

### 03 — `03_many_readers_parallel.cpp`

**Kya:** 5 reader threads, **no writer** — `max_concurrent_reads` track.

**Proof:** Value > 1 → reads sach mein parallel.

**Detail:** [`docs/03_many_readers_parallel.md`](./docs/03_many_readers_parallel.md)

---

### 04 — `04_writer_exclusive_access.cpp`

**Kya:** Writer 200ms exclusive section — readers snapshot verify (no change mid-read).

**Fayda:** Data race / torn read nahi — consistency during read.

**Detail:** [`docs/04_writer_exclusive_access.md`](./docs/04_writer_exclusive_access.md)

---

### 05 — `05_thread_safe_cache.cpp`

**Kya:** `ThreadSafeCache` — `get()` shared lock, `put()` exclusive.

**Real use:** User session cache, config store.

**Detail:** [`docs/05_thread_safe_cache.md`](./docs/05_thread_safe_cache.md)

---

### 06 — `06_reader_vs_writer_preference.cpp`

**Kya:**

- `ReadWriteLock` — **reader-preference** (writers wait if readers active)
- `WriterPreferenceRWLock` — writer wait kare to **naye readers block**

**Fayda:** Starvation trade-off samajh — policy workload par depend.

**Detail:** [`docs/06_reader_vs_writer_preference.md`](./docs/06_reader_vs_writer_preference.md)

---

## 7. Reader vs Writer preference

| Policy | Fayda | Nuksan |
|--------|-------|--------|
| **Reader preference** | Read throughput max | Writer starve ho sakta |
| **Writer preference** | Writes timely | Readers slow ho sakte |
| **Fair (FIFO)** | Balanced | Implementation heavy |

---

## 8. Doosre patterns se link

| Pattern | Relation |
|---------|----------|
| [Signaling](../Signaling_Pattern/) | Custom RW lock = `condition_variable` |
| [Producer-Consumer](../Producer_Consumer_Pattern/) | Alag problem — buffer, not RW |
| `mutex` only | RW special case when reads dominate |

---

## 9. Interview Q&A

| Sawal | Jawab |
|-------|-------|
| RW vs mutex? | Parallel reads allowed — better read-heavy perf |
| `shared_mutex` C++17? | Yes — `shared_lock` / `unique_lock` |
| Writer active readers? | Blocked — exclusive |
| Deadlock? | Nested write while holding read — avoid |
| Upgrade lock? | `shared_mutex` direct upgrade nahi — release read, acquire write |

---

## 10. Extended theory

[`READER_WRITER_PATTERN_COMPLETE.md`](./READER_WRITER_PATTERN_COMPLETE.md)

## 11. Learning order

```
01 (std) → 02 (custom) → 03 → 04 → 05 → 06
```

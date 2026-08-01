# Design Patterns Used — Consistent Hashing LLD

Ye system ek **data-structure LLD** hai (behavioral simulation nahi) — iska dil ek sorted hash ring
hai. Uske upar teen pattern lage hain: hash function ko pluggable banane ke liye **Strategy**, poore
system ka single entry point dene ke liye **Facade**, aur topology change pe baaki duniya ko khabar
dene ke liye **Observer**.

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                            PATTERN OVERVIEW                                    │
├──────────────────┬─────────────────────────────────┬─────────────────────────┤
│  Pattern         │  Kahan (class)                  │  Kis liye                │
├──────────────────┼─────────────────────────────────┼─────────────────────────┤
│  Strategy ⭐     │  HashFunction + 2 impls         │  hashing algorithm swap  │
│  Facade / Context│  ConsistentHashRing             │  system entry + hash fn  │
│                  │                                 │  hold karta hai          │
│  Observer ⭐     │  RingChangeListener + 2 impls   │  topology change ki       │
│                  │                                 │  khabar (migration/log)  │
│  Health-aware    │  Node.isAvailable() + status    │  DOWN nodes skip, par     │
│  routing         │                                 │  ring pe bane rehte      │
└──────────────────┴─────────────────────────────────┴─────────────────────────┘
```

---

## 1. Strategy Design Pattern ⭐ (placement ka dil)

### 📌 Concept
Ek algorithm ko interface ke peeche rakh do taaki wo runtime pe swap ho sake, aur naye algorithm
bina purana code chhue add ho sakein.

### PATTERN KE ROLES
- **Strategy interface** → [`HashFunction`](./strategies/HashFunction.h) (`hash`, `name`)
- **Concrete Strategies**:
  - [`FnvHashFunction`](./strategies/FnvHashFunction.h) — FNV-1a + murmur finalizer (simple, fast)
  - [`Murmur3HashFunction`](./strategies/Murmur3HashFunction.h) — MurmurHash3 32-bit (Cassandra wala)
- **Context** → [`ConsistentHashRing`](./core/ConsistentHashRing.h) — `unique_ptr<HashFunction>` hold karta hai

### Implementation
```cpp
ConsistentHashRing ring(make_unique<Murmur3HashFunction>(), 150, 3); // strategy inject
ring.getNode("session:42");                    // andar: hashFunction_->hash(key)
ring.setHashFunction(make_unique<FnvHashFunction>()); // ⭐ SWAP -> ring REBUILD
```

### ⭐ KYUN? (Why Strategy)
> Naya hash chahiye (`XxHashFunction`, `Md5HashFunction` ketama-compatible ke liye)? Bas nayi class
> banao jo `HashFunction` implement kare — `ConsistentHashRing`, `Node`, `main` ka ek line nahi
> badlega. Yahi **Open/Closed Principle** + NFR6 ("extensible for new hash functions").

### ⭐⭐ SABSE ZAROORI INTERVIEW POINT — swap MUFT nahi hai
> [`LoadBalancer_LLD`](../LoadBalancer_LLD/) me strategy swap **muft** tha: `setStrategy()` karo,
> agli request pe naya algorithm chal padta hai. Bas.
>
> **Yahan aisa BILKUL nahi hai.** Hash function badla = har node ki ring position badal gayi =
> poora ring dobara banana padega = **lagbhag saari keys move**. Demo me ye naapa gaya hai:
> swap pe **~74% keys** ne node badla.
>
> Isi liye `setHashFunction()` andar se ring rebuild karta hai aur `onRingRebuilt` event bhejta hai.
> Production me ye ek **planned migration** hoti hai, "runtime toggle" nahi.
>
> **Seekh:** pattern lagana aasan hai, par uske swap ki **keemat domain decide karta hai** —
> pattern nahi. Ye baat interview me bahut achhi lagti hai.

---

## 2. Facade Pattern (+ Strategy's Context, + Observer's Subject) — `ConsistentHashRing`

### 📌 Concept
Facade ek complex subsystem ke aage patla interface rakh deta hai. Yahan `ConsistentHashRing` teen
role nibha raha hai: **Facade** (entry point), Strategy ka **Context**, aur Observer ka **Subject**.

### Implementation
Client ko ring positions, virtual nodes, clockwise walk, wrap-around, ya collision handling ki koi
khabar nahi. Wo bas kuch method jaanta hai:
```cpp
addNode(id, host, weight);   removeNode(id);    markNodeStatus(id, ACTIVE/DOWN);
getNode(key);                getNodes(key);     addListener(&listener);
```

### ⭐ KYUN?
> `getNode()` ke peeche "key hash karo → `lower_bound` → end pe ho to wrap → DOWN node skip karo"
> wala poora orchestration chhupa hua hai. Client ko sirf "is key ka maalik kaun hai" chahiye.

---

## 3. Observer Design Pattern ⭐ (topology change ki khabar)

### 📌 Concept
Subject apni state badalne pe registered observers ko notify karta hai — bina ye jaane ki wo kaun
hain ya karenge kya.

### PATTERN KE ROLES
- **Subject** → [`ConsistentHashRing`](./core/ConsistentHashRing.h) (`addListener`, `notify*`)
- **Observer interface** → [`RingChangeListener`](./observers/RingChangeListener.h)
- **Concrete Observers**:
  - [`ConsoleRingLogger`](./observers/ConsoleRingLogger.h) — audit/debug log
  - [`MigrationTracker`](./observers/MigrationTracker.h) — data migration ka plan banata hai

### ⭐ KYUN? (ye pattern yahan sach me zaroori hai, sirf dikhawa nahi)
> Sochiye agar Observer na hota — to `addNode()` ke andar seedha migration ka code likhna padta.
> Matlab ek **routing data-structure** ko data transfer, network, retry, progress tracking sab ki
> khabar rakhni padti. Wo class do-teen kaam ek saath karne lagti — **Single Responsibility** gaya
> paani me, aur unit testing bhi mushkil ho jaati.
>
> Observer se ring bas "**kya hua**" bolta hai; "**uska kya karna hai**" listener decide karta hai.

### ⭐ Optional hooks (`= 0` nahi, khaali body)
> `RingChangeListener` ke methods pure virtual **nahi** hain — unki khaali default body hai. Isse
> har listener sirf wahi event override karta hai jisme uski dilchaspi hai (`MigrationTracker` ko
> `onNodeStatusChanged` se zyada matlab nahi). Ye "adapter style" pure virtual se zyada practical hai.

---

## 4. Health-aware routing (technique, GoF nahi)

`Node` ka [`NodeStatus`](./enums/NodeStatus.h) (ACTIVE/DOWN) + `isAvailable()` lookup ko
"self-healing" banata hai — aur yahan ek **bahut zaroori domain distinction** hai:

| | DOWN | REMOVE |
|---|---|---|
| Ring positions | wahi rehti hain | hat jaati hain |
| Keys kahan jaati | clockwise agla ACTIVE node | clockwise agla node |
| Data migration | **NAHI** ⭐ | haan (re-replication) |
| Wapas aane pe | keys **free me** wapas | positions dobara compute |

> ⭐ Chhoti si network glitch pe node ko REMOVE karna **galti** hai — har blip pe TB-scale
> migration shuru ho jaayega. Isi liye `DOWN` alag concept hai, aur `MigrationTracker`
> jaan-bujh ke DOWN pe koi task nahi banata (demo me ye naap ke dikhaya gaya hai).

---

## Object Relationships (UML samajhne ke liye)

```
ConsistentHashRing ◇──  HashFunction          (Aggregation: unique_ptr, injected + swappable)
ConsistentHashRing ◆──  Node (unordered_map)  (Composition: unique_ptr, ring owns)
ConsistentHashRing ◆──  VirtualNode (map)     (Composition: by value, ring owns)
ConsistentHashRing ··▶  RingChangeListener    (Dependency: non-owning ptr, notify only)
VirtualNode        ··▶  Node                  (Dependency: NON-OWNING raw ptr)
FnvHashFunction     ─▷  HashFunction          (implements)
Murmur3HashFunction ─▷  HashFunction          (implements)
ConsoleRingLogger   ─▷  RingChangeListener    (implements)
MigrationTracker    ─▷  RingChangeListener    (implements)
Node               ──▶  NodeStatus            (has-a enum)
```
- `◆──` Composition (owns) · `◇──` Aggregation (injected, swappable) · `─▷` Inheritance · `··▶` Dependency

---

## Requirement → Pattern/Technique quick map

| Requirement | Kaise pura hua |
|-------------|----------------|
| node add/remove | `addNode` / `removeNode` (duplicate id reject, fail-fast) |
| health ACTIVE/DOWN | `NodeStatus` + `markNodeStatus` (positions untouched) |
| key ka owner | `getNode` → clockwise walk + wrap-around |
| RF distinct replicas | `getNodes` → `set` se duplicate **physical** node roka |
| weighted nodes | `Node::virtualNodeCount()` = base × weight |
| pluggable hash | **Strategy** (`HashFunction`) |
| swappable at runtime | **Strategy** `setHashFunction()` + ring rebuild |
| topology notifications | **Observer** (`RingChangeListener`) |
| ~1/N keys move (NFR2) | consistent hashing ka core — demo me naapa gaya |
| even distribution (NFR3) | **virtual nodes** (150/node) |
| O(log V) lookup (NFR1) | `std::map` (Red-Black tree) `lower_bound` |

---

## Data structure ka faisla (ye interview me pakka poocha jaata)

| Option | Kyun / kyun nahi |
|--------|------------------|
| `std::map<uint32_t, VirtualNode>` ✅ | Sorted hai → `lower_bound` = "clockwise agla node" O(log V). **Yahi chahiye.** |
| `unordered_map` ❌ | Order hota hi nahi. Yahan poora khel "agla bada element" pe hai — ye chal hi nahi sakta. |
| `vector` + binary search | Lookup thoda tez (cache-friendly), par add/remove O(V) — nodes aksar aate-jaate hain, to `map` behtar. |

---

## "Ye pattern kyun NAHI use kiya" (interview me pooche jaate hain)

- **Factory for hash functions?** Abhi client seedha `make_unique<Murmur3HashFunction>()` deta hai —
  2 hi strategies hain. Config/string se banani ho ("murmur3" → object) tab factory add karenge.
  Abhi **YAGNI**.
- **Singleton ring?** Ek application me kai rings ho sakti hain (ek cache cluster ke liye, ek shard
  map ke liye). Singleton wo rok deta, aur global state testing mushkil karta — inject karna behtar.
- **Builder for ring config?** Sirf 3 parameter hain (hash, vnodes, RF) — constructor kaafi hai.
  10+ optional params hote tab Builder banta.
- **Template `<typename Key>` kyun nahi?** `string` key rakhi taaki demo padhne me saaf rahe. Generic
  banana ho to `HashFunction` ko template kar sakte — par abhi complexity ka faayda nahi.
- **Thread-safety (mutex)?** Requirements single-process, deterministic in-memory maangte hain
  (Assumption 4). Concurrent lookups chahiye to `shared_mutex` chahiye hoga — reads bahut zyada
  aur writes na ke barabar hote hain, to reader-writer lock perfect fit hai.
  (HLD folder ke [capstone](../../HLD/Consistent_Hashing/ConsistentHashRing.h) me wahi version hai.)
- **Bounded loads / Rendezvous hashing?** Ye consistent hashing ke **variants** hain, is LLD ke
  scope me nahi. Unka runnable code HLD notes me alag se hai
  ([Consistent_Hashing/](../../HLD/Consistent_Hashing/README.md)).

---

## Cleanup notes

- Saare files **targeted includes** pe hain (`<map>`, `<memory>`, ...) — `bits/stdc++.h` nahi
  (wo non-portable, GCC-only hai). Yahi convention [`LoadBalancer_LLD`](../LoadBalancer_LLD/) me bhi hai.
- Nodes `unique_ptr` me own kiye hain → koi manual `delete` nahi, koi leak nahi, koi double-free nahi.
- `VirtualNode` andar **non-owning** raw `Node*` rakhta hai (ring hi maalik hai) — isliye ring ke
  copy/move `= delete` kiye hain, warna naye ring ke vnodes purane nodes pe point karte reh jaate
  (dangling pointer). Rule of Three/Five.
- Virtual node **hash collision** handle kiya hai: `emplace` (overwrite nahi) + remove pe
  "ye point sach me isi node ka hai?" check — warna doosre node ka point galti se hat jaata.

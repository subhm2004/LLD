# Consistent Hashing LLD (C++)

Modular low-level design for an in-memory consistent hashing ring with virtual nodes, replication,
weighted nodes and pluggable hash functions.

## Structure

- `main.cpp`
- `core/ConsistentHashRing.h`
- `models/Node.h`
- `models/VirtualNode.h`
- `models/RingBalanceStats.h`
- `strategies/HashFunction.h`
- `strategies/FnvHashFunction.h`
- `strategies/Murmur3HashFunction.h`
- `observers/RingChangeListener.h`
- `observers/ConsoleRingLogger.h`
- `observers/MigrationTracker.h`
- `enums/NodeStatus.h`

## Features

- Add / remove nodes on a hash ring
- Virtual nodes (150 per node) for even distribution
- Balance quality metrics — **standard deviation** + worst-node ratio (weight-aware)
- Weighted nodes — bigger server gets proportionally more keys
- Replication — preference list of `RF` **distinct physical** nodes
- Mark node `ACTIVE` / `DOWN` without moving ring positions
- Pluggable hash function, swappable at runtime:
  - FNV-1a (32-bit)
  - MurmurHash3 (32-bit)
- Topology change listeners (Observer):
  - Console logger
  - Migration tracker

## Run

```bash
./compile.sh
./consistent_hashing_app
```

Ya seedha:

```bash
g++ -std=c++17 main.cpp -o consistent_hashing_app
./consistent_hashing_app
```

## Design Patterns

| Pattern | Kahan | Kis liye |
|---------|-------|----------|
| **Strategy** ⭐ | `HashFunction` + 2 impls | hashing algorithm pluggable |
| **Facade** | `ConsistentHashRing` | client ka single entry point |
| **Observer** ⭐ | `RingChangeListener` + 2 impls | topology change ki khabar |

Poori detail: [design_patterns_used.md](./design_patterns_used.md)

## Demo kya naapta hai

### Virtual node tuning — std-dev minimize (4 nodes, 200 trials per row)

`vnodes` badhane se balance kitna sudharta hai — guess nahi, **naap ke**:

| vnodes/node | ring points | avg std-dev | avg worst ratio |
|---|---|---|---|
| 1 | 4 | 18.035 | 2.08x |
| 5 | 20 | 9.207 | 1.51x |
| 20 | 80 | 4.400 | 1.23x |
| 50 | 200 | 2.843 | 1.15x |
| 100 | 400 | 1.908 | 1.10x |
| **150** | 600 | **1.609** | **1.09x** |
| 250 | 1000 | 1.263 | 1.07x |
| 500 | 2000 | 0.887 | 1.05x |

> ⭐ Std-dev **~1/√vnodes** ki raftaar se girta hai — yaani 4 guna vnodes lagane se std-dev sirf
> **aadha** hota hai. Isi liye 500 → 1000 karne ka faayda bahut kam, par memory dugni.
> Yahi trade-off hai jispe Cassandra/DynamoDB/Riak **100-256** pe rukte hain.
>
> ⚠ **Trap:** ek hi ring ka std-dev bahut noisy hota hai — sirf ek sample dekh ke
> "vnodes badhane se faayda nahi" keh dena galat nateeja hai. Isliye demo har row pe
> **200 alag rings** (alag hash seed) banata hai aur average leta hai.

### Baaki jo demo naapta hai

| Kya | Result |
|-----|--------|
| Node add (4 → 5) pe keys moved | **18.6%** (theory ~1/N = 20%; `hash % N` me ~80% hoti) |
| Keys jinki 2 copies ek hi node pe | **0** ✅ |
| `DOWN` mark pe baaki nodes ki keys hili | **0** ✅ |
| `DOWN` pe migration tasks bane | **0** ⭐ (temporary failure pe data hilana bekaar) |
| Node remove pe bojh | saare bache nodes me bata (kisi ek pe nahi gira) |
| Weight=3 node ka ring share | baaki se ~3x |
| Hash function swap pe keys moved | **~74%** ⚠ (isi liye ye planned migration hai) |

## ⚠ Do galtiyan jo interview me pakdi jaati hain

**1. Replication me "agle N points" le lena**
```cpp
// GALAT — vnodes ke baad agle 3 points ek hi physical node ke ho sakte hain!
for (int i = 0; i < 3; i++) replicas.push_back((++it)->second);
```
`cache-1#7`, `cache-1#88`, `cache-1#3` — teeno **cache-1**. Aapki "3 copies" asal me ek hi server pe.
✅ Sahi: `set` se track karo, sirf naye **physical** node ko lo.

**2. `DOWN` ko `REMOVE` samajh lena**
Chhoti network glitch pe node remove karoge to har blip pe poora data migration shuru ho jaayega.
✅ Sahi: `DOWN` mark karo — ring positions wahi rehti hain, recovery free hoti hai.

## Related

- HLD theory + runnable step-by-step demos: [`HLD/Consistent_Hashing/`](../../HLD/Consistent_Hashing/README.md)
- HLD notes: [`19_Consistent_Hashing.md`](../../HLD/19_Consistent_Hashing.md)
- Sibling LLD (same Strategy pattern, but swap is free): [`LoadBalancer_LLD/`](../LoadBalancer_LLD/)

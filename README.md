# 🎯 LLD + HLD — Complete System Design Repository

<p align="center">
  <img src="https://readme-typing-svg.demolab.com?font=Fira+Code&weight=700&size=24&duration=2600&pause=800&color=2F81F7&center=true&vCenter=true&width=900&lines=LLD+%2B+HLD+Master+Repo;Low-Level+%2B+High-Level+Design;40+Lessons+%7C+42%2B+Systems;45+HLD+Topics+%7C+30+Case+Studies;C%2B%2B17+%7C+Interview+Ready" alt="Typing animation" />
</p>

<p align="center">
  <img src="https://img.shields.io/badge/LLD-C%2B%2B17-00599C?style=for-the-badge&logo=cplusplus" alt="LLD" />
  <img src="https://img.shields.io/badge/HLD-System%20Design-e63946?style=for-the-badge" alt="HLD" />
  <img src="https://img.shields.io/badge/Interview-Ready-16a34a?style=for-the-badge" alt="Interview" />
</p>

> This repo is organized into two halves — **LLD** (Low-Level Design: code, OOP, design patterns) and
> **HLD** (High-Level Design: system architecture, scalability). Both are essential for FAANG/product
> interviews. Notes & code comments are written in **Hinglish** for easy understanding.

---

## 📂 Two folders — LLD and HLD

```
repo/
├── LLD/    → Low-Level Design (code-level, C++)
└── HLD/    → High-Level Design (system design)
```

<table>
<tr>
<td width="50%" valign="top">

### 🧩 [`LLD/`](./LLD/) — Low-Level Design

**Design inside a single component** — classes, objects, design patterns, SOLID, C++.

- **40 lessons** (L0–L40): OOP → UML → SOLID → 23 GoF patterns
- **42+ system projects**: Parking Lot, GPay, IRCTC, Elevator, Splitwise, Chess...
- **`LLD_Interview.md`** — 3000+ line interview guide
- **UML diagrams** (Mermaid) + full pattern breakdowns
- Runnable **C++17** code (`./compile.sh` per project)

**👉 [Open the LLD folder](./LLD/README.md)**

</td>
<td width="50%" valign="top">

### 🏛️ [`HLD/`](./HLD/) — High-Level Design

**Architecture of the whole system** — scalability, DB, caching, distributed systems.

- **45 topic deep-dives** (21 core + 11 deep-dives + 13 advanced):
  Monolith/Microservices, Load Balancing, Caching, CDN, CAP, Sharding, Consistent Hashing,
  Consensus, Rate Limiting, Message Queues...
- **30 full system-design case studies**: TinyURL, Twitter, Uber, Payment/UPI, Google Maps, DynamoDB...
- **`HLD_Interview.md`** — 3800+ line interview guide (RESHADED + 20+ system designs) + a quick-revision **CHEATSHEET**
- **5 runnable C++ folders** ⭐ — HLD theory ko **chala kar, naap kar** verify karta hai
  (neeche dekho)
- Mermaid diagrams throughout

**👉 [Open the HLD folder](./HLD/README.md)**

</td>
</tr>
</table>

---

## 🚀 Quick Start

### LLD
```bash
cd LLD/Parking_lot_system_LLD
./compile.sh && ./parking_app
```

### HLD
```bash
# Read topics sequentially
open HLD/01_Monolithic_and_Microservices.md
# or the complete guide
open HLD/HLD_Interview.md
```

### HLD — runnable demos ⭐
```bash
cd HLD/Consistent_Hashing && ./compile.sh && ./02_hash_ring_basic_demo
```

### Sab kuch ek saath verify karo
```bash
./run_all.sh          # 31 demos build + run + invariants verify
```

> ⭐ Ye sirf compile check nahi hai. Har demo apne asli **invariants** verify karta hai —
> "Bloom filter me false negative kabhi nahi", "Count-Min kabhi kam nahi batata",
> "koi cache policy Belady optimal se upar nahi ja sakti". Invariant toota to demo
> **exit code 1** deta hai aur script fail ho jaati hai. GitHub Actions har push pe yahi chalata hai.

Baaki HLD theory-only hai; ye 4 folders usse **chala kar** dikhate hain — har demo apni
kamzori khud **naap kar** batata hai (formula vs asli measurement).

| Folder | Kya naap ke dikhata hai |
|---|---|
| [`Consistent_Hashing/`](./HLD/Consistent_Hashing/README.md) | `hash % N` pe node add karne se **80%** keys hilti hain, ring pe sirf **1/N** |
| [`Load_Balancer_Algorithms/`](./HLD/Load_Balancer_Algorithms/README.md) | Unequal servers pe plain Round Robin ki latency **40.96** vs Weighted RR **3.13** |
| [`Rate_Limiting_Algorithms/`](./HLD/Rate_Limiting_Algorithms/README.md) | Fixed window boundary pe **hamesha 2x** limit nikal jaati hai |
| [`Probabilistic_Data_Structures/`](./HLD/Probabilistic_Data_Structures/README.md) | Bloom FP rate formula se **0.6% ke andar**; HLL 1 arab items **16 KB** me |
| [`Cache_Eviction_Policies/`](./HLD/Cache_Eviction_Policies/README.md) | Ek scan LRU ka miss rate **0.5% → 5%** kar deta hai (DB pe 10x load) |

> 💡 **Diagrams render on GitHub automatically.** In VS Code, install the **"Markdown Preview Mermaid
> Support"** extension to see the Mermaid diagrams in preview.

---

## 🗺️ LLD vs HLD

| | **LLD** (`LLD/`) | **HLD** (`HLD/`) |
|---|---|---|
| Scope | inside one service/component | the whole system, all components |
| Output | class diagram, working code | architecture diagram, components |
| Focus | OOP, design patterns, SOLID, C++ | scalability, DB, caching, distributed systems |
| Sample question | "Design a parking lot" | "Design Instagram" |
| Interview round | LLD / coding round | system design round |

> The two are complementary — a single interview often has both an LLD round (code) and an HLD round (architecture).

---

## 📚 Interview prep — start here

| Guide | Folder | Size |
|---|---|---|
| 📘 [LLD Interview Guide](./LLD/LLD_Interview.md) | LLD | 3000+ lines |
| 📕 [HLD Interview Guide](./HLD/HLD_Interview.md) | HLD | 3800+ lines |
| ⚡ [HLD Quick Cheatsheet](./HLD/CHEATSHEET.md) | HLD | last-minute revision |
| 🏗️ [30 System Design Case Studies](./HLD/System_Design_Case_Studies/README.md) | HLD | end-to-end designs |
| 45 HLD topic deep-dives | [HLD](./HLD/README.md) | per-topic |
| 40 LLD lessons + 42 systems | [LLD](./LLD/README.md) | code |

---

## 📖 Recommended path
1. **Foundations** → LLD lessons L0–L6 (OOP, UML, SOLID)
2. **Patterns** → LLD L7–L40 (23 GoF patterns) + [`LLD_Interview.md`](./LLD/LLD_Interview.md)
3. **Systems (code)** → 5–7 LLD projects (Parking, LRU, Elevator, GPay, Splitwise)
4. **System design** → HLD topics + [30 case studies](./HLD/System_Design_Case_Studies/README.md) + [`HLD_Interview.md`](./HLD/HLD_Interview.md)
5. **Practice** → whiteboard LLD problems + apply the RESHADED framework to HLD problems

---

<p align="center">
  <b>LLD (code) + HLD (architecture) · C++17 · Interview-ready</b><br/>
  <i>Explore both folders — theory + practice, all in one place. Go crack it! 🚀</i>
</p>

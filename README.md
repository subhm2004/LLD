# 🎯 LLD + HLD — Complete System Design Repository

<p align="center">
  <img src="https://readme-typing-svg.demolab.com?font=Fira+Code&weight=700&size=24&duration=2600&pause=800&color=2F81F7&center=true&vCenter=true&width=900&lines=LLD+%2B+HLD+Master+Repo;Low-Level+%2B+High-Level+Design;40+Lessons+%7C+42%2B+Systems+%7C+21+HLD+Topics;C%2B%2B17+%7C+Interview+Ready" alt="Typing animation" />
</p>

<p align="center">
  <img src="https://img.shields.io/badge/LLD-C%2B%2B17-00599C?style=for-the-badge&logo=cplusplus" alt="LLD" />
  <img src="https://img.shields.io/badge/HLD-System%20Design-e63946?style=for-the-badge" alt="HLD" />
  <img src="https://img.shields.io/badge/Interview-Ready-16a34a?style=for-the-badge" alt="Interview" />
</p>

> Ye repo do halves me organized hai — **LLD** (Low-Level Design: code, OOP, design patterns) aur
> **HLD** (High-Level Design: system architecture, scalability). Dono FAANG/product interviews ke
> liye essential. Language: **Hinglish** (comments + docs).

---

## 📂 Do folders — LLD aur HLD

```
repo/
├── LLD/    → Low-Level Design (code-level, C++)
└── HLD/    → High-Level Design (system design)
```

<table>
<tr>
<td width="50%" valign="top">

### 🧩 [`LLD/`](./LLD/) — Low-Level Design

**Ek component ke andar ka design** — classes, objects, design patterns, SOLID, C++.

- **40 lessons** (L0–L40): OOP → UML → SOLID → 23 GoF patterns
- **42+ system projects**: Parking Lot, GPay, IRCTC, Elevator, Splitwise, Chess...
- **`LLD_Interview.md`** — 3000+ line interview guide
- **8 UML diagrams** (Mermaid) + 38 pattern breakdowns
- Runnable **C++17** code (`./compile.sh` per project)

**👉 [LLD folder kholo](./LLD/README.md)**

</td>
<td width="50%" valign="top">

### 🏛️ [`HLD/`](./HLD/) — High-Level Design

**Poore system ka architecture** — scalability, DB, caching, distributed systems.

- **21 core topics** (deep dives with diagrams):
  Monolith/Microservices, Load Balancing, Caching, CDN, CAP, Sharding, Consistent Hashing,
  Rate Limiting, Message Queues...
- **`HLD_Interview.md`** — 3800+ line interview guide (RESHADED + 20+ system designs)
- Mermaid diagrams throughout

**👉 [HLD folder kholo](./HLD/README.md)**

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
# 21 topics sequentially
open HLD/01_Monolithic_and_Microservices.md
# ya complete guide
open HLD/HLD_Interview.md
```

---

## 🗺️ LLD vs HLD  

| | **LLD** (`LLD/`) | **HLD** (`HLD/`) |
|---|---|---|
| Scope | ek service/component ke andar | poora system, sab components |
| Output | class diagram, working code | architecture diagram, components |
| Focus | OOP, design patterns, SOLID, C++ | scalability, DB, caching, distributed |
| Sample Q | "Design a parking lot" | "Design Instagram" |
| Interview round | LLD/coding round | system design round |

> Dono complementary — ek interview me LLD round (code) aur HLD round (architecture) dono aate hain.

---

## 📚 Interview prep — start here

| Guide | Folder | Lines |
|---|---|---|
| 📘 [LLD Interview Guide](./LLD/LLD_Interview.md) | LLD | 3000+ |
| 📕 [HLD Interview Guide](./HLD/HLD_Interview.md) | HLD | 3800+ |
| 21 HLD topic deep-dives | [HLD](./HLD/README.md) | per-topic |
| 40 LLD lessons + 42 systems | [LLD](./LLD/README.md) | code |

---

## 📖 Recommended path
1. **Foundations** → LLD lessons L0–L6 (OOP, UML, SOLID)
2. **Patterns** → LLD L7–L40 (23 GoF patterns) + [`LLD_Interview.md`](./LLD/LLD_Interview.md)
3. **Systems (code)** → 5-7 LLD projects (Parking, LRU, Elevator, GPay, Splitwise)
4. **System design** → HLD topics 01–21 + [`HLD_Interview.md`](./HLD/HLD_Interview.md)
5. **Practice** → whiteboard LLD problems + apply RESHADED to HLD problems

---

<p align="center">
  <b>LLD (code) + HLD (architecture) · C++17 · Hinglish · Interview-ready</b><br/>
  <i>Dono folders explore karo — theory + practice dono yahan. Crack kar de! 🚀</i>
</p>
